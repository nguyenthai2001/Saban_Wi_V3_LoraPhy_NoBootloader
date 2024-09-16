
#include "usb_stack.h"
#include "hid_transfer.h"
#include "DataFlash_stack.h"
#include "SX1278.h"
#include "SX1278Fsk_Misc.h"
#include "SX1278LoRa_Misc.h"
#include "SX1278_LoRa.h"
#include "gpio_driver.h"

#include "mb.h"
#include "mbport.h"
#include "Modbus_User.h"

/**
*    @brief         SendBackDeviceSettingInfo
*
*    @param[in]
*    @param[in]
*    @param[in]
*    @return        None
*    @details       Gu thong tin cau hinh thiet bi
*/
void SendBackDeviceSettingInfo(void)
{
    uint8_t *ptr;

    uint8_t TxData[64] = {0};
    unsigned long int CheckFreq = 0 ;
    char CheckPower = 0 ;
    unsigned char CheckBw = 0;
    unsigned char CheckSF = 0 ;
    unsigned char CheckCodingrate = 0 ;
    unsigned long int Rxtimeout = 0 ;
    unsigned short modbustimeout = 0 ;
    uint8_t byte1 = 0 ;
    uint8_t byte2 = 0 ;
    uint8_t byte3 = 0 ;
    uint8_t byte4 = 0 ;

    uint8_t byteh = 0;
    uint8_t bytel = 0 ;

    CheckFreq = SX1276LoRaGetRFFrequency();
    CheckPower = SX1276LoRaGetRFPower();
    CheckBw = SX1276LoRaGetSignalBandwidth();
    CheckSF = SX1276LoRaGetSpreadingFactor();
    CheckCodingrate = SX1276LoRaGetErrorCoding();
    Rxtimeout = SX1276LoRaGetRxPacketTimeout();

    byte1 = ((Rxtimeout >> 24) & 0xFF); // Byte cao nh?t
    byte2 = ((Rxtimeout >> 16) & 0xFF);
    byte3 = ((Rxtimeout >> 8) & 0xFF);
    byte4 = (Rxtimeout & 0xFF); // Byte th?p nh?t

    modbustimeout = ClientDataFlash[1].Modbus_Timeout ;
    byteh = ((modbustimeout >> 8) & 0xFF);
    bytel = (modbustimeout & 0xFF); // Byte th?p nh?t

    if (CheckFreq == 0x19CF0E40)
    {
        TxData[0] = 0x01 ;
    }
    if (CheckFreq == 0x3689CAC0)
    {
        TxData[0] = 0x02 ;
    }
    TxData[1] = CheckPower ;
    TxData[2] = CheckBw ;
    TxData[3] = CheckSF ;
    TxData[4] = CheckCodingrate ;
    TxData[5] = ClientDataFlash[1].System_code;
    TxData[6] = ClientDataFlash[1].SlaveID ;
    TxData[7] = byte1 ;
    TxData[8] = byte2 ;
    TxData[9] = byte3 ;
    TxData[10] = byte4 ;

    TxData[11] = ClientDataFlash[1].Modbus_SlaveID ;
    TxData[12] = ClientDataFlash[1].Modbus_Baudrate ;
    TxData[13] = ClientDataFlash[1].Modbus_Register ;
    TxData[14] = ClientDataFlash[1].Modbus_Address ;
    TxData[15] = byteh ;
    TxData[16] = bytel ;

    TxData[17] = ClientDataFlash[1].Security ;

    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

/**
*    @brief         SendBack
*
*    @param[in]
*    @param[in]
*    @param[in]
*    @return        None
*    @details       Thiet lap thong tin gui len PC qua USB
*/
void SendBack(uint8_t result)
{
    uint8_t *ptr;
    uint8_t TxData[64] = {0};

    TxData[0] = 0x2;
    TxData[1] = result;

    uint32_t TimeonAir ;

    TimeonAir = SX1276GetTimeOnAir();
    SX1276LoRaSetRxPacketTimeout(TimeonAir + 20);

    TxData[2] = ((TimeonAir >> 24) & 0xFF);
    TxData[3] = ((TimeonAir >> 16) & 0xFF);
    TxData[4] = ((TimeonAir >> 8) & 0xFF);
    TxData[5] = (TimeonAir & 0xFF);

    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

/**
*    @brief         SendBackModbus
*
*    @param[in]
*    @param[in]
*    @param[in]
*    @return        None
*    @details       Thiet lap thong tin gui len PC qua USB
*/
void SendBackTestModbus(uint8_t address, uint16_t *result)
{
    uint8_t *ptr;
    uint8_t TxData[64] = {0};
    uint16_t rs485data[1] = {0} ;
    uint8_t mode = 0 ;

    TxData[0] = result[0] << 8;
    TxData[1] = result[0] ;
    TxData[2] = address ;

    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen)
{
    uint8_t u8Cmd;
    uint8_t u8Errno = 0;

    u8Cmd = pu8Buffer[0];
    uint8_t MasterID = 0;
    uint8_t ClientID = 0;
    uint8_t system_code = 0 ;
    uint8_t Commnad = 0;
    uint8_t MCCode = 0;

    uint8_t inputtype = 0 ;
    uint8_t byte1 = 0;
    uint8_t byte2 = 0;
    uint8_t byte3 = 0;
    uint8_t byte4 = 0 ;
    uint8_t byte5 = 0;
    uint8_t byte6 = 0;

    int restoredValue = 0;
    uint16_t MBtimeout ;
    uint8_t mode = 0 ;

    uint8_t err ;

    switch (u8Cmd)
    {
    case CMD_READ_ADD:
        SendBackDeviceSettingInfo();
        break;

    case CMD_WRITE_ADD :
        MasterID = pu8Buffer[1];
        ClientID = pu8Buffer[2];
        Commnad = device[ClientID].cmd ;
        MCCode = device[ClientID].mccode ;
        system_code = (MasterID << 4) | (Commnad << 2) | MCCode ;
        err =  Update_DataFlash_ID_From_PC(system_code, ClientID);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            SendBack(0);
        }
        CLK_SysTickDelay(1000);
        /* Reset CPU */
        SYS_ResetCPU();
        break ;

    case CMD_WRITE_INPUT_TYPE :
        inputtype = pu8Buffer[1];
        byte1 = pu8Buffer[2];
        byte2 = pu8Buffer[3];
        byte3 = pu8Buffer[4];
        byte4 = pu8Buffer[5];
        restoredValue = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
        err = Update_DataFlash_InputType_From_PC(inputtype, restoredValue);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            SB_Client_GPIO_Init();
            SendBack(0);
            CLK_SysTickDelay(1000);
            /* Reset CPU */
            SYS_ResetCPU();
        }
        break ;

    case CMD_WRITE_RF_CONFIG :
        byte1 = pu8Buffer[1];
        byte2 = pu8Buffer[2];
        byte3 = pu8Buffer[3];
        byte4 = pu8Buffer[4];
        byte5 = pu8Buffer[5];
        byte6 = pu8Buffer[6];

        err = Update_DataFlash_RFConfig_From_PC(byte1, byte2, byte3, byte4, byte5, byte6);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            switch (ClientDataFlash[1].RfFrequence)
            {
            case 0x11 :
                SX1276LoRaSetRFFrequency(915000000);
                break ;
            case 0x12 :
                SX1276LoRaSetRFFrequency(433000000);
                break ;
            case 0x13 :
                SX1276LoRaSetRFFrequency(915000000);
                break ;
            case 0x14 :
                SX1276LoRaSetRFFrequency(433000000);
                break ;
            case 0x15 :
                SX1276LoRaSetRFFrequency(915000000);
                break ;
            case 0x16 :
                SX1276LoRaSetRFFrequency(433000000);
                break ;
            case 0x17 :
                SX1276LoRaSetRFFrequency(915000000);
                break ;
            case 0x18 :
                SX1276LoRaSetRFFrequency(433000000);
                break ;
            }

            SX1276LoRaSetRFPower(ClientDataFlash[1].RFPower);
            SX1276LoRaSetSignalBandwidth(ClientDataFlash[1].RFBandwidth);
            SX1276LoRaSetSpreadingFactor(ClientDataFlash[1].RFSpreadingFactor);
            SX1276LoRaSetErrorCoding(ClientDataFlash[1].ErrCode);
            SendBack(0);
            CLK_SysTickDelay(1000);
            /* Reset CPU */
            SYS_ResetCPU();
        }
        break;
    case CMD_WRITE_MODBSU_CONFIG :
        byte1 = pu8Buffer[1];
        byte2 = pu8Buffer[2];
        byte3 = pu8Buffer[3];
        byte4 = pu8Buffer[4];
        byte5 = pu8Buffer[5];
        byte6 = pu8Buffer[6];
        MBtimeout = (byte5 << 8) | byte6 ;
        err = Update_DataFlash_ModbusConfig_From_PC(byte1, byte3, byte2, byte4, MBtimeout);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            SendBack(0);
        }
        CLK_SysTickDelay(1000);
        /* Reset CPU */
        SYS_ResetCPU();
        break ;

    case CMD_TEST_MODBUS :
        switch (ClientDataFlash[1].Modbus_Register)
        {
        case 0x00 :
            mode = 0 ;
            break ;       // modebus slave
        case 0x01 :
            mode = 0 ;
            break ;
        case 0x02 :
            mode = 0 ;
            break ;
        case 0x03 :
            mode = 0 ;
            break ;

        case 0x10 :
            mode = 1 ;
            break ;        // modbus master
        case 0x11 :
            mode = 1 ;
            break ;
        case 0x12 :
            mode = 1 ;
            break ;
        case 0x13 :
            mode = 1 ;
            break ;
        }
        if (pu8Buffer[1] == 0x01)
        {
            if (mode == 0)
            {
                device[1].Modbus_test = 0x01 ;
                UART_Close(UART1);
                CLK_SysTickDelay(100000);
                eMBErrorCode eStatus = MB_ENOERR;
                UseModbus();
            }
            if (mode == 1)
            {
                device[1].Modbus_test = 0x02 ;
            }
        }
        if (pu8Buffer[1] == 0x00)
        {
            device[1].Modbus_test = 0x00 ;
        }
        break ;
    }
    return u8Errno;
}

/* read , write cmd*/
void HID_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    ProcessCommand((uint8_t *)&pu8EpBuf[0], u32Size);
}

/*read cmd*/
void HID_SetInReport(void)
{

}