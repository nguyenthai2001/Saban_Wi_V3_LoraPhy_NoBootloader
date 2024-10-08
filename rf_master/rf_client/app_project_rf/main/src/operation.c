
#include "operation.h"
#include "DataFlash_stack.h"

#include "SX1278Fsk_Misc.h"
#include "SX1278-Fsk.h"
#include "SX1278LoRa_Misc.h"
#include "SX1278_LoRa.h"
#include "radio.h"

#include "RF_Transfer.h"

#include "gpio_control.h"

#include "modbus_master.h"
extern void Modbus_Master_Rece_Handler(void);

#include "mb.h"
#include "mbport.h"
#include "Modbus_User.h"

#include "usb_stack.h"

unsigned char Mode  = 0 ;
unsigned int timercvstart = 0;
unsigned int timercvstop = 0 ;
unsigned int timesendstart = 0;
unsigned int timesendstop = 0 ;

unsigned char RxBuf[64] = {0};
double RSSIvalue = 0 ;
unsigned char SNRvalue = 0 ;

uint8_t result = 0 ;
uint16_t Modbus_result[60] = {0} ;

data_user_pass hmi_user_pass ;

pkg_feedback_status_request packet_feedback_status_request ;
//pkg_feedback_status_request packet_feedback_status_request =
//{
//        .machineCode = "PL1",
//        .line = '1',
//        .lane = '1',
//        .partNumber = "FRNB-800A-20ADE-LM",
//        .slot = '2',
//        .Number = "30",
//        .level = "HIGH",
//        .status = "WAIT",
//              .user   = "",
//};

//void save_pkg_feedback_status_request(pkg_feedback_status_request *pkg, uint8_t *output, size_t output_size)
//{
//    // Xóa n?i dung m?ng output tru?c khi luu giá tr?
//
//    int index = 2;  // B?t d?u t? ph?n t? th? 2

//    // Luu machineCode (3 byte)
//    for (int i = 0; i < 3; i++) {
//        output[index++] = pkg->machineCode[i];
//    }
//    output[index++] = 0x1D;

//    // Luu line
//    output[index++] = pkg->line;
//    output[index++] = 0x1D;

//    // Luu lane
//    output[index++] = pkg->lane;
//    output[index++] = 0x1D;

//    // Luu partNumber (20 byte)
//    for (int i = 0; i < 20; i++) {
//        output[index++] = pkg->partNumber[i];
//    }
//    output[index++] = 0x1D;

//    // Luu slot
//    output[index++] = pkg->slot;
//    output[index++] = 0x1D;

//    // Luu Number (2 byte)
//    output[index++] = pkg->Number[0];
//    output[index++] = pkg->Number[1];
//    output[index++] = 0x1D;

//    // Luu level
//    output[index++] = pkg->level;
//    output[index++] = 0x1D;

//    // Luu status (5 byte, bao g?m chu?i "WAIT" và ký t? null)
//    for (int i = 0; i < 5; i++) {
//        output[index++] = pkg->status[i];
//    }
//    output[index++] = 0x1D;

//    // Luu user (30 byte)
//    for (int i = 0; i < 15; i++) {
//        output[index++] = pkg->user[i];
//    }
//    output[index++] = 0x1D;
//}

void save_pkg_feedback_status_request(pkg_feedback_status_request *pkg, uint8_t *output)
{
    int index = 2;

    for (int i = 0; i < 3; i++)
    {
        output[index++] = pkg->machineCode[i];
    }
    output[index++] = 0x1D;

    output[index++] = pkg->line;
    output[index++] = 0x1D;

    output[index++] = pkg->lane;
    output[index++] = 0x1D;

    for (int i = 0; i < 20; i++)
    {
        output[index++] = pkg->partNumber[i];
    }
    output[index++] = 0x1D;

    output[index++] = pkg->slot;
    output[index++] = 0x1D;

    for (int i = 0; i < 2; i++)
    {
        output[index++] = pkg->Number[i];
    }
    output[index++] = 0x1D;

    for (int i = 0; i < 5; i++)
    {
        output[index++] = pkg->level[i];
    }
    output[index++] = 0x1D;

    for (int i = 0; i < 5; i++)
    {
        output[index++] = pkg->status[i];
    }
    output[index++] = 0x1D;

    for (int i = 0; i < 10; i++)
    {
        output[index++] = pkg->user[i];
    }
    output[index++] = 0x1D;
}

void save_modbus_to_packet_feedback_status_request(pkg_feedback_status_request *pkg, uint16_t modbus_result[])
{
    int i, j = 0 ;
    uint8_t low_byte;
    uint8_t high_byte;

    pkg->machineCode[0] = modbus_result[2] & 0xFF ;
    pkg->machineCode[1] = (modbus_result[2] >> 8) & 0xFF ;
    pkg->machineCode[2] = modbus_result[3] & 0xFF ;
    pkg->line = modbus_result[5] & 0xFF ;
    pkg->lane = modbus_result[6] & 0xFF ;
    for (i = 7, j = 0; i < 13; i++)
    {
        uint8_t low_byte = modbus_result[i] & 0xFF;
        uint8_t high_byte = (modbus_result[i] >> 8) & 0xFF;

        if (low_byte != 0)
        {
            pkg->partNumber[j++] = low_byte;
        }
        if (high_byte != 0)
        {
            pkg->partNumber[j++] = high_byte;
        }
    }
    pkg->slot = modbus_result[17] & 0xFF;
    pkg->Number[0] = modbus_result[18] & 0xFF ;
    pkg->Number[1] = (modbus_result[18] >> 8) & 0xFF ;
    if (modbus_result[20] == 0x01)
    {
        strcpy(pkg->level, "MEDIUM");
    }
    if (modbus_result[20] == 0x00)
    {
        strcpy(pkg->level, "HIGH");
    }
    if (modbus_result[20] == 0x02)
    {
        strcpy(pkg->level, "LOW");
    }
}

void Radio_Start(Saban_Client_Dataflash *client_pkg)
{
    uint8_t version ;
    unsigned char CheckConfigErr = 0 ;
    unsigned int TimeOnAir = 0 ;

    Mode = SX1276SetMode(LORA);
    SX1276Init();
    if (Mode == LORA)
    {
        version = SX1276LoRaGetVersion();
        CheckConfigErr = SX1276CheckLoRaConfigError();
        while (version != 0x12 && CheckConfigErr != 0x0)
        {
            SX1276Init();
            //log_message("LoRa Init Fail !!!!!\n");
        }
        SX1276LoRaSetRFPower(client_pkg->RFPower);
        SX1276LoRaSetSignalBandwidth(client_pkg->RFBandwidth);
        SX1276LoRaSetSpreadingFactor(client_pkg->RFSpreadingFactor);
        SX1276LoRaSetErrorCoding(client_pkg->ErrCode);
        //log_message("\nLoRa Init Done !!!!!");
        TimeOnAir = SX1276GetTimeOnAir();
        //log_message(" TimeOnAir : %d[ms] \n", TimeOnAir);
        SX1276StartCad();
    }

    if (Mode == FSK)
    {
        version = SX1276FskGetVersion();
        while (version != 0x12)
        {
            SX1276Init();
            log_message("FSK Init Fail !!!!!\n");
        }
        log_message("FSK Init Done !!!!!\n");
        TimeOnAir = SX1276FskGetTimeOnAir();
        log_message("FSK TimeOnAir : %d [ms] !!!!!\n", TimeOnAir);
        SX1276StartRx();
    }
    switch (client_pkg->RfFrequence)
    {
    case 0x11 :
        client_pkg->Security = 0 ;
        break ;
    case 0x12 :
        client_pkg->Security = 0 ;
        break ;
    case 0x13 :
        client_pkg->Security = 1 ;
        break ;
    case 0x14 :
        client_pkg->Security = 1 ;
        break ;
    case 0x15 :
        client_pkg->Security = 2 ;
        break ;
    case 0x16 :
        client_pkg->Security = 2 ;
        break ;
    case 0x17 :
        client_pkg->Security = 3 ;
        break ;
    case 0x18 :
        client_pkg->Security = 3 ;
        break ;
    }
}

void OnClient(Saban *t_device, Saban_Client_Dataflash *client_pkg)
{
    unsigned char CheckID = 0;
    unsigned char check_decode_err = 0;
    unsigned char HMidata [60] = {0} ;
    uint16_t cnt_read_modbus = 0 ;
    size_t beforeSize = 0, afterSize = 0;

    switch (SX1276Process())
    {
    case RF_CHANNEL_ACTIVITY_DETECTED :
        log_message("Channel Active !!! \n");
        break ;

    case RF_CHANNEL_EMPTY:
        log_message("Channel Empty !!! \n");
        SX1276StartRx();
        break ;

    case RF_RX_RUNNING:
        Timer3_SetTickMs();
        timercvstart = Timer3_GetTickMs();
        log_message("RX Running !!! ");
        break;

    case RF_RX_TIMEOUT :
        timercvstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        log_message("RX Timeout !!! [%d] us\n ", timercvstop - timercvstart);
        SX1276StartCad();
        break ;

    case RF_RX_DONE :
        timercvstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        SX1276GetRxPacket(RxBuf, (unsigned short int)sizeof(RxBuf));
        if (RxBuf > 0)
        {
            switch (client_pkg->Security)
            {
            case 0:
                CheckID = Decode_Packet_Receive_CRC(t_device, client_pkg, Client, RxBuf);
                break ;
            case 1:
                CheckID = Decode_Packet_Receive_AESCRC(t_device, client_pkg, Client, RxBuf);
                break ;
            case 2:
                CheckID = Decode_Packet_Receive_SHA(t_device, client_pkg, Client, RxBuf);
                break ;
            case 3:
                CheckID = Decode_Packet_Receive_AESSHA(t_device, client_pkg, Client, RxBuf);
                break ;
            }
            if (CheckID == 1)
            {
                RSSIvalue = SX1276ReadRssi();
                SNRvalue = SX1276GetPacketSnr();
                log_message("Rx Receive Done !!! [%d] us , RSSI : [%.2f] , SNR : [%d] ", timercvstop - timercvstart, RSSIvalue, SNRvalue);
                if (t_device->cmd == CMD_IO_STANDAND)
                {
                    Saban_Output_Control(client_pkg);
                    Saban_Feedback_Mode_IOStandand(t_device, client_pkg, client_pkg->Security);
                    Timer3_SetTickMs();
                    timesendstart = Timer3_GetTickMs();
                }
                if (t_device->cmd == CMD_RS485)
                {
                    Saban_Feedback_Mode_RS485(t_device, client_pkg, client_pkg->Modbus_Address, t_device->Modbus_value[0] & 0xFF, client_pkg->Security);
                    Timer3_SetTickMs();
                    timesendstart = Timer3_GetTickMs();
                }
            }
            else
            {
                log_message("rx done hmi status !!!");
                check_decode_err = Decode_Package_Master_Send_HMIStatus(t_device, client_pkg, RxBuf);
                if (check_decode_err == 0)
                {
#if MODBUS_ENABLE
                    if (hmi_data_60byte.hmi_cmd == GET_STATE)                 // save user pass
                    {
                        MBGetData16Bits(REG_HOLDING, 3, &Modbus_result[0]);    // read state hmi feddback
                        MBGetData16Bits(REG_HOLDING, 4, &Modbus_result[1]);    // read cmd hmi feedback
                        Client_Get_HMI_Data(Modbus_result);                    // read data hmi feedback
                        copyUint16ToUint8(Modbus_result, HMidata, 30, 60);     // creat hmidata send feedback to master
                        splitArrayByByte(&HMidata[2], 58, hmi_user_pass.HMI_User, &beforeSize, hmi_user_pass.HMI_Pass, &afterSize);
                    }
                    if (hmi_data_60byte.hmi_cmd == SET_STATUS_LOGIN)            // send feedback set status log in
                    {
                        if (RxBuf[5] == 0x4F && RxBuf[6] == 0x4B)               // master feedback login ok
                        {
                            MBSetData16Bits(REG_HOLDING, 1, 1);                 // send modebus addr 0 , login ok
                        }
                        else
                        {
                            //MBSetData16Bits(REG_HOLDING, 1, 0);                 // send modbus addr 0 , login err
                        }

                        //Client_Get_HMI_Data(Modbus_result);                    // read data hmi feedback
                        //HMidata[0] = 0x2 ;
                        //HMidata[1] = 0x2 ;

                        MBGetData16Bits(REG_HOLDING, 3, &Modbus_result[0]);   // read state hmi feddback
                        MBGetData16Bits(REG_HOLDING, 4, &Modbus_result[1]);    // read cmd hmi feedback

                        Client_Get_HMI_Data(Modbus_result);
                        save_modbus_to_packet_feedback_status_request(&packet_feedback_status_request, Modbus_result);
                        strcpy(packet_feedback_status_request.status, "WAIT");
                        memcpy(packet_feedback_status_request.user, hmi_user_pass.HMI_User, 10);
                        HMidata[0] = Modbus_result[0] ;
                        HMidata[1] = Modbus_result[1] ;
                        save_pkg_feedback_status_request(&packet_feedback_status_request, HMidata);
                        //MBSetData16Bits(REG_HOLDING, 2, 1);
                    }
                    if (hmi_data_60byte.hmi_cmd == SET_STATUS_REQUEST)          // send feedback set status request
                    {
                        //MBSetData16Bits(REG_HOLDING, 1, 1);                   // send modebus addr 0 , login ok
                        /* set dia chi modbus cho man wait */
                        memset(HMidata, 0, 60);
                        MBSetData16Bits(REG_HOLDING, 2, 0);
                        HMidata[0] = 0x3;
                        HMidata[1] = 0x3;
                        if (RxBuf[5] == 0x4F && RxBuf[6] == 0x4B)                // master feedback check data ok
                        {
                            MBSetData16Bits(REG_HOLDING, 2, 1);
                        }
                        else                                                     // master feedback check data wait
                        {
                            MBSetData16Bits(REG_HOLDING, 2, 2);
                            MBSetData16Bits(REG_HOLDING, 1, 0);
                        }
                        memset(RxBuf, 0, sizeof(RxBuf));
                    }
#else
#endif
                    Rf_Send_Feedback_HMIStatus(t_device, client_pkg, MASTER_GET_HMI_STATUS, HMidata);      // send to master
                    Timer3_SetTickMs();
                    timesendstart = Timer3_GetTickMs();
                }
                else
                {
                    SX1276StartCad();
                }
                //SX1276StartRx();
            }
        }
        else
        {
            log_message("RX RECEIVE ERR !!!!!\n");
        }
        break ;

    case RF_TX_RUNNING :
        log_message("Tx Running !!! ");
        break ;

    case RF_TX_DONE :
        timesendstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        log_message("Tx Done !!! [%d] us \n", timesendstop - timesendstart);
        if (Mode == LORA)
        {
            //SX1276StartCad();
            SX1276StartRx();
        }
        if (Mode == FSK)
        {
            SX1276StartRx();
        }
        break ;

    default :
        break ;
    }
}

void Modbus_Init(Saban *t_device, Saban_Client_Dataflash *client_pkg)
{
    GPIO_SetMode(PA, BIT8, GPIO_PMD_OUTPUT);
    if (t_device->Modbus_mode == 0)
    {
        eMBErrorCode eStatus = MB_ENOERR;
        UseModbus(client_pkg->Modbus_SlaveID) ;
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

uint8_t Modbus_Start(Saban *t_device, Saban_Client_Dataflash *client_pkg)
{
    uint8_t err = 0 ;

    if (t_device->Modbus_mode == 0)
    {
        (void)eMBPoll();                              // receive function code and response to Master
        err = 0 ;
    }
    else if (t_device->Modbus_mode == 1)
    {
        result = ModbusMaster_readHoldingRegisters(client_pkg->Modbus_SlaveID, 0x01, 15);  //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            Modbus_result[0] = ModbusMaster_getResponseBuffer(0);
            ModbusMaster_writeSingleRegister(client_pkg->Modbus_SlaveID, 0x02, Modbus_result[0]);
            err  = 0 ;
        }
        else
        {
            err  = 1 ;
        }
    }
    else
    {
        err = 1 ;
    }
    return err ;
}

void Modbus_Test_PC(Saban *t_device, Saban_Client_Dataflash *client_pkg)
{
    uint8_t result = 0 ;
    uint8_t Modbus_result[16] = {0} ;
    uint16_t u16Modbus_result[2] = {0};
    uint16_t u16Modbus_total[16] = {0};
    int i = 0;

    if (t_device->Modbus_test == 0x02)
    {
        result = ModbusMaster_readHoldingRegisters(client_pkg->Modbus_SlaveID, 0x01, 15); //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            for (i = 0 ; i <= client_pkg->Modbus_Address ; i++)
            {
                u16Modbus_result[i] = ModbusMaster_getResponseBuffer(i);
                SendBackTestModbus(i, u16Modbus_result);
            }
        }
    }
    if (t_device->Modbus_test == 0x01)
    {
        (void)eMBPoll();                              // receive function code and response to Master
        for (i = 0; i < client_pkg->Modbus_Address; i++)
        {
            MBGetData16Bits(REG_HOLDING, i, u16Modbus_result) ;
            SendBackTestModbus(i, u16Modbus_result);
        }
    }
}

void Client_Get_HMI_Data(uint16_t t_modbus_result[])
{
    uint16_t cnt = 0 ;
    int i = 0 ;
    for (cnt = 5 ; cnt <= 40 ; cnt++)
    {
        MBGetData16Bits(REG_HOLDING, cnt, &t_modbus_result[ cnt - 3]);
    }
}

void Client_Set_HMI_User_Pass(void)
{
    int cnt_test = 0 ;
    uint8_t str1[30] = "" ;
    uint8_t str2[30] = "" ;
    short shortArray[15];
    short shortArray2[15];
    int copy_cnt  = 0 ;
    int i  = 0 ;
    memcpy(str1, hmi_user_pass.HMI_User, 30);
    memcpy(str2, hmi_user_pass.HMI_Pass, 30);
    int len1 = sizeof(str1);
    int len2 = sizeof(str2);

    for (i = 0; i < len1; i += 2)
    {
        if (i + 1 < len1)
        {
            shortArray[copy_cnt] = (short)((str1[i + 1] << 8) | str1[i]);
        }
        else
        {
            shortArray[copy_cnt] = (short) str1[i];
        }
        copy_cnt++;
    }

    for (i = 0; i < len2; i += 2)
    {
        if (i + 1 < len2)
        {
            shortArray2[copy_cnt] = (short)((str2[i + 1] << 8) | str2[i]);
        }
        else
        {
            shortArray2[copy_cnt] = (short) str2[i];
        }
        copy_cnt++;
    }
    for (cnt_test = 3 ; cnt_test < 30 ; cnt_test ++)
    {
        MBSetData16Bits(REG_HOLDING, cnt_test, shortArray[cnt_test - 3]);
    }
    for (cnt_test = 30 ; cnt_test < 60 ; cnt_test ++)
    {
        MBSetData16Bits(REG_HOLDING, cnt_test, shortArray2[cnt_test - 30]);
    }
}