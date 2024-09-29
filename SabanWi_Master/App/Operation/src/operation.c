
#include "operation.h"
#include "timer_driver.h"
#include "DataFlash_stack.h"
#include "usb_stack.h"

#include "SX1278Fsk_Misc.h"
#include "SX1278-Fsk.h"
#include "SX1278LoRa_Misc.h"
#include "SX1278_LoRa.h"
#include "radio.h"

#include "RF_Transfer.h"

#include "modbus_master.h"

#include "mb.h"
#include "mbport.h"
#include "Modbus_User.h"

extern void Modbus_Master_Rece_Handler(void);

unsigned char Mode  = 0 ;
unsigned int timesendstart = 0;
unsigned int timesendstop = 0 ;
unsigned int timeresvstart = 0;
unsigned int timeresvstop = 0 ;

unsigned char device_pos = 0 ;               // STT client device
unsigned char RxBuf[64] = {0};
double RSSIvalue = 0 ;
unsigned char SNRvalue = 0 ;
unsigned char u8cmd ;

const int NUMBER_PORT_INPUT  = 8 ;
const int NUMBER_PORT_OUTPUT  = 8 ;

/*----------------------------------------------------------------*/
void Radio_Start(void)
{
    uint8_t version ;
    unsigned char CheckConfigErr = 0 ;
    unsigned int TimeOnAir = 0 ;
    uint32_t bitrate = 0  ;

    Mode = (MasterDataFlash[1].RfFrequence >> 4) & 0x0F ;
    if (Mode == LORA)
    {
        SX1276SetMode(LORA);
        SX1276Init();
        version = SX1276LoRaGetVersion();
        CheckConfigErr = SX1276CheckLoRaConfigError();
        while (version != 0x12 && CheckConfigErr != 0x0)
        {
            SX1276Init();
            //log_message("LoRa Init Fail !!!!!\n");
        }
        switch (MasterDataFlash[1].RfFrequence)
        {
        case 0x11 :
            SX1276LoRaSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 0 ;
            break ;
        case 0x12 :
            SX1276LoRaSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 0 ;
            break ;
        case 0x13 :
            SX1276LoRaSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 1 ;
            break ;
        case 0x14 :
            SX1276LoRaSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 1 ;
            break ;
        case 0x15 :
            SX1276LoRaSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 2 ;
            break ;
        case 0x16 :
            SX1276LoRaSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 2 ;
            break ;
        case 0x17 :
            SX1276LoRaSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 3 ;
            break ;
        case 0x18 :
            SX1276LoRaSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 3 ;
            break ;
        }

        SX1276LoRaSetRFPower(MasterDataFlash[1].RFPower);
        SX1276LoRaSetSignalBandwidth(MasterDataFlash[1].RFBandwidth);
        SX1276LoRaSetSpreadingFactor(MasterDataFlash[1].RFSpreadingFactor);
        SX1276LoRaSetErrorCoding(MasterDataFlash[1].ErrCode);

        //log_message("LoRa Init Done !!!!!\n");
        TimeOnAir = SX1276GetTimeOnAir();
        //log_message("LoRa TimeOnAir : %d [ms] !!!!!\n", TimeOnAir);
        SX1276StartCad();
    }

    else
    {
        SX1276SetMode(FSK);
        SX1276Init();
        version = SX1276FskGetVersion();
        while (version != 0x12)
        {
            SX1276Init();
            //log_message("FSK Init Fail !!!!!\n");
        }

        switch (MasterDataFlash[1].RfFrequence)
        {
        case 0x01 :
            SX1276FskSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 0 ;
            break ;
        case 0x02 :
            SX1276FskSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 0 ;
            break ;
        case 0x03 :
            SX1276FskSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 1 ;
            break ;
        case 0x04 :
            SX1276FskSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 1 ;
            break ;
        case 0x05 :
            SX1276FskSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 2 ;
            break ;
        case 0x06 :
            SX1276FskSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 2 ;
            break ;
        case 0x07 :
            SX1276FskSetRFFrequency(915000000);
            MasterDataFlash[1].Security = 3 ;
            break ;
        case 0x08 :
            SX1276FskSetRFFrequency(433000000);
            MasterDataFlash[1].Security = 3 ;
            break ;
        }
        bitrate = 0 << 24 | (uint32_t)MasterDataFlash[1].RFBandwidth << 16 | (uint32_t)MasterDataFlash[1].RFSpreadingFactor << 8 | (uint32_t)MasterDataFlash[1].ErrCode;
        SX1276FskSetBitrate(bitrate);
        //log_message("FSK Init Done !!!!!\n");
        TimeOnAir = SX1276FskGetTimeOnAir();
        //log_message("FSK TimeOnAir : %d [ms] !!!!!\n", TimeOnAir);
        SX1276StartRx();
    }
    device[1].Mode_work = MODE_WORK_HMI ;
}

/*--------------------------------------------------------------------------------------------------------------*/
/* ERR : device_[pos khong the vuot qua 0xC3 , loi  In Hard Fault Handler */
/*--------------------------------------------------------------------------------------------------------------*/

void OnMaster(void)
{
    unsigned char len ;
    unsigned char CheckID = 0;
    unsigned char t_check_decode_err = 0;

    uint8_t u8HMIData[60] = {0} ;

    hmi_pkg.addrHMI = 0x0a ;

    switch (SX1276Process())
    {
    case  RF_CHANNEL_ACTIVITY_DETECTED :
        //log_message(" \nChannel Busy !!!! ");
        break ;

    case  RF_CHANNEL_EMPTY:
        //log_message(" \nChannel empty !!!");
        if (device[1].Mode_work == MODE_WORK_NORMAL)
        {
            //log_message(" Client ID : %2X ", DeviceDataFlash[device_pos].ClientID);
            switch ((DeviceDataFlash[device_pos].Systemcode >> 4) & 0x0F)
            {
            case 0 :
                Saban_Mode_IO_Standand(DeviceDataFlash[device_pos].ClientID, NUMBER_PORT_INPUT, NUMBER_PORT_OUTPUT, DeviceDataFlash[device_pos].DataH, DeviceDataFlash[device_pos].DataL, MasterDataFlash[1].Security);
                break ;
            case 1 :
                Saban_Mode_RS485(DeviceDataFlash[device_pos].ClientID, 0x01, 0xFF, MasterDataFlash[1].Security);
                break ;
            case 2 :
                Saban_Mode_I2C(DeviceDataFlash[device_pos].ClientID, 0x01, 0xff, MasterDataFlash[1].Security);
                break ;
            case 3 :
                break ;
            }
        }
        else if (device[1].Mode_work == MODE_WORK_HMI)
        {
            Rf_Send_Request_HMIStatus(hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, hmi_pkg.HMIData);
        }
        Timer3_SetTickMs();
        timesendstart = Timer3_GetTickMs();
        break ;

    case RF_RX_RUNNING:
        //log_message(" RX Running !!! ");
        Timer3_SetTickMs();
        timeresvstart = Timer3_GetTickMs();
        break;

    case RF_RX_TIMEOUT :
        timeresvstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();                                           // Client disconnect
        //log_message(" RX Timeout !!![%d] us ", timeresvstop - timeresvstart);

        if (device[1].Mode_work == MODE_WORK_NORMAL)
        {
            pkg_client_recv[DeviceDataFlash[device_pos - 1].ClientID ].data_h = 0;
            pkg_client_recv[DeviceDataFlash[device_pos - 1].ClientID ].data_l = 0;
            pkg_client_recv[DeviceDataFlash[device_pos - 1].ClientID ].err ++ ;

            if (Mode == LORA)
            {
                SX1276StartCad();
            }
            if (Mode == FSK)
            {
                switch ((DeviceDataFlash[device_pos].Systemcode >> 4) & 0x0F)
                {
                case 0 :
                    Saban_Mode_IO_Standand(DeviceDataFlash[device_pos].ClientID, NUMBER_PORT_INPUT, NUMBER_PORT_OUTPUT, DeviceDataFlash[device_pos].DataH, DeviceDataFlash[device_pos].DataL, MasterDataFlash[1].Security);
                    break ;
                case 1 :
                    Saban_Mode_RS485(DeviceDataFlash[device_pos].ClientID, 0x01, 0xFF, MasterDataFlash[1].Security);
                    break ;
                case 2 :
                    Saban_Mode_I2C(DeviceDataFlash[device_pos].ClientID, 0x01, 0xff, MasterDataFlash[1].Security);
                    break ;
                case 3 :
                    break ;
                }
                Timer3_SetTickMs();
                timesendstart = Timer3_GetTickMs();
            }
        }
        else
        {
            //log_message(" HMI Not Connect !!! ");
            if (Mode == LORA)
            {
                SX1276StartCad();
            }
        }
        break ;

    case RF_RX_DONE :
        timeresvstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();                                                  // Client Connected
        RSSIvalue = SX1276ReadRssi();
        SNRvalue = SX1276GetPacketSnr();
        //log_message(" Rx Receive Done !!! [%d] us , Rssi : [%.2f] dBm , Snr : [%d] dB", timeresvstop - timeresvstart, RSSIvalue, SNRvalue);
        SX1276GetRxPacket(RxBuf, (unsigned short int)sizeof(RxBuf));
        if (RxBuf > 0)
        {
            if (device[1].Mode_work == MODE_WORK_NORMAL)
            {
                pkg_client_recv[DeviceDataFlash[device_pos - 1].ClientID].err = 0 ;
                switch (MasterDataFlash[1].Security)
                {
                case 0:
                    CheckID = Decode_Packet_Receive_CRC(Master, RxBuf);
                    break ;
                case 1:
                    CheckID = Decode_Packet_Receive_AESCRC(Master, RxBuf);
                    break ;
                case 2:
                    CheckID = Decode_Packet_Receive_SHA(Master, RxBuf);
                    break ;
                case 3:
                    CheckID = Decode_Packet_Receive_AESSHA(Master, RxBuf);
                    break ;
                }
                if (CheckID == 1)
                {
                    u8cmd = (DeviceDataFlash[ device_pos - 1].Systemcode & 0xF0) >> 4 ;
                    if (pkg_client_recv[device_pos - 1].cmd == u8cmd)
                    {
                        //log_message(" Client Connect !!!!! \n");
                    }
                    if (Mode == LORA)
                    {
                        SX1276StartCad();
                    }
                    if (Mode == FSK)
                    {
                        switch ((DeviceDataFlash[device_pos].Systemcode >> 4) & 0x0F)
                        {
                        case 0 :
                            Saban_Mode_IO_Standand(DeviceDataFlash[device_pos].ClientID, NUMBER_PORT_INPUT, NUMBER_PORT_OUTPUT, DeviceDataFlash[device_pos].DataH, DeviceDataFlash[device_pos].DataL, MasterDataFlash[1].Security);
                            break ;
                        case 1 :
                            Saban_Mode_RS485(DeviceDataFlash[device_pos].ClientID, 0x01, 0xFF, MasterDataFlash[1].Security);
                            break ;
                        case 2 :
                            Saban_Mode_I2C(DeviceDataFlash[device_pos].ClientID, 0x01, 0xff, MasterDataFlash[1].Security);
                            break ;
                        case 3 :
                            break ;
                        }
                        Timer3_SetTickMs();
                        timesendstart = Timer3_GetTickMs();
                    }
                }
                else
                {
                    SX1276StartCad();
                }
            }
            else
            {
                t_check_decode_err = Decode_Packet_Client_Feddback_HMIStatus(MASTER_GET_HMI_STATUS, RxBuf);
                if (t_check_decode_err == 0)
                {
                    if (device[1].Mode_work == MODE_WORK_HMI)
                    {
                        memcpy(u8HMIData, RxBuf + 3, 60);
                        SendHMIDataFromMasterToPC(&hmi_pkg, CMD_GET_HMI_STATUS, hmi_pkg.addrHMI, u8HMIData);
                        Rf_Send_Request_HMIStatus(hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, hmi_pkg.HMIData);
                    }
                    else if (device[1].Mode_work == MODE_WORK_HMI_FEEDBACK_HMI_LOGIN)
                    {
                        Rf_Send_Request_HMIStatus(hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, hmi_pkg.HMIData);
                    }
                }
                else
                {
                    SX1276StartCad();
                }
            }
        }
        else
        {
            //log_message(" RX RECEIVE ERR !!!!!");
        }
        break ;

    case RF_TX_RUNNING :
        if (device[1].Mode_work == MODE_WORK_NORMAL)
        {
            device_pos ++ ;
            if (device_pos > MasterDataFlash[1].DeviceNumber + 1)
            {
                device_pos = 0 ;
            }
            //log_message(" Tx Running Normal !!! ");
        }
        else
        {
            //log_message(" Tx Running HMI Request!!! ");
        }
        break ;

    case RF_TX_DONE :
        timesendstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        //log_message(" Tx Done !!![%d]us ", timesendstop - timesendstart);
        SX1276StartRx();
        break ;

    default :
        break ;
    }
}

void Modbus_Init(void)
{
    if (device[1].Modbus_mode == 0)
    {
        eMBErrorCode eStatus = MB_ENOERR;
        UseModbus() ;
    }
    if (device[1].Modbus_mode == 1)
    {
        UART_EnableInt(UART1, UART_IER_RDA_IEN_Msk);
        NVIC_EnableIRQ(UART1_IRQn);
        SB_Timer2_Init();
        TIMER_Start(TIMER2);
        ModbusMaster_begin();
    }

}

void Modbus_Start(void)
{
    uint8_t result = 0 ;
    uint16_t Modbus_result[16] = {0} ;

    if (device[1].Modbus_mode == 0)
    {
        (void)eMBPoll();                              // receive function code and response to Master
        MBGetData16Bits(REG_HOLDING, 1, Modbus_result) ;
        MBSetData16Bits(REG_HOLDING, 2, Modbus_result[0]);
    }
    if (device[1].Modbus_mode == 1)
    {
        result = ModbusMaster_readHoldingRegisters(MasterDataFlash[1].Modbus_SlaveID, MasterDataFlash[1].Modbus_Address, 15);  //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            Modbus_result[0] = ModbusMaster_getResponseBuffer(0);
            ModbusMaster_writeSingleRegister(MasterDataFlash[1].Modbus_SlaveID, 0x02, Modbus_result[0]);
        }
    }
}

void Modbus_Test_PC(void)
{
    uint8_t result = 0 ;
    uint8_t Modbus_result[16] = {0} ;
    uint16_t u16Modbus_result[1] = {0};
    uint16_t u16Modbus_total[16] = {0};
    int i = 0;

    if (device[1].Modbus_test == 0x01)
    {
        result = ModbusMaster_readHoldingRegisters(MasterDataFlash[1].Modbus_SlaveID, MasterDataFlash[1].Modbus_Address, 15);  //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            for (i = 0; i < 16; i++)
            {
                Modbus_result[i] = ModbusMaster_getResponseBuffer(i);
            }
            SendBackTestModbus(Modbus_result);
        }
    }
    if (device[1].Modbus_test == 0x02)
    {
        (void)eMBPoll();                              // receive function code and response to Master
        for (i = 0; i < 16; i++)
        {
            MBGetData16Bits(REG_HOLDING, i, u16Modbus_result) ;
            Modbus_result[i] = u16Modbus_result[0] ;
        }
        SendBackTestModbus(Modbus_result);
    }
}