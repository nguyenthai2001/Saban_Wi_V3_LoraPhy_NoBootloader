
#include "SX1278_LoRa.h"
#include "radio.h"
#include "SX1278_HW.h"
#include "SX1278LoRa_Misc.h"

unsigned char rxSnrEstimate;

/*!
 * Constant values need to compute the RSSI value
 */
#define RSSI_OFFSET_LF                              -164.0
#define RSSI_OFFSET_HF                              -157.0

/*!
 * Frequency hopping frequencies table
 */
const unsigned long int HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};

// Default settings

tLoRaSettings LoRaSettings =
{
    433000000,        // RFFrequency
    17,               // Power
    8,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
    // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    8,               // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    1,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    1,                // CrcOn [0: OFF, 1: ON]
    0,                // ImplicitHeaderOn [0: OFF, 1: ON]
    1,                // RxSingleOn [0: Continuous, 1 Single]
    0,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every frequency hopping period symbols
    10,              // TxPacketTimeout (default, data will be update by TimeOutSetting())
    150,              // RxPacketTimeout (the same as TxPacketTimeout)
    48,                // PayloadLength
    12,               // PreambleLength
};

//SignalBW table
const unsigned long int BW[] = {7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000};
const unsigned char CRdeno[] = {5, 6, 7, 8};
/*!
 * SX1276 LoRa registers variable
 */
tSX1276LR *SX1276LR;

/*!
 * Local RF buffer for communication support
 */
static unsigned char RFBuffer[RF_BUFFER_SIZE];

/*!
 * RF state machine variable
 */
unsigned char RFLRState = RFLR_STATE_IDLE;

/*!
 * Rx management support variables
 */
static unsigned int RxPacketSize = 0;
static int8_t RxPacketSnrEstimate;
static double RxPacketRssiValue;
static unsigned char RxGain = 1;


/*!
 * PacketTimeout Stores the Rx window time value for packet reception
 */
static uint32_t PacketTimeout;
uint32_t RxTimeoutTimer = 0;

/*!
 * Tx management support variables
 */
static unsigned int TxPacketSize = 0;

uint8_t SX1276LoRaGetVersion(void)
{
    SX1276Read(REG_LR_VERSION, &SX1276LR->RegVersion);
    return SX1276LR->RegVersion ;
}

void SX1276LoRaInit(void)
{
    RFLRState = RFLR_STATE_IDLE;

    SX1276LoRaSetDefaults();

    SX1276ReadBuffer(REG_LR_OPMODE, SX1276Regs + 1, 0x70 - 1);

    SX1276LR->RegLna = RFLR_LNA_GAIN_G1;

    SX1276WriteBuffer(REG_LR_OPMODE, SX1276Regs + 1, 0x70 - 1);

    // set the RF settings
    SX1276LoRaSetRFFrequency(LoRaSettings.RFFrequency);
    SX1276LoRaSetSpreadingFactor(LoRaSettings.SpreadingFactor);   // SF6 only operates in implicit header mode.
    SX1276LoRaSetErrorCoding(LoRaSettings.ErrorCoding);
    SX1276LoRaSetPacketCrcOn(LoRaSettings.CrcOn);
    SX1276LoRaSetSignalBandwidth(LoRaSettings.SignalBw);

    SX1276LoRaSetImplicitHeaderOn(LoRaSettings.ImplicitHeaderOn);
    SX1276LoRaSetSymbTimeout(0x3FF);
    SX1276LoRaSetPayloadLength(LoRaSettings.PayloadLength);
    SX1276LoRaSetPreambleLength(LoRaSettings.PreambleLength);
    SX1276LoRaSetLowDatarateOptimize(TRUE);

    SX1276LoRaSetPAOutput(RFLR_PACONFIG_PASELECT_PABOOST);
    SX1276LoRaSetPa20dBm(TRUE);
    SX1276LoRaSetRFPower(LoRaSettings.Power);

    SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);
}

void SX1276LoRaSetDefaults(void)
{
    // REMARK: See SX1276 datasheet for modified default values.
    SX1276Read(REG_LR_VERSION, &SX1276LR->RegVersion);
}

void SX1276LoRaSetOpMode(unsigned char opMode)
{
    static unsigned char opModePrev = RFLR_OPMODE_STANDBY;
    opModePrev = SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;

    if (opMode != opModePrev)
    {
        SX1276LR->RegOpMode = (SX1276LR->RegOpMode & RFLR_OPMODE_MASK) | opMode;

        SX1276Write(REG_LR_OPMODE, SX1276LR->RegOpMode);
    }
}

unsigned char SX1276LoRaGetOpMode(void)
{
    SX1276Read(REG_LR_OPMODE, &SX1276LR->RegOpMode);

    return SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;
}

unsigned char SX1276LoRaReadRxGain(void)
{
    SX1276Read(REG_LR_LNA, &SX1276LR->RegLna);
    return (SX1276LR->RegLna >> 5) & 0x07;
}

double SX1276LoRaReadRssi(void)
{
    // Reads the RSSI value
    SX1276Read(REG_LR_RSSIVALUE, &SX1276LR->RegRssiValue);

    if (LoRaSettings.RFFrequency < 860000000)   // LF
    {
        return (double)RSSI_OFFSET_LF + (double)SX1276LR->RegRssiValue;
    }
    else
    {
        return (double)RSSI_OFFSET_HF + (double)SX1276LR->RegRssiValue;
    }
}

unsigned char SX1276LoRaGetPacketRxGain(void)
{
    return RxGain;
}

char SX1276LoRaGetPacketSnr(void)
{
    return RxPacketSnrEstimate;
}

double SX1276LoRaGetPacketRssi(void)
{
    return RxPacketRssiValue;
}

void SX1276LoRaStartRx(void)
{
    SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
}

void SX1276LoRaGetRxPacket(void *buffer, unsigned short int size_in)
{
    size_in = RxPacketSize;
    RxPacketSize = 0;
    memcpy((void *)buffer, (void *)RFBuffer, (size_t)size_in);
}

void SX1276LoRaSetTxPacket(const void *buffer, unsigned short int size_in)
{
    TxPacketSize = size_in;
    memcpy((void *)RFBuffer, buffer, (size_t)TxPacketSize);
    RFLRState = RFLR_STATE_TX_INIT;
}

unsigned char SX1276LoRaGetRFState(void)
{
    return RFLRState;
}

void SX1276LoRaSetRFState(unsigned char state)
{
    RFLRState = state;
}

extern unsigned char g_u8IsTargetAbortINTFlag;
/*!
 * \brief Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY,
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
unsigned long int SX1276LoRaProcess(void)
{
    unsigned short int result = RF_BUSY;

    switch (RFLRState)
    {
    case RFLR_STATE_IDLE:
        break;

    case RFLR_STATE_RX_INIT:
        SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

        SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    //RFLR_IRQFLAGS_RXDONE |
                                    //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1276Write(REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask);

        if (LoRaSettings.FreqHopOn == TRUE)
        {
            SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

            SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
            SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
        }
        else
        {
            SX1276LR->RegHopPeriod = 255;
        }

        SX1276Write(REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod);

        //                         RxDone                     RxTimeout                  FhssChangeChannel          CadDone
        SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
        //                         CadDetected                ModeReady
        SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer(REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2);

        if (LoRaSettings.RxSingleOn == TRUE)   // Rx single mode
        {
            SX1276LoRaSetOpMode(RFLR_OPMODE_RECEIVER_SINGLE);
        }
        else // Rx continuous mode
        {
            SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
            SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);
            SX1276LoRaSetOpMode(RFLR_OPMODE_RECEIVER);
        }

        memset(RFBuffer, 0, (size_t)RF_BUFFER_SIZE);

        PacketTimeout =  LoRaSettings.RxPacketTimeout;
        TIMER_Start(TIMER0);
        RxTimeoutTimer = Timer_GetTickMs();
        RFLRState = RFLR_STATE_RX_RUNNING;
        result = RF_RX_RUNNING;
        break;

    case RFLR_STATE_RX_RUNNING:
        if (DIO0 == 1)   // RxDone
        {
            RxTimeoutTimer = Timer_GetTickMs();
            if (LoRaSettings.FreqHopOn == TRUE)
            {
                SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
                SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
            }

            // Clear Irq
            SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);
            RFLRState = RFLR_STATE_RX_DONE;
        }

        if (DIO2 == 1)   // FHSS Changed Channel
        {
            if (LoRaSettings.FreqHopOn == TRUE)
            {
                SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
                SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
            }

            // Clear Irq
            SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL);
            // Debug
            RxGain = SX1276LoRaReadRxGain();
        }

        if (LoRaSettings.RxSingleOn == TRUE)
        {
            if (Timer_GetTickMs() - RxTimeoutTimer > PacketTimeout)
            {
                Timer_ResetTickMs();
                RFLRState = RFLR_STATE_RX_TIMEOUT;
            }
        }

        break;

    case RFLR_STATE_RX_DONE:
        SX1276Read(REG_LR_IRQFLAGS, &SX1276LR->RegIrqFlags);

        if ((SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR)
        {
            // Clear Irq
            SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR);

            if (LoRaSettings.RxSingleOn == TRUE)   // Rx single mode
            {
                RFLRState = RFLR_STATE_RX_INIT;
            }
            else
            {
                RFLRState = RFLR_STATE_RX_RUNNING;
            }
            break;
        }

        {
            SX1276Read(REG_LR_PKTSNRVALUE, &rxSnrEstimate);

            if (rxSnrEstimate & 0x80)   // The SNR sign bit is 1
            {
                // Invert and divide by 4
                RxPacketSnrEstimate = ((~rxSnrEstimate + 1) & 0xFF) >> 2;
                RxPacketSnrEstimate = -RxPacketSnrEstimate;
            }
            else
            {
                // Divide by 4
                RxPacketSnrEstimate = (rxSnrEstimate & 0xFF) >> 2;
            }
        }

        SX1276Read(REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue);

        if (LoRaSettings.RFFrequency < 860000000)   // LF
        {
            if (RxPacketSnrEstimate < 0)
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_LF + ((double)SX1276LR->RegPktRssiValue) + RxPacketSnrEstimate;
            }
            else
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_LF + (1.0666 * ((double)SX1276LR->RegPktRssiValue));
            }
        }
        else                                        // HF
        {
            if (RxPacketSnrEstimate < 0)
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_HF + ((double)SX1276LR->RegPktRssiValue) + RxPacketSnrEstimate;
            }
            else
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_HF + (1.0666 * ((double)SX1276LR->RegPktRssiValue));
            }
        }

        if (LoRaSettings.RxSingleOn == TRUE)   // Rx single mode
        {
            SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
            SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);

            if (LoRaSettings.ImplicitHeaderOn == TRUE)
            {
                RxPacketSize = SX1276LR->RegPayloadLength;
                SX1276ReadFifo(RFBuffer, SX1276LR->RegPayloadLength);
            }
            else
            {
                SX1276Read(REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes);
                RxPacketSize = SX1276LR->RegNbRxBytes;
                SX1276ReadFifo(RFBuffer, SX1276LR->RegNbRxBytes);
            }
        }
        else // Rx continuous mode
        {
            SX1276Read(REG_LR_FIFORXCURRENTADDR, &SX1276LR->RegFifoRxCurrentAddr);

            if (LoRaSettings.ImplicitHeaderOn == TRUE)
            {
                RxPacketSize = SX1276LR->RegPayloadLength;
                SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
                SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);
                SX1276ReadFifo(RFBuffer, SX1276LR->RegPayloadLength);
            }
            else
            {
                SX1276Read(REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes);
                RxPacketSize = SX1276LR->RegNbRxBytes;
                SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
                SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);
                SX1276ReadFifo(RFBuffer, SX1276LR->RegNbRxBytes);
            }
        }

        if (LoRaSettings.RxSingleOn == TRUE)   // Rx single mode
        {
            RFLRState = RFLR_STATE_RX_INIT;
        }
        else // Rx continuous mode
        {
            RFLRState = RFLR_STATE_RX_RUNNING;
        }
        result = RF_RX_DONE;
        break;

    case RFLR_STATE_RX_TIMEOUT:
        RFLRState = RFLR_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;

    case RFLR_STATE_TX_INIT:

        SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

        if (LoRaSettings.FreqHopOn == TRUE)
        {
            SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

            SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
            SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
        }
        else
        {
            SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            SX1276LR->RegHopPeriod = 0;
        }

        SX1276Write(REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod);
        SX1276Write(REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask);

        // Initializes the payload size
        SX1276LR->RegPayloadLength = TxPacketSize;
        SX1276Write(REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength);

        SX1276LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1276Write(REG_LR_FIFOTXBASEADDR, SX1276LR->RegFifoTxBaseAddr);

        SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoTxBaseAddr;
        SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);

        // Write payload buffer to LORA modem
        SX1276WriteFifo(RFBuffer, SX1276LR->RegPayloadLength);
        //                         TxDone                     RxTimeout                  FhssChangeChannel          ValidHeader
        SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
        //                         PllLock                    Mode Ready
        SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer(REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2);

        SX1276LoRaSetOpMode(RFLR_OPMODE_TRANSMITTER);

        RFLRState = RFLR_STATE_TX_RUNNING;
        result = RF_TX_RUNNING;
        break;

    case RFLR_STATE_TX_RUNNING:

        if (DIO0 == 1)   // TxDone
        {
            // Clear Irq
            SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
            RFLRState = RFLR_STATE_TX_DONE;
        }

        if (DIO2 == 1)   // FHSS Changed Channel
        {
            if (LoRaSettings.FreqHopOn == TRUE)
            {
                SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
                SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
            }

            // Clear Irq
            SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL);
        }

        break;

    case RFLR_STATE_TX_DONE:
        // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
        SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

        RFLRState = RFLR_STATE_IDLE;

        result = RF_TX_DONE;
        break;

    case RFLR_STATE_CAD_INIT:
        SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

        SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_RXDONE |
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    //RFLR_IRQFLAGS_CADDONE |
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL; // |
        //RFLR_IRQFLAGS_CADDETECTED;
        SX1276Write(REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask);

        //                         RxDone                     RxTimeout                  FhssChangeChannel          CadDone
        SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
        //                         CAD Detected               ModeReady
        SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer(REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2);

        SX1276LoRaSetOpMode(RFLR_OPMODE_CAD);
        RFLRState = RFLR_STATE_CAD_RUNNING;
        break;

    case RFLR_STATE_CAD_RUNNING:
        if (DIO3 == 1)   //CAD Done interrupt
        {
            // Clear Irq
            SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE);

            if (DIO4 == 1)   // CAD Detected interrupt
            {
                // Clear Irq
                SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED);
                // CAD detected, we have a LoRa preamble
                RFLRState = RFLR_STATE_RX_INIT;
                result = RF_CHANNEL_ACTIVITY_DETECTED;
            }
            else
            {
                // The device goes in Standby Mode automatically
                RFLRState = RFLR_STATE_IDLE;
                result = RF_CHANNEL_EMPTY;
            }
        }

        break;

    default:
        break;
    }

    return result;
}

void RX_Init(void)
{
    SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

    SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                //RFLR_IRQFLAGS_RXDONE |
                                //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                RFLR_IRQFLAGS_VALIDHEADER |
                                RFLR_IRQFLAGS_TXDONE |
                                RFLR_IRQFLAGS_CADDONE |
                                //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                RFLR_IRQFLAGS_CADDETECTED;
    SX1276Write(REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask);

    if (LoRaSettings.FreqHopOn == TRUE)
    {
        SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

        SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
        SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
    }
    else
    {
        SX1276LR->RegHopPeriod = 255;
    }

    SX1276Write(REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod);

    //                         RxDone                     RxTimeout                  FhssChangeChannel          CadDone
    SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
    //                         CadDetected                ModeReady
    SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2);

    if (LoRaSettings.RxSingleOn == TRUE)   // Rx single mode
    {

        SX1276LoRaSetOpMode(RFLR_OPMODE_RECEIVER_SINGLE);
    }
    else // Rx continuous mode
    {
        SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
        SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);

        SX1276LoRaSetOpMode(RFLR_OPMODE_RECEIVER);
    }

    memset(RFBuffer, 0, (size_t)RF_BUFFER_SIZE);

    RFLRState = RFLR_STATE_RX_RUNNING;
}

void RX_Done(void)
{
    if (LoRaSettings.FreqHopOn == TRUE)
    {
        SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
        SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
    }

    // Clear Irq
    SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);

    SX1276Read(REG_LR_IRQFLAGS, &SX1276LR->RegIrqFlags);

    if ((SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR)
    {
        // Clear Irq
        SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR);
        RFLRState = RFLR_STATE_RX_RUNNING;
    }
    else
    {
        unsigned char rxSnrEstimate;
        SX1276Read(REG_LR_PKTSNRVALUE, &rxSnrEstimate);

        if (rxSnrEstimate & 0x80)   // The SNR sign bit is 1
        {
            // Invert and divide by 4
            RxPacketSnrEstimate = ((~rxSnrEstimate + 1) & 0xFF) >> 2;
            RxPacketSnrEstimate = -RxPacketSnrEstimate;
        }
        else
        {
            // Divide by 4
            RxPacketSnrEstimate = (rxSnrEstimate & 0xFF) >> 2;
        }

        SX1276Read(REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue);

        if (LoRaSettings.RFFrequency < 860000000)   // LF
        {
            if (RxPacketSnrEstimate < 0)
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_LF + ((double)SX1276LR->RegPktRssiValue) + RxPacketSnrEstimate;
            }
            else
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_LF + (1.0666 * ((double)SX1276LR->RegPktRssiValue));
            }
        }
        else                                        // HF
        {
            if (RxPacketSnrEstimate < 0)
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_HF + ((double)SX1276LR->RegPktRssiValue) + RxPacketSnrEstimate;
            }
            else
            {
                RxPacketRssiValue = (double)RSSI_OFFSET_HF + (1.0666 * ((double)SX1276LR->RegPktRssiValue));
            }
        }

        // Rx continuous mode
        SX1276Read(REG_LR_FIFORXCURRENTADDR, &SX1276LR->RegFifoRxCurrentAddr);

        if (LoRaSettings.ImplicitHeaderOn == TRUE)
        {
            RxPacketSize = SX1276LR->RegPayloadLength;
            SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
            SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);
            SX1276ReadFifo(RFBuffer, SX1276LR->RegPayloadLength);
        }
        else
        {
            SX1276Read(REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes);
            RxPacketSize = SX1276LR->RegNbRxBytes;
            SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
            SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);
            SX1276ReadFifo(RFBuffer, SX1276LR->RegNbRxBytes);
        }

        RFLRState = RFLR_STATE_RX_RUNNING;
    }
}

void TX_Init(void)
{
    SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

    if (LoRaSettings.FreqHopOn == TRUE)
    {
        SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_RXDONE |
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    //RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

        SX1276Read(REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel);
        SX1276LoRaSetRFFrequency(HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK]);
    }
    else
    {
        SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_RXDONE |
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    //RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1276LR->RegHopPeriod = 0;
    }

    SX1276Write(REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod);
    SX1276Write(REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask);

    // Initializes the payload size
    SX1276LR->RegPayloadLength = TxPacketSize;
    SX1276Write(REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength);

    SX1276LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
    SX1276Write(REG_LR_FIFOTXBASEADDR, SX1276LR->RegFifoTxBaseAddr);

    SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoTxBaseAddr;
    SX1276Write(REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr);

    // Write payload buffer to LORA modem
    SX1276WriteFifo(RFBuffer, SX1276LR->RegPayloadLength);
    //                         TxDone                     RxTimeout                  FhssChangeChannel          ValidHeader
    SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
    //                         PllLock                    Mode Ready
    SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2);

    SX1276LoRaSetOpMode(RFLR_OPMODE_TRANSMITTER);

    RFLRState = RFLR_STATE_TX_RUNNING;
}

void TX_Done(void)
{
    SX1276Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
    RFLRState = RFLR_STATE_TX_DONE ;
}

void SX1276_RX_COUNT_TIMER(void)
{
    SysTick->LOAD = (uint32_t)9000 * (LoRaSettings.RxPacketTimeout) ;
    SysTick->VAL = 0x00 ;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SX1276_TX_COUNT_TIMER(void)
{
    SysTick->LOAD = (uint32_t)9000 * (LoRaSettings.TxPacketTimeout) ;
    SysTick->VAL = 0x00 ;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

static uint32_t SX1276GetLoRaBandwidthInHz(uint32_t bw)
{
    uint32_t bandwidthInHz = 0;

    switch (bw)
    {
    case 7: // 125 kHz
        bandwidthInHz = 125000UL;
        break;
    case 8: // 250 kHz
        bandwidthInHz = 250000UL;
        break;
    case 9: // 500 kHz
        bandwidthInHz = 500000UL;
        break;
    }

    return bandwidthInHz;
}

uint32_t SX1276GetTimeOnAir(void)
{
    uint32_t numerator = 0;
    uint32_t denominator = 1;


    numerator   = 1000U * SX1276GetLoRaTimeOnAirNumerator(LoRaSettings.SignalBw, LoRaSettings.SpreadingFactor, LoRaSettings.ErrorCoding,
                  LoRaSettings.PreambleLength, LoRaSettings.PayloadLength, LoRaSettings.CrcOn);
    denominator = SX1276GetLoRaBandwidthInHz(LoRaSettings.SignalBw);


    // Perform integral ceil()
    return (numerator + denominator - 1) / denominator;
}

static uint32_t SX1276GetLoRaTimeOnAirNumerator(uint32_t bandwidth,
        uint32_t datarate, uint8_t coderate,
        uint16_t preambleLen, uint8_t payloadLen,
        uint8_t crcOn)
{
    int32_t crDenom           = coderate + 4;
    uint8_t    lowDatareOptimize = 0;

    // Ensure that the preamble length is at least 12 symbols when using SF5 or
    // SF6
    if ((datarate == 5) || (datarate == 6))
    {
        if (preambleLen < 12)
        {
            preambleLen = 12;
        }
    }

    if (((bandwidth == 0) && ((datarate == 11) || (datarate == 12))) ||
            ((bandwidth == 1) && (datarate == 12)))
    {
        lowDatareOptimize = 1;
    }

    int32_t ceilDenominator;
    int32_t ceilNumerator = (payloadLen << 3) +
                            (crcOn ? 16 : 0) -
                            (4 * datarate) +  20 ;

    if (datarate <= 6)
    {
        ceilDenominator = 4 * datarate;
    }
    else
    {
        ceilNumerator += 8;

        if (lowDatareOptimize == 1)
        {
            ceilDenominator = 4 * (datarate - 2);
        }
        else
        {
            ceilDenominator = 4 * datarate;
        }
    }

    if (ceilNumerator < 0)
    {
        ceilNumerator = 0;
    }

    // Perform integral ceil()
    int32_t intermediate =
        ((ceilNumerator + ceilDenominator - 1) / ceilDenominator) * crDenom + preambleLen + 12;

    if (datarate <= 6)
    {
        intermediate += 2;
    }

    return (uint32_t)((4 * intermediate + 1) * (1 << (datarate - 2)));
}

uint8_t SX1276CheckLoRaConfigError(void)
{
    unsigned long int CheckFreq = 0 ;
    char CheckPower = 0 ;
    unsigned char CheckBw = 0;
    unsigned char CheckSF = 0 ;
    unsigned char CheckCodingrate = 0 ;
    unsigned char CheckCRCon = 0;
    unsigned char CheckPayloadlength = 0;
    uint8_t result = 0 ;

    CheckFreq = SX1276LoRaGetRFFrequency();
    CheckPower = SX1276LoRaGetRFPower();
    CheckBw = SX1276LoRaGetSignalBandwidth();
    CheckSF = SX1276LoRaGetSpreadingFactor();
    CheckCodingrate = SX1276LoRaGetErrorCoding();
    CheckCRCon = SX1276LoRaGetPacketCrcOn() ;
    CheckPayloadlength = SX1276LoRaGetPayloadLength();

    if (CheckFreq != LoRaSettings.RFFrequency) result = 1 ;
    if (CheckPower != LoRaSettings.Power) result = 2 ;
    if (CheckBw != LoRaSettings.SignalBw) result = 3;
    if (CheckSF != LoRaSettings.SpreadingFactor) result = 4 ;
    if (CheckCodingrate != LoRaSettings.ErrorCoding) result = 5 ;
    if (CheckCRCon != LoRaSettings.CrcOn) result = 6 ;
    if (CheckPayloadlength != LoRaSettings.PayloadLength) result = 7 ;
    else result = 0;
    return result ;
}

