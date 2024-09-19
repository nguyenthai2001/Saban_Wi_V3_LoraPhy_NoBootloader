#include "usb_stack.h"
#include "hid_transfer.h"
#include "DataFlash_stack.h"
#include "SX1278.h"
#include "SX1278Fsk_Misc.h"
#include "SX1278LoRa_Misc.h"
#include "SX1278_LoRa.h"
#include "crc_modbus.h"
#include "RF_Transfer.h"

RF_HMI_Package_Send hmi_pkg ;
uint16_t table[256];
#define POLYNOMIAL 0xA001

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

    unsigned char Mode = 0 ;
    uint8_t TxData[64] = {0};
    unsigned long int CheckFreq = 0 ;
    char CheckPower = 0 ;
    unsigned char CheckBw = 0;
    unsigned char CheckSF = 0 ;
    unsigned char CheckCodingrate = 0 ;
    unsigned long int Rxtimeout = 0 ;
    unsigned long int CheckBitrate = 0 ;
    uint8_t byte1 = 0 ;
    uint8_t byte2 = 0 ;
    uint8_t byte3 = 0 ;
    uint8_t byte4 = 0 ;

    uint8_t byte5 = 0 ;
    uint8_t byte6 = 0 ;
    uint8_t byte7 = 0 ;
    uint8_t byte8 = 0 ;

    Mode = SX1276GetMode();
    if (Mode == 1)
    {
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

        if (CheckFreq == 0x19CF0E40)        // 915
        {
            TxData[0] = 0x12 ;
        }
        if (CheckFreq == 0x3689CAC0)        // 433
        {
            TxData[0] = 0x11 ;
        }
        TxData[1] = CheckPower ;
        TxData[2] = CheckBw ;
        TxData[3] = CheckSF ;
        TxData[4] = CheckCodingrate ;
        TxData[5] = byte1 ;
        TxData[6] = byte2 ;
        TxData[7] = byte3 ;
        TxData[8] = byte4 ;
        TxData[10] = MasterDataFlash[1].Masterid ;
        TxData[11] = MasterDataFlash[1].DeviceNumber;
        TxData[12] = Mode ;
        TxData[13] = MasterDataFlash[1].Security ;
    }

    if (Mode == 0)
    {
        CheckFreq = SX1276FskGetRFFrequency();
        CheckPower = SX1276FskGetRFPower();
        CheckBitrate = SX1276FskGetBitrate();
        Rxtimeout = SX1276FskGetTimeOnAir();

        byte5 = ((CheckBitrate >> 24) & 0xFF); // Byte cao nh?t
        byte6 = ((CheckBitrate >> 16) & 0xFF);
        byte7 = ((CheckBitrate >> 8) & 0xFF);
        byte8 = (CheckBitrate & 0xFF); // Byte th?p nh?t

        byte1 = ((Rxtimeout >> 24) & 0xFF); // Byte cao nh?t
        byte2 = ((Rxtimeout >> 16) & 0xFF);
        byte3 = ((Rxtimeout >> 8) & 0xFF);
        byte4 = (Rxtimeout & 0xFF); // Byte th?p nh?t

        if (CheckFreq == 0x19CF0E40)
        {
            TxData[0] = 0x01 ;
        }
        if (CheckFreq == 0x3689CAC0)
        {
            TxData[0] = 0x02 ;
        }
        TxData[1] = CheckPower ;
        TxData[2] = byte5 ;
        TxData[3] = byte6 ;
        TxData[4] = byte7 ;
        TxData[5] = byte8 ;
        TxData[6] = byte1 ;
        TxData[7] = byte2 ;
        TxData[8] = byte3 ;
        TxData[9] = byte4 ;
        TxData[10] = MasterDataFlash[1].Masterid ;
        TxData[11] = MasterDataFlash[1].DeviceNumber;
        TxData[12] = Mode ;
        TxData[13] = MasterDataFlash[1].Security ;
    }

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

    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

void SendBackClientSettingInfo(void)
{
    uint8_t *ptr;
    uint8_t TxData[64] = {0};
    int i = 0 ;

    if (MasterDataFlash[1].DeviceNumber < 64)
    {
        for (i = 0 ; i < MasterDataFlash[1].DeviceNumber ; i++)
        {
            TxData[i * 2] = DeviceDataFlash[i + 1].ClientID ;
            TxData[i * 2 + 1] = DeviceDataFlash[i + 1].Systemcode ;
        }
    }
    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

/**
*    @brief         SendBackDevicesStatus
*
*    @param[in]
*    @param[in]
*    @param[in]
*    @return        None
*    @details       Gui thong tin trang thai hoat dong cua cac thiet bi len PC
*/
void SendBackDevicesStatus(uint8_t length)
{
    uint8_t *ptr;
    uint8_t i, index = 0;
    uint16_t crcValue = 0;

    uint8_t TxData[64] = {0};

    TxData[index++] = 0x0A; // B?t d?u c?a thông di?p
    TxData[index++] = length; // Ð? dài c?a d? li?u

    crcValue = 0x0A + length; // Kh?i t?o CRC v?i b?t d?u c?a thông di?p và d? dài

    for (i = 0; i < length; i++)
    {
        TxData[index++] = pkg_client_recv[i].slaveid;

        // Split uint16_t status into two bytes and store in little-endian format
        TxData[index++] = pkg_client_recv[i].data_h ; // Least significant byte
        TxData[index++] = pkg_client_recv[i].data_l ;  // Most significant byte
        // C?p nh?t CRC v?i m?i byte d? li?u c?a thi?t b?
        crcValue += pkg_client_recv[i].slaveid;
        crcValue += pkg_client_recv[i].data_h ; // Least significant byte
        crcValue += pkg_client_recv[i].data_l ;  // Most significant byte
    }

    // Luu tr? CRC ? d?nh d?ng big-endian
    TxData[index++] = crcValue >> 8; // Byte quan tr?ng nh?t
    TxData[index++] = crcValue & 0xFF; // Byte ít quan tr?ng nh?t
    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

void SendBackTestModbus(uint8_t * result)
{
    uint8_t *ptr;
    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, result, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

// Hàm kh?i t?o b?ng CRC-16
void init_crc16_table() {
    uint16_t remainder;
    int i = 0 ;
    int bit = 0 ;
    for ( i = 0; i < 256; i++) {
        remainder = i;
        for ( bit = 0; bit < 8; bit++) {
            if (remainder & 1) {
                remainder = (remainder >> 1) ^ POLYNOMIAL;
            } else {
                remainder = (remainder >> 1);
            }
        }
        table[i] = remainder;
    }
}

// Hàm tính toán CRC-16
uint16_t compute_checksum(uint8_t *bytes, int length) {
    uint16_t crc = 0;
    int i = 0 ;
    for ( i = 0; i < length; i++) {
        uint8_t index = (uint8_t)(crc ^ bytes[i]);
        crc = (crc >> 8) ^ table[index];
    }
    return crc;
}


void SendHMIDataFromMasterToPC(RF_HMI_Package_Send *pkg, uint8_t u8cmd, uint8_t u8addrHMI, uint8_t u8HMIData[])
{
    uint8_t *ptr;
    uint16_t u16crc ;
    uint8_t crc = 0 ;
    uint8_t crc_high = 0;
    uint8_t crc_low = 0 ;

    int i = 0 ;
    uint8_t TxData[64] = {0};

    pkg->cmd = u8cmd ;
    pkg->addrHMI = u8addrHMI ;
    for (i = 0; i < 60 ; i++)
    {
        pkg->HMIData[i] = u8HMIData[i] ;
    }
    u16crc = crc16((char*)pkg, 61);
    pkg->crc = u16crc ;
    TxData[0] = pkg->cmd ;
    TxData[1] = pkg->addrHMI ;
    for (i = 0; i < 60 ; i++)
    {
        TxData[i + 2] = pkg->HMIData[i] ;
    }
    crc_high = (pkg->crc >> 8) & 0xFF;  // Byte cao
    crc_low = pkg->crc & 0xFF;          // Byte th?p
    TxData[62] = crc_high ;
    TxData[63] = crc_low ;
    ptr = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));
    USBD_MemCopy(ptr, TxData, EP2_MAX_PKT_SIZE);
    USBD_SET_PAYLOAD_LEN(EP2, EP2_MAX_PKT_SIZE);
}

// Hàm luu chu?i user và pass vào m?ng byte
void save_user_pass_to_byte_array(const char *user, const char *pass, uint8_t *byte_array) 
{
    // Sao chép chu?i user vào m?ng byte
    while (*user) {
        *byte_array++ = (uint8_t)*user++;
    }

    // Thêm ký t? phân cách (có th? là b?t k? ký t? nào, ví d? ':' ho?c kho?ng tr?ng)
    *byte_array++ = ':';

    // Sao chép chu?i pass vào m?ng byte
    while (*pass) {
        *byte_array++ = (uint8_t)*pass++;
    }

    // Thêm ký t? null '\0' d? k?t thúc chu?i
    *byte_array = '\0';
}

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen)
{
    WDT_Close();
    WDT_DisableInt();
    uint8_t u8Cmd;
    uint8_t u8Errno = 0;
    uint8_t err = 0 ;
    uint8_t combinedByte = 0 ;

    uint8_t mode = 0 ;
    uint8_t freq = 0 ;

    u8Cmd = pu8Buffer[0];
    uint32_t bitrate = 0 ;

    uint8_t byte1 ;
    uint8_t byte2 ;
    uint8_t byte3 ;
    uint8_t byte4 ;
    uint8_t byte5 ;
    uint8_t byte6 ;
    uint8_t byte7 ;

    int u8addrHMI = 0 ;

    uint8_t u8HMIData[64] = {0};
    uint8_t CheckCRCFfomPC[61] = {0};
    uint16_t u16crcOld = 0 ;
    uint16_t u16crcNew = 0 ;
    
		const char *user = "Nguyen quy thai"; 
		const char *pass = "123456"; 
		int length = strlen(user);  

    switch (u8Cmd)
    {
    case CMD_READ_ADD:
        device[1].Modbus_test = 0x00 ;
        SendBackDeviceSettingInfo();
        break;

    case CMD_WRITE_ADD :
        err = Update_DataFlash_ID_From_PC(pu8Buffer[1], pu8Buffer[2]);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            SendBack(0);
        }
        CLK_SysTickDelay(1000);
        SYS_ResetChip();
        break ;

    case CMD_WRITE_CLIENT_SETTING :
        combinedByte = ((pu8Buffer[3] << 4) | (pu8Buffer[4] & 0x0F));
        //printf(" \n Sysstemcode : %d\n" , combinedByte);
        err = Update_DataFlashDevice_From_PC(pu8Buffer[1], pu8Buffer[2], combinedByte, pu8Buffer[5], pu8Buffer[6]);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            SendBack(0);
        }
        CLK_SysTickDelay(1000);
        SYS_ResetChip();
        break ;

    case CMD_READ_DEVICE_INFO_1 :
        SendBackClientSettingInfo();
        //printf(" \n read device info 1 : \n" );
        break ;

    case CMD_WRITE_RF_CONFIG :
        // Tách thành 4 bit cao và 4 bit th?p
        mode = (pu8Buffer[1] >> 4) & 0x0F; // 4 bit cao
        freq = pu8Buffer[1] & 0x0F;         // 4 bit th?p
        if (mode == 1)
        {
            err = Update_DataFlash_RFConfig_From_PC(pu8Buffer[1], pu8Buffer[2], pu8Buffer[3], pu8Buffer[4], pu8Buffer[5], 0);
            if (err != 0)
            {
                SendBack(1);
            }
            else
            {
                switch (MasterDataFlash[1].RfFrequence)
                {
                case 0x11 :
                    SX1276LoRaSetRFFrequency(915000000);
                    break ;
                case 0x12 :
                    SX1276LoRaSetRFFrequency(433000000);
                    break ;
                }

                SX1276LoRaSetRFPower(MasterDataFlash[1].RFPower);
                SX1276LoRaSetSignalBandwidth(MasterDataFlash[1].RFBandwidth);
                SX1276LoRaSetSpreadingFactor(MasterDataFlash[1].RFSpreadingFactor);
                SX1276LoRaSetErrorCoding(MasterDataFlash[1].ErrCode);
                SendBack(0);
                CLK_SysTickDelay(1000);
                SYS_ResetChip();
            }
        }
        if (mode == 0)
        {
            byte1 = pu8Buffer[1] ;
            byte2 = pu8Buffer[2] ;
            byte3 = pu8Buffer[3] ;
            byte4 = pu8Buffer[4] ;
            byte5 = pu8Buffer[5] ;
            byte6 = pu8Buffer[6] ;
            byte7 = pu8Buffer[7] ;
            err = Update_DataFlash_RFConfig_From_PC(pu8Buffer[1], pu8Buffer[2], pu8Buffer[4], pu8Buffer[5], pu8Buffer[6], 0);
            if (err != 0)
            {
                SendBack(1);
            }
            else
            {
                switch (MasterDataFlash[1].RfFrequence)
                {
                case 0x01 :
                    SX1276FskSetRFFrequency(915000000);
                    break ;
                case 0x02 :
                    SX1276FskSetRFFrequency(433000000);
                    break ;
                }
                bitrate = 0 << 24 | (uint32_t)MasterDataFlash[1].RFBandwidth << 16 | (uint32_t)MasterDataFlash[1].RFSpreadingFactor << 8 | (uint32_t)MasterDataFlash[1].ErrCode;
                SX1276FskSetBitrate(bitrate);
                SendBack(0);
                CLK_SysTickDelay(1000);
                SYS_ResetChip();
            }
        }

        break ;

    case CMD_READ_SYS_STATUS:                              //0x09
        SendBackDevicesStatus(20);
        break;
    case CMD_WRITE_MODBUS_CONFIG :
        err = Update_DataFlash_ModbusConfig_From_PC(pu8Buffer[2], pu8Buffer[3], pu8Buffer[1], pu8Buffer[4]);
        if (err != 0)
        {
            SendBack(1);
        }
        else
        {
            SendBack(0);
            CLK_SysTickDelay(1000);
            SYS_ResetChip();
        }
        break ;
    case CMD_TEST_MODBUS :
        if (pu8Buffer[1] == 0x01)
        {
            device[1].Modbus_test = 0x01 ;
            SendBack(1);
        }
        if (pu8Buffer[1] == 0x02)
        {
            device[1].Modbus_test = 0x02 ;
            SendBack(1);
        }
        if (pu8Buffer[1] == 0x00)
        {
            device[1].Modbus_test = 0x00 ;
            SendBack(1);
        }
        break ;
    case CMD_GET_HMI_STATUS :
			   init_crc16_table();
        memcpy(CheckCRCFfomPC, pu8Buffer, 61);
        u8addrHMI = pu8Buffer[1] ;

        u16crcOld = pu8Buffer[62] << 8 | pu8Buffer[63] ;
//        u16crcNew = crc16((char *)CheckCRCFfomPC, sizeof(CheckCRCFfomPC));
		     u16crcNew = compute_checksum(CheckCRCFfomPC, 61);
        if (u16crcNew == u16crcOld)
        {
					  save_user_pass_to_byte_array(user,pass,u8HMIData);
            SendHMIDataFromMasterToPC(&hmi_pkg, CMD_GET_HMI_STATUS, DeviceDataFlash[u8addrHMI].ClientID, u8HMIData);
        }
        else
        {
            SendBack(1);
        }
        //SendHMIDataFromMasterToPC(&hmi_pkg,CMD_GET_HMI_STATUS,DeviceDataFlash[u8addrHMI].ClientID,u8HMIData);
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