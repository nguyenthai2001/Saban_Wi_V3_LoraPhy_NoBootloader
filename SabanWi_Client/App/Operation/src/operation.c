
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

void Radio_Start(void)
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
            //  printf("LoRa Init Fail !!!!!\n");
        }
        SX1276LoRaSetRFPower(ClientDataFlash[1].RFPower);
        SX1276LoRaSetSignalBandwidth(ClientDataFlash[1].RFBandwidth);
        SX1276LoRaSetSpreadingFactor(ClientDataFlash[1].RFSpreadingFactor);
        SX1276LoRaSetErrorCoding(ClientDataFlash[1].ErrCode);
        //            printf("\nLoRa Init Done !!!!!");
        TimeOnAir = SX1276GetTimeOnAir();
        //            printf(" TimeOnAir : %d[ms] \n", TimeOnAir);
        SX1276StartCad();
    }

    if (Mode == 0)
    {
        version = SX1276FskGetVersion();
        while (version != 0x12)
        {
            SX1276Init();
            //printf("FSK Init Fail !!!!!\n");
        }
        //printf("FSK Init Done !!!!!\n");
        TimeOnAir = SX1276FskGetTimeOnAir();
        //printf("FSK TimeOnAir : %d [ms] !!!!!\n", TimeOnAir);
        SX1276StartRx();
    }
    switch (ClientDataFlash[1].RfFrequence)
    {
    case 0x11 :
        ClientDataFlash[1].Security = 0 ;
        break ;
    case 0x12 :
        ClientDataFlash[1].Security = 0 ;
        break ;
    case 0x13 :
        ClientDataFlash[1].Security = 1 ;
        break ;
    case 0x14 :
        ClientDataFlash[1].Security = 1 ;
        break ;
    case 0x15 :
        ClientDataFlash[1].Security = 2 ;
        break ;
    case 0x16 :
        ClientDataFlash[1].Security = 2 ;
        break ;
    case 0x17 :
        ClientDataFlash[1].Security = 3 ;
        break ;
    case 0x18 :
        ClientDataFlash[1].Security = 3 ;
        break ;
    }
}

void OnClient(void)
{
    unsigned char CheckID = 0;
    uint16_t result = 0 ;
    //printf(" Rf_start  !!! \n");
    switch (SX1276Process())
    {
    case RF_CHANNEL_ACTIVITY_DETECTED :
        //printf("Channel Active !!! \n");
        break ;

    case RF_CHANNEL_EMPTY:
        //printf("Channel Empty !!! \n");
        SX1276StartCad();
        break ;

    case RF_RX_RUNNING:
        Timer3_SetTickMs();
        timercvstart = Timer3_GetTickMs();
        //printf("RX Running !!! ");
        break;

    case RF_RX_TIMEOUT :
        timercvstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        //printf("RX Timeout !!! [%d] us\n " , timercvstop - timercvstart);
        SX1276StartRx();
        break ;

    case RF_RX_DONE :
        timercvstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        //printf ("Rx Receive Done !!! [%d] us" , timercvstop - timercvstart);
        RSSIvalue = SX1276GetPacketRssi();
        SX1276GetRxPacket(RxBuf, (unsigned short int)sizeof(RxBuf));
        if (RxBuf > 0)
        {
            switch (ClientDataFlash[1].Security)
            {
            case 0:
                CheckID = Decode_Packet_Receive_CRC(Client, RxBuf);
                break ;
            case 1:
                CheckID = Decode_Packet_Receive_AESCRC(Client, RxBuf);
                break ;
            case 2:
                CheckID = Decode_Packet_Receive_SHA(Client, RxBuf);
                break ;
            case 3:
                CheckID = Decode_Packet_Receive_AESSHA(Client, RxBuf);
                break ;
            }
            if (CheckID == 1)
            {
                if (device[ClientDataFlash[1].SlaveID].cmd == CMD_IO_STANDAND)
                {
                    Saban_Output_Control();
                    Saban_Feedback_Mode_IOStandand(ClientDataFlash[1].Security);
                    Timer3_SetTickMs();
                    timesendstart = Timer3_GetTickMs();
                }
                if (device[ClientDataFlash[1].SlaveID].cmd == CMD_RS485)
                {
                    Saban_Feedback_Mode_RS485(ClientDataFlash[1].Modbus_Address, device[1].Modbus_value[0] & 0xFF, ClientDataFlash[1].Security);
                    Timer3_SetTickMs();
                    timesendstart = Timer3_GetTickMs();
                }
            }
            else
            {
                SX1276StartRx();
            }

        }
        else
        {
            //printf("RX RECEIVE ERR !!!!!\n");
        }
        break ;

    case RF_TX_RUNNING :
        //printf ("Tx Running !!! ");
        break ;

    case RF_TX_DONE :
        timesendstop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        //printf("Tx Done !!! [%d] us \n" , timesendstop - timesendstart);
        if (Mode == LORA)
        {
            SX1276StartCad();
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
        result = ModbusMaster_readHoldingRegisters(ClientDataFlash[1].Modbus_SlaveID, ClientDataFlash[1].Modbus_Address, 15);  //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            Modbus_result[0] = ModbusMaster_getResponseBuffer(0);
            ModbusMaster_writeSingleRegister(ClientDataFlash[1].Modbus_SlaveID, 0x02, Modbus_result[0]);
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
        result = ModbusMaster_readHoldingRegisters(0xF7, 0x01, 15); //master send readInputRegisters command to slave
        if (result == 0x00)
        {
            for (i = 0 ; i <= ClientDataFlash[1].Modbus_Address ; i++)
            {
                u16Modbus_result[i] = ModbusMaster_getResponseBuffer(i);
                SendBackTestModbus(i, u16Modbus_result);
            }
        }
    }
    if (device[1].Modbus_test == 0x02)
    {
        (void)eMBPoll();                              // receive function code and response to Master
        for (i = 0; i < 16; i++)
        {
            MBGetData16Bits(REG_HOLDING, i, u16Modbus_result) ;
            SendBackTestModbus(i, u16Modbus_result);
        }
    }
}