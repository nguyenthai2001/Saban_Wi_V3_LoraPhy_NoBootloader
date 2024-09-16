
#include "SX1278-Fsk.h"
#include "radio.h"
#include "SX1278_HW.h"
#include "SX1278Fsk_Misc.h"
#include "timer_driver.h"

// Default settings
// Default settings
tFskSettings FskSettings =
{
    433000000,      // RFFrequency
    10000,           // Bitrate
    50000,          // Fdev
    13,             // Power
    100000,         // RxBw
    150000,         // RxBwAfc
    true,           // CrcOn
    true,           // AfcOn
    20,             // PayloadLength (set payload size to the maximum for variable mode, else set the exact payload length)
    12               // Preamble Length
};


/*!
 * SX1276FSk FSK registers variable
 */
tSX1276FSk *SX1276FSk ;

/*!
 * Local RF buffer for communication support
 */
static uint8_t RFBuffer[RF_BUFFER_SIZE];

/*!
 * Chunk size of data write in buffer
 */
static uint8_t DataChunkSize = 32;


/*!
 * RF state machine variable
 */
static uint8_t RFState = RF_STATE_IDLE;

/*!
 * Rx management support variables
 */

/*!
 * PacketTimeout holds the RF packet timeout
 * SyncSize = [0..8]
 * VariableSize = [0;1]
 * AddressSize = [0;1]
 * PayloadSize = [0..RF_BUFFER_SIZE]
 * CrcSize = [0;2]
 * PacketTimeout = ( ( 8 * ( VariableSize + AddressSize + PayloadSize + CrcSize ) / BR ) * 1000.0 ) + 1
 * Computed timeout is in miliseconds
 */
static uint32_t PacketTimeout;

/*!
 * Preamble2SyncTimeout
 * Preamble2SyncTimeout = ( ( 8 * ( PremableSize + SyncSize ) / RFBitrate ) * 1000.0 ) + 1
 * Computed timeout is in miliseconds
 */
static uint32_t Preamble2SyncTimeout;

static bool PreambleDetected = false;
static bool SyncWordDetected = false;
static bool PacketDetected = false;
static uint16_t RxPacketSize = 0;
static uint8_t RxBytesRead = 0;
static uint8_t TxBytesSent = 0;
static double RxPacketRssiValue;
static uint32_t RxPacketAfcValue;
static uint8_t RxGain = 1;
static uint32_t RxTimeoutTimer = 0;
static uint32_t Preamble2SyncTimer = 0;

unsigned short pream ;
/*!
 * Tx management support variables
 */
static uint16_t TxPacketSize = 0;
static uint32_t TxTimeoutTimer = 0;

uint8_t SX1276FskGetVersion(void)
{
    SX1276Read(REG_VERSION, &SX1276FSk->RegVersion);
    return SX1276FSk->RegVersion ;
}

uint32_t SX1276FskGetRxBw(void)
{
    return FskSettings.RxBw ;
}

uint32_t SX1276FskGetRxBwAfc(void)
{
    return FskSettings.RxBwAfc ;
}

void SX1276FskInit(void)
{

    RFState = RF_STATE_IDLE;

    SX1276FskSetDefaults();

    SX1276ReadBuffer(REG_OPMODE, SX1276Regs + 1, 0x70 - 1);

    // Set the device in FSK mode and Sleep Mode
    SX1276FSk->RegOpMode = RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_SLEEP;
    SX1276Write(REG_OPMODE,  SX1276FSk->RegOpMode);

    SX1276FSk->RegPaRamp = RF_PARAMP_MODULATIONSHAPING_01;
    SX1276Write(REG_PARAMP, SX1276FSk->RegPaRamp);

    SX1276FSk->RegLna = RF_LNA_GAIN_G1;
    SX1276Write(REG_LNA, SX1276FSk->RegLna);

    if (FskSettings.AfcOn == true)
    {
        SX1276FSk->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_ON |
                                 RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }
    else
    {
        SX1276FSk->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_OFF |
                                 RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }

    SX1276FSk->RegPreambleLsb = 8;

    SX1276FSk->RegPreambleDetect = RF_PREAMBLEDETECT_DETECTOR_ON | RF_PREAMBLEDETECT_DETECTORSIZE_2 |
                                   RF_PREAMBLEDETECT_DETECTORTOL_10;

    SX1276FSk->RegRssiThresh = 0xFF;

    SX1276FSk->RegSyncConfig = RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA |
                               RF_SYNCCONFIG_SYNC_ON |
                               RF_SYNCCONFIG_SYNCSIZE_4;

    SX1276FSk->RegSyncValue1 = 0x69;
    SX1276FSk->RegSyncValue2 = 0x81;
    SX1276FSk->RegSyncValue3 = 0x7E;
    SX1276FSk->RegSyncValue4 = 0x96;

    SX1276FSk->RegPacketConfig1 = RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RF_PACKETCONFIG1_DCFREE_OFF |
                                  (FskSettings.CrcOn << 4) | RF_PACKETCONFIG1_CRCAUTOCLEAR_ON |
                                  RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT;
    SX1276FskGetPacketCrcOn();  // Update CrcOn on FskSettings

    SX1276FSk->RegPayloadLength = FskSettings.PayloadLength;

    // we can now update the registers with our configuration
    SX1276WriteBuffer(REG_OPMODE, SX1276Regs + 1, 0x70 - 1);

    // then we need to set the RF settings
    SX1276FskSetRFFrequency(FskSettings.RFFrequency);
    SX1276FskSetBitrate(FskSettings.Bitrate);
    SX1276FskSetFdev(FskSettings.Fdev);

    SX1276FskSetDccBw(&SX1276FSk->RegRxBw, 0, FskSettings.RxBw);
    SX1276FskSetDccBw(&SX1276FSk->RegAfcBw, 0, FskSettings.RxBwAfc);
    SX1276FskSetRssiOffset(0);
    SX1276FskSetPaRamp(RF_PARAMP_3400_US);
    SX1276FskSetPreambleSize(FskSettings.PreambleLength);

    pream = SX1276FskGetPreambleSize();
    if (FskSettings.RFFrequency > 860000000)
    {
        SX1276FskSetPAOutput(RF_PACONFIG_PASELECT_RFO);
        SX1276FskSetPa20dBm(false);
        FskSettings.Power = 14;
        SX1276FskSetRFPower(FskSettings.Power);
    }
    else
    {
        SX1276FskSetPAOutput(RF_PACONFIG_PASELECT_PABOOST);
        SX1276FskSetPa20dBm(true);
        FskSettings.Power = 20;
        SX1276FskSetRFPower(FskSettings.Power);
    }

    SX1276FskSetOpMode(RF_OPMODE_STANDBY);

    // Calibrate the HF
    SX1276FskRxCalibrate();
}

void SX1276FskSetDefaults(void)
{
    // REMARK: See SX1276FSk datasheet for modified default values.
    SX1276Read(REG_VERSION, &(SX1276FSk->RegVersion));
}

void SX1276FskSetOpMode(uint8_t opMode)
{
    static uint8_t opModePrev = RF_OPMODE_STANDBY;
    static bool antennaSwitchTxOnPrev = true;
    bool antennaSwitchTxOn = false;

    opModePrev = SX1276FSk->RegOpMode & ~RF_OPMODE_MASK;

    if (opMode != opModePrev)
    {
        if (opMode == RF_OPMODE_TRANSMITTER)
        {
            antennaSwitchTxOn = true;
        }
        else
        {
            antennaSwitchTxOn = false;
        }
        if (antennaSwitchTxOn != antennaSwitchTxOnPrev)
        {
            antennaSwitchTxOnPrev = antennaSwitchTxOn;
        }
        SX1276FSk->RegOpMode = (SX1276FSk->RegOpMode & RF_OPMODE_MASK) | opMode;

        SX1276Write(REG_OPMODE, SX1276FSk->RegOpMode);
    }
}

uint8_t SX1276FskGetOpMode(void)
{
    SX1276Read(REG_OPMODE, &SX1276FSk->RegOpMode);

    return SX1276FSk->RegOpMode & ~RF_OPMODE_MASK;
}

int32_t SX1276FskReadFei(void)
{
    SX1276ReadBuffer(REG_FEIMSB, &SX1276FSk->RegFeiMsb, 2);                            // Reads the FEI value

    return (int32_t)(double)(((uint16_t)SX1276FSk->RegFeiMsb << 8) | (uint16_t)SX1276FSk->RegFeiLsb) * (double)FREQ_STEP;
}

int32_t SX1276FskReadAfc(void)
{
    SX1276ReadBuffer(REG_AFCMSB, &SX1276FSk->RegAfcMsb, 2);                              // Reads the AFC value
    return (int32_t)(double)(((uint16_t)SX1276FSk->RegAfcMsb << 8) | (uint16_t)SX1276FSk->RegAfcLsb) * (double)FREQ_STEP;
}

uint8_t SX1276FskReadRxGain(void)
{
    SX1276Read(REG_LNA, &SX1276FSk->RegLna);
    return (SX1276FSk->RegLna >> 5) & 0x07;
}

double SX1276FskReadRssi(void)
{
    SX1276Read(REG_RSSIVALUE, &SX1276FSk->RegRssiValue);                                 // Reads the RSSI value

    return -(double)((double)SX1276FSk->RegRssiValue / 2.0);
}

uint8_t SX1276FskGetPacketRxGain(void)
{
    return RxGain;
}

double SX1276FskGetPacketRssi(void)
{
    return RxPacketRssiValue;
}

uint32_t SX1276FskGetPacketAfc(void)
{
    return RxPacketAfcValue;
}

void SX1276FskStartRx(void)
{
    SX1276FskSetRFState(RF_STATE_RX_INIT);
}

void SX1276FskGetRxPacket(void *buffer, uint16_t size)
{
    size = RxPacketSize;
    RxPacketSize = 0;
    memcpy((void *)buffer, (void *)RFBuffer, (size_t) size);
}

void SX1276FskSetTxPacket(const void *buffer, uint16_t size)
{
    TxPacketSize = size;
    memcpy((void *)RFBuffer, buffer, (size_t)TxPacketSize);
    RFState = RF_STATE_TX_INIT;
}

// Remark: SX1276FSk must be fully initialized before calling this function
uint16_t SX1276FskGetPacketPayloadSize(void)
{
    uint16_t syncSize;
    uint16_t variableSize;
    uint16_t addressSize;
    uint16_t payloadSize;
    uint16_t crcSize;

    syncSize = (SX1276FSk->RegSyncConfig & 0x07) + 1;
    variableSize = ((SX1276FSk->RegPacketConfig1 & 0x80) == 0x80) ? 1 : 0;
    addressSize = ((SX1276FSk->RegPacketConfig1 & 0x06) != 0x00) ? 1 : 0;
    payloadSize = SX1276FSk->RegPayloadLength;
    crcSize = ((SX1276FSk->RegPacketConfig1 & 0x10) == 0x10) ? 2 : 0;

    return syncSize + variableSize + addressSize + payloadSize + crcSize;
}

// Remark: SX1276FSk must be fully initialized before calling this function
uint16_t SX1276FskGetPacketHeaderSize(void)
{
    uint16_t preambleSize;
    uint16_t syncSize;

    preambleSize = ((uint16_t)SX1276FSk->RegPreambleMsb << 8) | (uint16_t)SX1276FSk->RegPreambleLsb;
    syncSize = (SX1276FSk->RegSyncConfig & 0x07) + 1;

    return preambleSize + syncSize;
}

uint8_t SX1276FskGetRFState(void)
{
    return RFState;
}

void SX1276FskSetRFState(uint8_t state)
{
    RFState = state;
}

uint32_t SX1276FskProcess(void)
{
    uint32_t result = RF_BUSY;

    switch (RFState)
    {
    case RF_STATE_IDLE:
        break;
    // Rx management
    case RF_STATE_RX_INIT:
        // DIO mapping setup
        if ((SX1276FSk->RegPacketConfig1 & RF_PACKETCONFIG1_CRC_ON) == RF_PACKETCONFIG1_CRC_ON)
        {
            //                           CrcOk,                   FifoLevel,               SyncAddr,               FifoEmpty
            SX1276FSk->RegDioMapping1 = RF_DIOMAPPING1_DIO0_01 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_11 | RF_DIOMAPPING1_DIO3_00;
        }
        else
        {
            //                           PayloadReady,            FifoLevel,               SyncAddr,               FifoEmpty
            SX1276FSk->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_11 | RF_DIOMAPPING1_DIO3_00;
        }
        //                          Preamble,                   Data
        SX1276FSk->RegDioMapping2 = RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_10 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT;
        SX1276WriteBuffer(REG_DIOMAPPING1, &SX1276FSk->RegDioMapping1, 2);

        SX1276FskSetOpMode(RF_OPMODE_RECEIVER);

        memset(RFBuffer, 0, (size_t)RF_BUFFER_SIZE);

        PacketTimeout = (uint16_t)(round((8.0 * ((double)SX1276FskGetPacketPayloadSize()) / (double)FskSettings.Bitrate) * 1000.0) + 1.0);
        PacketTimeout = PacketTimeout + (PacketTimeout >> 1);   // Set the Packet timeout as 1.5 times the full payload transmission time

        Preamble2SyncTimeout = PacketTimeout;

        TIMER_Start(TIMER0);

        Preamble2SyncTimer = RxTimeoutTimer = Timer_GetTickMs();

        SX1276FSk->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x20; // 32 bytes of data
        SX1276Write(REG_FIFOTHRESH, SX1276FSk->RegFifoThresh);

        PreambleDetected = false;
        SyncWordDetected = false;
        PacketDetected = false;
        RxBytesRead = 0;
        RxPacketSize = 0;
        RFState = RF_STATE_RX_SYNC;
        break;
    case RF_STATE_RX_SYNC:
//          TIMER_Start(TIMER0);
        if ((DIO4 == 1) && (PreambleDetected == false))     // Preamble
        {
            PreambleDetected = true;
            Preamble2SyncTimer = Timer_GetTickMs();
        }
        if ((DIO2 == 1) && (PreambleDetected == true) && (SyncWordDetected == false))         // SyncAddr
        {
            SyncWordDetected = true;

            RxPacketRssiValue = SX1276FskReadRssi();

            RxPacketAfcValue = SX1276FskReadAfc();
            RxGain = SX1276FskReadRxGain();

            Preamble2SyncTimer = RxTimeoutTimer = Timer_GetTickMs();

            RFState = RF_STATE_RX_RUNNING;
        }

        // Preamble 2 SyncAddr timeout
        if ((SyncWordDetected == false) && (PreambleDetected == true) && ((Timer_GetTickMs() - Preamble2SyncTimer) > Preamble2SyncTimeout))
        {
            RFState = RF_STATE_RX_INIT;
            SX1276Write(REG_RXCONFIG, SX1276FSk->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK);
        }
        if ((SyncWordDetected == false) && (PreambleDetected == false) && (PacketDetected == false) && ((Timer_GetTickMs() - RxTimeoutTimer) > PacketTimeout))
        {
            Timer_ResetTickMs();
            RFState = RF_STATE_RX_TIMEOUT;
        }
        break;
    case RF_STATE_RX_RUNNING:
        if (RxPacketSize > RF_BUFFER_SIZE_MAX)
        {
            RFState = RF_STATE_RX_LEN_ERROR;
            break;
        }
#if 1
        if (DIO1 == 1)  // FifoLevel
        {
            if ((RxPacketSize == 0) && (RxBytesRead == 0))      // Read received packet size
            {
                if ((SX1276FSk->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE)
                {
                    SX1276ReadFifo((uint8_t*)&RxPacketSize, 1);
                }
                else
                {
                    RxPacketSize = SX1276FSk->RegPayloadLength;
                }
            }

            if ((RxPacketSize - RxBytesRead) > (SX1276FSk->RegFifoThresh & 0x3F))
            {
                SX1276ReadFifo((RFBuffer + RxBytesRead), (SX1276FSk->RegFifoThresh & 0x3F));
                RxBytesRead += (SX1276FSk->RegFifoThresh & 0x3F);
            }
            else
            {
                SX1276ReadFifo((RFBuffer + RxBytesRead), RxPacketSize - RxBytesRead);
                RxBytesRead += (RxPacketSize - RxBytesRead);
            }
        }
#endif
        if (DIO0 == 1)  // PayloadReady/CrcOk
        {
            RxTimeoutTimer = Timer_GetTickMs();
            if ((RxPacketSize == 0) && (RxBytesRead == 0))      // Read received packet size
            {
                if ((SX1276FSk->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE)
                {
                    SX1276ReadFifo((uint8_t*)&RxPacketSize, 1);
                }
                else
                {
                    RxPacketSize = SX1276FSk->RegPayloadLength;
                }
                SX1276ReadFifo(RFBuffer + RxBytesRead, RxPacketSize - RxBytesRead);
                RxBytesRead += (RxPacketSize - RxBytesRead);
                PacketDetected = true;
                RFState = RF_STATE_RX_DONE;
            }
            else
            {
                SX1276ReadFifo(RFBuffer + RxBytesRead, RxPacketSize - RxBytesRead);
                RxBytesRead += (RxPacketSize - RxBytesRead);
                PacketDetected = true;
                RFState = RF_STATE_RX_DONE;
            }
        }

        // Packet timeout
        if ((PacketDetected == false) && ((Timer_GetTickMs() - RxTimeoutTimer) > PacketTimeout))
        {
            RFState = RF_STATE_RX_TIMEOUT;
        }
        result = RF_RX_RUNNING;
        break;
    case RF_STATE_RX_DONE:
        RxBytesRead = 0;
        RFState = RF_STATE_RX_INIT;
        result = RF_RX_DONE;
        break;
    case RF_STATE_RX_TIMEOUT:
        RxBytesRead = 0;
        RxPacketSize = 0;
        SX1276Write(REG_RXCONFIG, SX1276FSk->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK);
        RFState = RF_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RF_STATE_RX_LEN_ERROR:
        RxBytesRead = 0;
        RxPacketSize = 0;
        SX1276Write(REG_RXCONFIG, SX1276FSk->RegRxConfig | RF_RXCONFIG_RESTARTRXWITHPLLLOCK);
        RFState = RF_STATE_RX_INIT;
        result = RF_LEN_ERROR;
        break;
    // Tx management
    case RF_STATE_TX_INIT:
        // Packet DIO mapping setup
        //                           PacketSent,               FifoLevel,              FifoFull,               TxReady
        SX1276FSk->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_01;
        //                           LowBat,                   Data
        SX1276FSk->RegDioMapping2 = RF_DIOMAPPING2_DIO4_00 | RF_DIOMAPPING2_DIO5_10;
        SX1276WriteBuffer(REG_DIOMAPPING1, &SX1276FSk->RegDioMapping1, 2);

        SX1276FSk->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x18; // 24 bytes of data
        SX1276Write(REG_FIFOTHRESH, SX1276FSk->RegFifoThresh);

        SX1276FskSetOpMode(RF_OPMODE_TRANSMITTER);
        RFState = RF_STATE_TX_READY_WAIT;
        TxBytesSent = 0;
        break;
    case RF_STATE_TX_READY_WAIT:
        if (DIO3 == 1)     // TxReady
        {
            if ((SX1276FSk->RegPacketConfig1 & RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE) == RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE)
            {
                SX1276WriteFifo((uint8_t*)&TxPacketSize, 1);
            }

            if ((TxPacketSize > 0) && (TxPacketSize <= 64))
            {
                DataChunkSize = TxPacketSize;
            }
            else
            {
                DataChunkSize = 32;
            }

            SX1276WriteFifo(RFBuffer, DataChunkSize);
            TxBytesSent += DataChunkSize;
            TIMER_Start(TIMER0);
            TxTimeoutTimer = Timer_GetTickMs();
            RFState = RF_STATE_TX_RUNNING;
            result = RF_TX_RUNNING;
        }
//        result = RF_TX_RUNNING;
        break;

    case RF_STATE_TX_RUNNING:
        if (DIO1 == 0)     // FifoLevel below thresold
        {
            if ((TxPacketSize - TxBytesSent) > DataChunkSize)
            {
                SX1276WriteFifo((RFBuffer + TxBytesSent), DataChunkSize);
                TxBytesSent += DataChunkSize;
            }
            else
            {
                // we write the last chunk of data
                SX1276WriteFifo(RFBuffer + TxBytesSent, TxPacketSize - TxBytesSent);
                TxBytesSent += TxPacketSize - TxBytesSent;
            }
        }

        if (DIO0 == 1)  // PacketSent
        {
            TxTimeoutTimer = Timer_GetTickMs();
            RFState = RF_STATE_TX_DONE;
            SX1276FskSetOpMode(RF_OPMODE_STANDBY);
        }

        // Packet timeout
        if ((Timer_GetTickMs() - TxTimeoutTimer) > 1000)
        {
            Timer_ResetTickMs();
            RFState = RF_STATE_TX_TIMEOUT;
        }
        break;
    case RF_STATE_TX_DONE:
        RFState = RF_STATE_IDLE;
        result = RF_TX_DONE;
        break;
    case RF_STATE_TX_TIMEOUT:
        RFState = RF_STATE_IDLE;
        result = RF_TX_TIMEOUT;
        break;
    default:
        break;
    }
    return result;
}

