
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
double ReceiverSensitivity = 0;

unsigned char u8cmd ;

int check_pc_feedback_hmi_status = 0;
int check_client_feedback_hmi_status = 0;
unsigned int check_pc_feedback_timeout = 0;

DecodeErrorCode errcode = NOERR ;

#define TIMER_CHECK_PC_FEDDBACK 0x493e0 /* 300 ms */

/*----------------------------------------------------------------*/
void Radio_Start(Saban_Master_Dataflash *master_pkg)
{
    uint8_t version ;
    unsigned char CheckConfigErr = 0 ;
    unsigned int TimeOnAir = 0 ;
    uint32_t bitrate = 0  ;

    Mode = (master_pkg->RfFrequence >> 4) & 0x0F ;
    if (Mode == LORA)
    {
        SX1276SetMode(LORA);
        SX1276Init();
        version = SX1276LoRaGetVersion();
        CheckConfigErr = SX1276CheckLoRaConfigError();
        while (version != 0x12 && CheckConfigErr != 0x0)
        {
            SX1276Init();
            log_message("LoRa Init Fail !!!!!\n");
        }
        switch (master_pkg->RfFrequence)
        {
        case 0x11 :
            SX1276LoRaSetRFFrequency(915000000);
            master_pkg->Security = 0 ;
            break ;
        case 0x12 :
            SX1276LoRaSetRFFrequency(433000000);
            master_pkg->Security = 0 ;
            break ;
        case 0x13 :
            SX1276LoRaSetRFFrequency(915000000);
            master_pkg->Security = 1 ;
            break ;
        case 0x14 :
            SX1276LoRaSetRFFrequency(433000000);
            master_pkg->Security = 1 ;
            break ;
        case 0x15 :
            SX1276LoRaSetRFFrequency(915000000);
            master_pkg->Security = 2 ;
            break ;
        case 0x16 :
            SX1276LoRaSetRFFrequency(433000000);
            master_pkg->Security = 2 ;
            break ;
        case 0x17 :
            SX1276LoRaSetRFFrequency(915000000);
            master_pkg->Security = 3 ;
            break ;
        case 0x18 :
            SX1276LoRaSetRFFrequency(433000000);
            master_pkg->Security = 3 ;
            break ;
        }

        SX1276LoRaSetRFPower(master_pkg->RFPower);
        SX1276LoRaSetSignalBandwidth(master_pkg->RFBandwidth);
        SX1276LoRaSetSpreadingFactor(master_pkg->RFSpreadingFactor);
        SX1276LoRaSetErrorCoding(master_pkg->ErrCode);

        log_message("LoRa Init Done !!!!!\n");
        TimeOnAir = SX1276GetTimeOnAir();
        log_message("LoRa TimeOnAir : %d [ms] !!!!!\n", TimeOnAir);
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
            log_message("FSK Init Fail !!!!!\n");
        }

        switch (master_pkg->RfFrequence)
        {
        case 0x01 :
            SX1276FskSetRFFrequency(915000000);
            master_pkg->Security = 0 ;
            break ;
        case 0x02 :
            SX1276FskSetRFFrequency(433000000);
            master_pkg->Security = 0 ;
            break ;
        case 0x03 :
            SX1276FskSetRFFrequency(915000000);
            master_pkg->Security = 1 ;
            break ;
        case 0x04 :
            SX1276FskSetRFFrequency(433000000);
            master_pkg->Security = 1 ;
            break ;
        case 0x05 :
            SX1276FskSetRFFrequency(915000000);
            master_pkg->Security = 2 ;
            break ;
        case 0x06 :
            SX1276FskSetRFFrequency(433000000);
            master_pkg->Security = 2 ;
            break ;
        case 0x07 :
            SX1276FskSetRFFrequency(915000000);
            master_pkg->Security = 3 ;
            break ;
        case 0x08 :
            SX1276FskSetRFFrequency(433000000);
            master_pkg->Security = 3 ;
            break ;
        }
        bitrate = 0 << 24 | (uint32_t)master_pkg->RFBandwidth << 16 | (uint32_t)master_pkg->RFSpreadingFactor << 8 | (uint32_t)master_pkg->ErrCode;
        SX1276FskSetBitrate(bitrate);
        log_message("FSK Init Done !!!!!\n");
        TimeOnAir = SX1276FskGetTimeOnAir();
        log_message("FSK TimeOnAir : %d [ms] !!!!!\n", TimeOnAir);
        SX1276StartRx();
    }
}

/*--------------------------------------------------------------------------------------------------------------*/
/* ERR : device_[pos khong the vuot qua 0xC3 , loi  In Hard Fault Handler */
/*--------------------------------------------------------------------------------------------------------------*/

void OnMaster(Saban *t_device, Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg, Saban_Device_Dataflash device_pkg[])
{
    unsigned char len ;
    unsigned char CheckID = 0;
    unsigned char t_check_decode_err = 0;

    uint8_t u8HMIData[60] = {0} ;

    hmi_pkg.addrHMI = 0x0a ;
    uint32_t TimeOnAir = 0;

    switch (SX1276Process())
    {
    case  RF_CHANNEL_ACTIVITY_DETECTED :
        log_message(" \nChannel busy !!!! ");
        break ;

    case  RF_CHANNEL_EMPTY:
        log_message(" \nChannel empty !!!");
        if (t_device->Mode_work == MODE_WORK_NORMAL)
        {
            log_message(" Client ID : %2X ", device_pkg[device_pos].ClientID);
            switch ((device_pkg[device_pos].Systemcode >> 4) & 0x0F)
            {
            case 0 :
                Saban_Mode_IO_Standand(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, device_pkg[device_pos].DataH, device_pkg[device_pos].DataL, master_pkg->Security);
                break ;
            case 1 :
                Saban_Mode_RS485(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, 0x01, 0xFF, master_pkg->Security);
                break ;
            case 2 :
                Saban_Mode_I2C(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, 0x01, 0xff, master_pkg->Security);
                break ;
            case 3 :
                break ;
            }
        }
        if (t_device->Mode_work == MODE_WORK_HMI)                       // get status hmi
        {
            if (check_pc_feedback_hmi_status == 0)
            {
                log_message("master send set parameter for get hmi status !!! ");
                RF_Send_Set_Parameter_HMIstatus(recv_pkg, master_pkg, hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, g_hmi_data_recv_pc);
                SX1276LoRaSetPayloadLength(64);
                TimeOnAir = SX1276GetTimeOnAir();
                SX1276LoRaSetRxPacketTimeout(TimeOnAir + 250);
            }
            else
            {
                log_message("master send to client get hmi status !!! ");
                Rf_Send_Request_HMIStatus(recv_pkg, master_pkg, hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, g_hmi_data_recv_pc); // send to client master get status hmi
            }
        }
        if (t_device->Mode_work == MODE_WORK_HMI_FEEDBACK_HMI_LOGIN)                       // set state log in
        {
            log_message("master send to client set status login !!!");
            Rf_Send_Request_HMIStatus(recv_pkg, master_pkg, hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, g_hmi_data_recv_pc); // send to client master set state log in
        }
        if (t_device->Mode_work == MODE_WORK_HMI_FEEDBACK_HMI_REQUEST)                       // set state log in
        {
            log_message("master send to client set status request login !!!");
            Rf_Send_Request_HMIStatus(recv_pkg, master_pkg, hmi_pkg.addrHMI, CMD_I2C, MCCODE_REQUEST_FEEDBACK, MASTER_GET_HMI_STATUS, g_hmi_data_recv_pc); // send to client master set state log in
        }
        //Timer3_SetTickMs();
        //timesendstart = Timer3_GetTickMs();
        break ;

    case RF_RX_RUNNING:
        log_message(" Rx running !!! ");
        //Timer3_SetTickMs();
        //timeresvstart = Timer3_GetTickMs();
        break;

    case RF_RX_TIMEOUT :
        //timeresvstop = Timer3_GetTickMs();
        //Timer3_ResetTickMs();                                           // Client disconnect
        log_message(" Rx timeout !!![%d] us ", timeresvstop - timeresvstart);

        if (t_device->Mode_work == MODE_WORK_NORMAL)
        {
            recv_pkg[device_pkg[device_pos - 1].ClientID ].data_h = 0;
            recv_pkg[device_pkg[device_pos - 1].ClientID ].data_l = 0;
            recv_pkg[device_pkg[device_pos - 1].ClientID ].err ++ ;

            if (Mode == LORA)
            {
                SX1276StartCad();
            }
            if (Mode == FSK)
            {
                switch ((device_pkg[device_pos].Systemcode >> 4) & 0x0F)
                {
                case 0 :
                    Saban_Mode_IO_Standand(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, device_pkg[device_pos].DataH, device_pkg[device_pos].DataL, master_pkg->Security);
                    break ;
                case 1 :
                    Saban_Mode_RS485(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, 0x01, 0xFF, master_pkg->Security);
                    break ;
                case 2 :
                    Saban_Mode_I2C(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, 0x01, 0xff, master_pkg->Security);
                    break ;
                case 3 :

                    break ;
                }
                //Timer3_SetTickMs();
                //timesendstart = Timer3_GetTickMs();
            }
        }
        else                                              // master get hmi status
        {
            log_message(" HMI Not Connect !!! ");
            if (Mode == LORA)
            {
                SX1276StartCad();
            }
        }
        break ;

    case RF_RX_DONE :
        //timeresvstop = Timer3_GetTickMs();
        //Timer3_ResetTickMs();                                                  // Client Connected
        RSSIvalue = SX1276ReadRssi();
        SNRvalue = SX1276GetPacketSnr();
        ReceiverSensitivity = SX1276GetReceiverSensitivity();
        log_message(" Rx Receive Done !!! [%d] us , Rssi : [%.2f] dBm , Snr : [%d] dB , RXSensi : [%.2f] dBm", timeresvstop - timeresvstart,
                    RSSIvalue, SNRvalue, ReceiverSensitivity);

        SX1276GetRxPacket(RxBuf, (unsigned short int)sizeof(RxBuf));
        if (RxBuf > 0)
        {
            /* Normal mode  */
            if (t_device->Mode_work == MODE_WORK_NORMAL)
            {
                recv_pkg[device_pkg[device_pos - 1].ClientID].err = 0 ;
                switch (master_pkg->Security)
                {
                case 0:
                    errcode = Decode_Packet_Receive_CRC(recv_pkg, master_pkg, RxBuf);
                    break ;
                case 1:
                    CheckID = Decode_Packet_Receive_AESCRC(recv_pkg, master_pkg, RxBuf);
                    break ;
                case 2:
                    CheckID = Decode_Packet_Receive_SHA(recv_pkg, master_pkg, RxBuf);
                    break ;
                case 3:
                    CheckID = Decode_Packet_Receive_AESSHA(recv_pkg, master_pkg, RxBuf);
                    break ;
                }
                if (errcode == NOERR)
                {
                    u8cmd = (device_pkg[ device_pos - 1].Systemcode & 0xF0) >> 4 ;
                    if (recv_pkg[device_pos - 1].cmd == u8cmd)
                    {
                        log_message(" client connect !!!!! ");
                        if (Mode == LORA)
                        {
                            SX1276StartCad();
                        }
                        if (Mode == FSK)
                        {
                            switch ((device_pkg[device_pos].Systemcode >> 4) & 0x0F)
                            {
                            case 0 :
                                Saban_Mode_IO_Standand(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, device_pkg[device_pos].DataH, device_pkg[device_pos].DataL, master_pkg->Security);
                                break ;
                            case 1 :
                                Saban_Mode_RS485(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, 0x01, 0xFF, master_pkg->Security);
                                break ;
                            case 2 :
                                Saban_Mode_I2C(recv_pkg, master_pkg, device_pkg[device_pos].ClientID, 0x01, 0xff, master_pkg->Security);
                                break ;
                            case 3 :
                                break ;
                            }
                            //Timer3_SetTickMs();
                            //timesendstart = Timer3_GetTickMs();
                        }
                    }
                }
                else
                {
                    log_message("master decode client feddback err !!! ");
                    SX1276StartCad();
                }
            }

            /* Goi lieu HMI  */
            else                                                                         // master recv data from client for master get hmi status
            {
                log_message("Check recv data !!! ");
                errcode = Decode_Packet_Client_Feddback_HMIStatus(master_pkg, MASTER_GET_HMI_STATUS, RxBuf);              // check data
                if (errcode == NOERR)                                                                              // decode data ok
                {
                    log_message("Check recv data OK !!! ");
                    if (t_device->Mode_work == MODE_WORK_HMI)                                                             // pc get user pass
                    {
                        check_pc_feedback_hmi_status = 1 ;
                        log_message("Check recv data : MODE_WORK_HMI !!! ");
                        memcpy(u8HMIData, RxBuf + 3, 60);
                        SendHMIDataFromMasterToPC(&hmi_pkg, CMD_GET_HMI_STATUS, hmi_pkg.addrHMI, u8HMIData);                                  // send user pass to pc
                        Timer3_SetTickMs();
                        check_pc_feedback_timeout = Timer3_GetTickMs();
                        while (check_pc_feedback_timeout < TIMER_CHECK_PC_FEDDBACK)
                        {
                            log_message("wait check pc feedback[%d] ", check_pc_feedback_timeout);
                            check_pc_feedback_timeout = Timer3_GetTickMs();
                        }
                        if (t_device->Mode_work == MODE_WORK_HMI)
                        {
                            t_device->Mode_work = MODE_WORK_NORMAL;
                        }
                        else
                        {
                            SX1276StartCad();
                        }
                    }
                    else if (t_device->Mode_work == MODE_WORK_HMI_FEEDBACK_HMI_LOGIN)                                                         // pc send user pass ok / err
                    {
                        log_message("Check recv data : MODE_WORK_HMI_FEEDBACK_HMI_LOGIN !!! ");
                        memcpy(u8HMIData, RxBuf + 3, 60);
                        SendHMIDataFromMasterToPC(&hmi_pkg, CMD_GET_HMI_STATUS, hmi_pkg.addrHMI, u8HMIData);
                        SX1276StartCad();
                    }
                    else if (t_device->Mode_work == MODE_WORK_HMI_FEEDBACK_HMI_REQUEST && RxBuf[6] != 0x43 && RxBuf[7] != 0x4C)
                    {
                        log_message("Check recv data : MODE_WORK_HMI_FEEDBACK_HMI_REQUEST !!! ");
                        memcpy(u8HMIData, RxBuf + 3, 60);
                        SendHMIDataFromMasterToPC(&hmi_pkg, CMD_GET_HMI_STATUS, hmi_pkg.addrHMI, u8HMIData);
                        SX1276StartCad();
                    }
                    else if (t_device->Mode_work == MODE_WORK_HMI_FEEDBACK_HMI_REQUEST && RxBuf[6] == 0x43 && RxBuf[7] == 0x4C)
                    {
                        log_message("Check recv data : MODE_WORK_HMI_FEEDBACK_HMI_REQUEST !!! ");
                        t_device->Mode_work = MODE_WORK_NORMAL ;
                        SX1276SetOpMode(RFLR_OPMODE_SLEEP);
                        SX1276SetOpMode(RFLR_OPMODE_STANDBY);
                        CLK_SysTickDelay(1000000);
                        SX1276StartCad();
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
            log_message(" Rx receive ERR !!!!!");
        }
        break ;

    case RF_TX_RUNNING :
        /* Normal mode */
        if (t_device->Mode_work == MODE_WORK_NORMAL)
        {
            device_pos ++ ;
            if (device_pos > master_pkg->DeviceNumber + 1)
            {
                device_pos = 0 ;
            }
            log_message(" Tx running normal mode !!! ");
        }
        /*Goi lieu mode */
        else
        {
            log_message(" Tx running HMI Request!!! ");
        }
        break ;

    case RF_TX_DONE :
        //timesendstop = Timer3_GetTickMs();
        //Timer3_ResetTickMs();
        /* normal mode */
        if (t_device->Mode_work == MODE_WORK_NORMAL)
        {
            log_message(" Tx done normal mode !!![%d]us ", timesendstop - timesendstart);
            SX1276StartRx();
        }
        /* Goi lieu mode */
        else
        {
            log_message(" Tx done goi lieu mode !!![%d]us ", timesendstop - timesendstart);
            SX1276StartRx();
        }
        break ;

    default :
        break ;
    }
}

void Modbus_Init(Saban *t_device)
{
    if (t_device->Modbus_mode == 0)
    {
        eMBErrorCode eStatus = MB_ENOERR;
        UseModbus() ;
    }
    if (t_device->Modbus_mode == 1)
    {
        UART_EnableInt(UART1, UART_IER_RDA_IEN_Msk);
        NVIC_EnableIRQ(UART1_IRQn);
        SB_Timer2_Init();
        TIMER_Start(TIMER2);
        ModbusMaster_begin();
    }

}

void Modbus_Start(Saban *t_device, Saban_Master_Dataflash *master_pkg)
{
    uint8_t result = 0 ;
    uint16_t Modbus_result[16] = {0} ;

    if (t_device->Modbus_mode == 0)
    {
        (void)eMBPoll();                              // receive function code and response to Master
        MBGetData16Bits(REG_HOLDING, 1, Modbus_result) ;
        MBSetData16Bits(REG_HOLDING, 2, Modbus_result[0]);
    }
    if (t_device->Modbus_mode == 1)
    {
        result = ModbusMaster_readHoldingRegisters(master_pkg->Modbus_SlaveID, master_pkg->Modbus_Address, 15);  //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            Modbus_result[0] = ModbusMaster_getResponseBuffer(0);
            ModbusMaster_writeSingleRegister(master_pkg->Modbus_SlaveID, 0x02, Modbus_result[0]);
        }
    }
}

void Modbus_Test_PC(Saban *t_device, Saban_Master_Dataflash *master_pkg)
{
    uint8_t result = 0 ;
    uint8_t Modbus_result[16] = {0} ;
    uint16_t u16Modbus_result[1] = {0};
    uint16_t u16Modbus_total[16] = {0};
    int i = 0;

    if (t_device-> Modbus_test == 0x01)
    {
        result = ModbusMaster_readHoldingRegisters(master_pkg->Modbus_SlaveID, master_pkg->Modbus_Address, 15);  //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            for (i = 0; i < 16; i++)
            {
                Modbus_result[i] = ModbusMaster_getResponseBuffer(i);
            }
            SendBackTestModbus(Modbus_result);
        }
    }
    if (t_device-> Modbus_test == 0x02)
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