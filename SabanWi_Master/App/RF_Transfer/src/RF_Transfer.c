
#include "RF_Transfer.h"
#include "DataFlash_stack.h"
#include "timer_driver.h"
#include "aes.h"
#include "sha.h"
#include "crc_modbus.h"
#include "SX1278Fsk_Misc.h"
#include "SX1278LoRa_Misc.h"
#include "SX1278_LoRa.h"
#include "radio.h"

#if SABAN_CLIENT
#include "gpio_control.h"
#else
#endif

uint8_t DATA_H ;
uint8_t DATA_L  ;

uint8_t SLAVE_ID ;

uint8_t Mode1  = 0 ;

Packet_Rreceive_Data pkg_client_recv[255] ;
data_user_pass hmi_user_pass ;

void Save_Receive_Data_RF_Transfer(Packet_Rreceive_Data pkg[],  unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                   unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned short int receive_crc)
{
    pkg[SlaveID].masterid = MasterID ;
    pkg[SlaveID].cmd = Commnad ;
    pkg[SlaveID].mccode = MCCode ;
    pkg[SlaveID].slaveid = SlaveID ;
    pkg[SlaveID].data_h = u16cmd ;
    pkg[SlaveID].data_l = Data ;
    pkg[SlaveID].crc_result = receive_crc ;

    log_message(" SlaveID : %2X ", pkg[SlaveID].slaveid);
    log_message(" Command : %2X ", pkg[SlaveID].cmd);
    log_message(" MCcode : %2X ", pkg[SlaveID].mccode);
    log_message(" Data : %2X ", pkg[SlaveID].data_h);
    log_message("%2X ", pkg[SlaveID].data_l);
}

/*-----------------------------------------------------------------------------------------------------------------*/
/* Su dung CRC xac thuc , khong co ma hoa  */
/*-----------------------------------------------------------------------------------------------------------------*/
uint8_t Creat_Packet_Request_CRC(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                 unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet)
{
    int TimeStart  = 0 ;
    int TimeStop = 0 ;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t temp_packet[4] = {0} ;
    uint16_t crc = 0 ;
    uint8_t crc_h = 0;
    uint8_t crc_l = 0 ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();

    // kiem tra gia tri ma he thong
    if (MasterID >= 16 || Commnad >= 4 || MCCode >= 4)
    {
        log_message(" SYSTERM CODE ERR !!! \n");
        err = 1 ;
    }
    else
    {
        log_message(" SYSTERM CODE OK !!! ");
        // creat 1 byte ma he thong
        system_code = (MasterID << 4) | (Commnad << 2) | MCCode ;
        // Creat packet data
        temp_packet[0] = system_code;
        temp_packet[1] = SlaveID ;
        temp_packet[2] = u16cmd;
        temp_packet[3] = Data;
        // calculate CRC16
        crc = crc16((char*)temp_packet, sizeof(temp_packet));
        // creat crc_h crc_l
        crc_h = (crc >> 8) & 0xFF;
        crc_l = crc & 0xFF;
        // creat packet send
        packet [0] = system_code ;
        packet [1] = SlaveID ;
        packet [2] = u16cmd ;
        packet [3] = Data ;
        packet [4] = crc_h ;
        packet [5] = crc_l ;

        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();

        log_message(" PACKET CREAT DONE !!! [%d] us", TimeStop - TimeStart);
        err = 0;
    }
    return err ;
}

uint8_t Decode_Packet_Receive_CRC(uint8_t mode, unsigned char * packet_src)
{
    int TimeStart = 0 ;
    int TimeStop = 0 ;

    uint8_t masteridcurrent;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t receive_crc_h = 0 ;
    uint8_t receive_crc_l = 0 ;
    uint16_t receive_crc = 0 ;
    uint8_t temp_packet[4];
    uint16_t calculated_crc = 0 ;

    unsigned char MasterID ;
    unsigned char Commnad ;
    unsigned char MCCode ;
    unsigned char SlaveID ;
    unsigned char u16cmd;
    unsigned char Data ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();

    system_code = packet_src [0];
    SlaveID = packet_src[1];
    u16cmd = packet_src[2];
    Data = packet_src[3];
    receive_crc_h = packet_src[4];
    receive_crc_l = packet_src[5];
    receive_crc = (receive_crc_h << 8) | receive_crc_l ;

    // masterid command mccode
    MasterID = (system_code >> 4) & 0x0F ;
    Commnad = (system_code >> 2) & 0x03 ;
    MCCode = system_code & 0x03 ;

    // creat packet crc
    temp_packet[0] = system_code;
    temp_packet[1] = SlaveID;
    temp_packet[2] = u16cmd ;
    temp_packet[3] = Data ;

    // calaculated crc
    calculated_crc = crc16((char*)temp_packet, sizeof(temp_packet));

    // check crc receive and crc calculated
    if (calculated_crc != receive_crc)
    {
        err = 0;
        log_message(" DECODE DATA ERR !!! ");
    }
    else
    {
        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
//            printf(" DECODE DATA SLAVE [%2X] OK !!! [%d] us " , SlaveID , TimeStop - TimeStart);
#if SABAN_CLIENT
        masteridcurrent = (ClientDataFlash[1].System_code >> 4) & 0x0F ;
        SLAVE_ID = ClientDataFlash[1].SlaveID;
        if (MCCode == MCCODE_REQUEST_FEEDBACK  && SLAVE_ID == SlaveID && masteridcurrent == MasterID)              // decode for slave
        {
            Save_Receive_Data_RF_Transfer(pkg_client_recv, masteridcurrent, Commnad, MCCode, SLAVE_ID, u16cmd, Data, receive_crc);
            Update_DataFlash_From_Master();
            err = 1 ;
        }
#else
        if (MCCode == MCCODE_SLAVE_FEEDBACK && MasterDataFlash[1].Masterid == MasterID)                // decode for master
        {
            Save_Receive_Data_RF_Transfer(pkg_client_recv, MasterID, Commnad, MCCode, SlaveID, u16cmd, Data, receive_crc);
            err = 1 ;
        }
#endif
    }
    return err ;
}

void Rf_Send_Request_CRC(uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode)
{
    Mode1 = SX1276GetMode();
    unsigned char TxBuf[6] = {0};
    uint8_t test_cout = 0;
    uint32_t TimeOnAir = 0;

    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(8);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 40);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(8);
    }

    pkg_client_recv[deviceId].masterid = MasterDataFlash[1].Masterid ;
    pkg_client_recv[deviceId].cmd = u8cmd ;
    pkg_client_recv[deviceId].mccode = u8mccode ;
    pkg_client_recv[deviceId].slaveid = deviceId ;
    Creat_Packet_Request_CRC(pkg_client_recv[deviceId].masterid, pkg_client_recv[deviceId].cmd, pkg_client_recv[deviceId].mccode,
                             pkg_client_recv[deviceId].slaveid, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));
}

void Rf_Send_Feedback_CRC(void)
{
#if  SABAN_CLIENT
    unsigned char TxBuf[6] = {0};
    uint8_t test_cout = 0;
    uint32_t TimeOnAir = 0;

    Mode1 = SX1276GetMode();
    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(8);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 40);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(8);
    }

    SLAVE_ID = ClientDataFlash[1].SlaveID;

    pkg_client_recv[SLAVE_ID].mccode = MCCODE_SLAVE_FEEDBACK ;

    Creat_Packet_Request_CRC(pkg_client_recv[SLAVE_ID].masterid, pkg_client_recv[SLAVE_ID].cmd, pkg_client_recv[SLAVE_ID].mccode,
                             pkg_client_recv[SLAVE_ID].slaveid, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));
#endif
}


/*--------------------------------------------------------------------------------------------------------------------*/
/* Su dung ma hao AES va xac thuc CRC  */
/*--------------------------------------------------------------------------------------------------------------------*/
uint8_t Creat_Packet_Request_AESCRC(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                    unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet)
{
    int TimeStart = 0 ;
    int TimeStop = 0 ;
    // Kh�a AES-128 (16 byte)
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0xcf, 0x40, 0x3d, 0xae, 0x69, 0x6e
                      };
    // IV (Initialization Vector - 16 byte)
    uint8_t iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                     };
    // Du lieu sau khi padding (16 byte)
    uint8_t padded_data[16];
    memset(padded_data, 0, 16);

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t temp_packet[4] = {0} ;
    uint16_t crc = 0 ;
    uint8_t crc_h = 0;
    uint8_t crc_l = 0;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();
    // kiem tra gia tri ma he thong
    if (MasterID >= 16 || Commnad >= 4 || MCCode >= 4)
    {
        log_message(" SYSTERM CODE ERR !!! \n");
        err = 1 ;
    }
    else
    {
        log_message(" SYSTERM CODE OK !!! ");
        // creat 1 byte ma he thong
        system_code = (MasterID << 4) | (Commnad << 2) | MCCode ;
        // Creat packet data
        temp_packet[0] = system_code;
        temp_packet[1] = SlaveID ;
        temp_packet[2] = u16cmd;
        temp_packet[3] = Data;

        // padding data 16byte
        memcpy(padded_data, temp_packet, 4);
        // Ma hoa
        struct AES_ctx ctx;
        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_encrypt_buffer(&ctx, padded_data, 16);

        // calculate CRC16
        crc = crc16((char*)padded_data, sizeof(padded_data));
        // creat crc_h crc_l
        crc_h = (crc >> 8) & 0xFF;
        crc_l = crc & 0xFF;
        // creat packet send

        memcpy(packet, padded_data, 16);
        packet[16] = crc_h ;
        packet[17] = crc_l ;

        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        log_message(" PACKET CREAT DONE !!! [%d] us", TimeStop - TimeStart);
        err = 0;
    }

    return err ;
}


uint8_t Decode_Packet_Receive_AESCRC(unsigned char mode, unsigned char * packet_src)
{
    int TimeStart = 0 ;
    int TimeStop = 0 ;
    // Kh�a AES-128 (16 byte)
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0xcf, 0x40, 0x3d, 0xae, 0x69, 0x6e
                      };
    // IV (Initialization Vector - 16 byte)
    uint8_t iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                     };

    uint8_t masteridcurrent;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t receive_crc_h = 0 ;
    uint8_t receive_crc_l = 0 ;
    uint16_t receive_crc = 0 ;
    uint8_t packet[16] ;
    uint16_t calculated_crc = 0 ;

    uint8_t padded_data[16];
    memset(padded_data, 0, 16);

    unsigned char MasterID ;
    unsigned char Commnad ;
    unsigned char MCCode ;
    unsigned char SlaveID ;
    unsigned char u16cmd;
    unsigned char Data ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();

    memcpy(packet, packet_src, 16);

    receive_crc_h = packet_src[16];
    receive_crc_l = packet_src[17];
    receive_crc = (receive_crc_h << 8) | receive_crc_l ;

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, packet, 16);

    system_code = packet [0];
    SlaveID = packet[1];
    u16cmd = packet[2];
    Data = packet[3];

    // masterid command mccode
    MasterID = (system_code >> 4) & 0x0F ;
    Commnad = (system_code >> 2) & 0x03 ;
    MCCode = system_code & 0x03 ;

    memcpy(padded_data, packet, 4);
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, padded_data, 16);
    // calaculated crc
    calculated_crc = crc16((char*)padded_data, sizeof(padded_data));

    // check crc receive and crc calculated
    if (calculated_crc != receive_crc)
    {
        err = 0;
        log_message(" DECODE DATA ERR !!! ");
    }
    else
    {
        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        log_message(" DECODE DATA SLAVE [%2X] OK !!! [%d] us ", SlaveID, TimeStop - TimeStart);

#if SABAN_CLIENT
        masteridcurrent = (ClientDataFlash[1].System_code >> 4) & 0x0F ;
        SLAVE_ID = ClientDataFlash[1].SlaveID;
        if (MCCode == MCCODE_REQUEST_FEEDBACK  && SLAVE_ID == SlaveID && masteridcurrent == MasterID)             // decode for slave
        {
            Save_Receive_Data_RF_Transfer(pkg_client_recv, masteridcurrent, Commnad, MCCode, SLAVE_ID, u16cmd, Data, receive_crc);
            Update_DataFlash_From_Master();
            err = 1 ;
        }

#else
        if (MCCode == MCCODE_SLAVE_FEEDBACK && MasterDataFlash[1].Masterid == MasterID)               // decode for master
        {
            Save_Receive_Data_RF_Transfer(pkg_client_recv, MasterID, Commnad, MCCode, SlaveID, u16cmd, Data, receive_crc);
            err = 1 ;
        }
#endif
    }
    return err ;
}

void Rf_Send_Request_AESCRC(uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode)
{
#if SABAN_CLIENT
#else
    unsigned char TxBuf[18] = {0};

    uint32_t TimeOnAir = 0;

    Mode1 = SX1276GetMode();
    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(18);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 60);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(18);
    }

    pkg_client_recv[deviceId].masterid = MasterDataFlash[1].Masterid ;
    pkg_client_recv[deviceId].cmd = u8cmd ;
    pkg_client_recv[deviceId].mccode = u8mccode ;
    pkg_client_recv[deviceId].slaveid = deviceId ;
    Creat_Packet_Request_AESCRC(pkg_client_recv[deviceId].masterid, pkg_client_recv[deviceId].cmd, pkg_client_recv[deviceId].mccode,
                                pkg_client_recv[deviceId].slaveid, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));
//      for(test_cout = 0 ;test_cout < sizeof(TxBuf) ; test_cout ++)
//      {
//            printf("%02X ",TxBuf[test_cout]);
//      }
#endif
}

void Rf_Send_Feedback_AESCRC(void)
{

#if SABAN_CLIENT
    unsigned char TxBuf[18] = {0};
    uint8_t test_cout = 0;
    uint32_t TimeOnAir = 0;

    Mode1 = SX1276GetMode();
    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(18);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 60);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(18);
    }

    SLAVE_ID = ClientDataFlash[1].SlaveID;

    device[SLAVE_ID].mccode = MCCODE_SLAVE_FEEDBACK ;

    Creat_Packet_Request_AESCRC(device[SLAVE_ID].masterID, device[SLAVE_ID].cmd, device[SLAVE_ID].mccode,
                                device[SLAVE_ID].slaveID, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));

//      for(test_cout = 0 ;test_cout < sizeof(TxBuf) ; test_cout ++)
//      {
//            printf("%02X ",TxBuf[test_cout]);
//      }
#else
#endif
}

/*--------------------------------------------------------------------------------------------------------*/
/* Khong ma hoa, xac thuc SHA  */
/*--------------------------------------------------------------------------------------------------------*/
uint8_t Creat_Packet_Request_SHA(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                 unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet)
{
    int TimeStart = 0 ;
    int TimeStop = 0 ;

    Sha256Context   sha256Context;
    SHA256_HASH     sha256Hash;
    uint16_t        i;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t temp_packet[4] = {0} ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();
    // kiem tra gia tri ma he thong
    if (MasterID >= 16 || Commnad >= 4 || MCCode >= 4)
    {
        log_message(" SYSTERM CODE ERR !!! \n");
        err = 1 ;
    }
    else
    {
        log_message(" SYSTERM CODE OK !!! ");
        // creat 1 byte ma he thong
        system_code = (MasterID << 4) | (Commnad << 2) | MCCode ;
        // Creat packet data
        temp_packet[0] = system_code;
        temp_packet[1] = SlaveID ;
        temp_packet[2] = u16cmd;
        temp_packet[3] = Data;
        // calculate SHA256
        Sha256Initialise(&sha256Context);
        Sha256Update(&sha256Context, (unsigned char*)temp_packet, sizeof(temp_packet));
        Sha256Finalise(&sha256Context, &sha256Hash);
        // creat packet send
        packet [0] = system_code ;
        packet [1] = SlaveID ;
        packet [2] = u16cmd ;
        packet [3] = Data ;
        for (i = 0; i < 32; i++)
        {
            packet[i + 4] = sha256Hash.bytes[i] ;
        }

        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        log_message(" PACKET CREAT SHA DONE !!! [%d] us", TimeStop - TimeStart);
        err = 0;
    }
    return err ;
}

uint8_t Decode_Packet_Receive_SHA(unsigned char mode, unsigned char * packet_src)
{

    int TimeStart = 0 ;
    int TimeStop = 0 ;

    Sha256Context   sha256Context;
    SHA256_HASH     sha256Hash;
    uint8_t        i;

    uint8_t masteridcurrent;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t receive_sha[32]  ;
    uint8_t temp_packet[4];

    unsigned char MasterID ;
    unsigned char Commnad ;
    unsigned char MCCode ;
    unsigned char SlaveID ;
    unsigned char u16cmd;
    unsigned char Data ;

    system_code = packet_src [0];
    SlaveID = packet_src[1];
    u16cmd = packet_src[2];
    Data = packet_src[3];

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();

    for (i = 0; i < sizeof(receive_sha); i++)
    {
        receive_sha[i] = packet_src[i + 4] ;
    }

    // masterid command mccode
    MasterID = (system_code >> 4) & 0x0F ;
    Commnad = (system_code >> 2) & 0x03 ;
    MCCode = system_code & 0x03 ;

    // creat packet crc
    temp_packet[0] = system_code;
    temp_packet[1] = SlaveID;
    temp_packet[2] = u16cmd ;
    temp_packet[3] = Data ;

    // calculate SHA256
    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, (unsigned char*)temp_packet, sizeof(temp_packet));
    Sha256Finalise(&sha256Context, &sha256Hash);

    // check crc receive and crc calculated
    if (0 != memcmp((char *) receive_sha, (char *) sha256Hash.bytes, 32))
    {
        err = 0;
        log_message(" DECODE DATA SHA ERR !!! ");
    }
    else
    {
        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        log_message(" DECODE DATA SHA SLAVE [%2X] OK !!! [%d] us", SlaveID, TimeStop - TimeStart);

#if SABAN_CLIENT
        masteridcurrent = (ClientDataFlash[1].System_code >> 4) & 0x0F ;
        SLAVE_ID = ClientDataFlash[1].SlaveID;
        if (MCCode == MCCODE_REQUEST_FEEDBACK  && SLAVE_ID == SlaveID && masteridcurrent == MasterID)             // decode for slave
        {
            device[SLAVE_ID].masterID = MasterID ;
            device[SLAVE_ID].cmd = Commnad ;
            device[SLAVE_ID].mccode = MCCode ;
            device[SLAVE_ID].slaveID = SlaveID ;
            device[SLAVE_ID].data_h = u16cmd ;
            device[SLAVE_ID].data_l = Data ;
            log_message(" MasterID : %2X ", device[SLAVE_ID].masterID);
            log_message(" Command : %2X ", device[SLAVE_ID].cmd);
            log_message(" MCcode : %2X ", device[SLAVE_ID].mccode);
            log_message(" Data : %2X ", device[SLAVE_ID].data_h);
            log_message(" \n Data  : %2X ", device[SLAVE_ID].data_l);
            Update_DataFlash_From_Master();
            err = 1 ;
        }
#else
        if (MCCode == MCCODE_SLAVE_FEEDBACK && MasterDataFlash[1].Masterid == MasterID)               // decode for master
        {
            Save_Receive_Data_RF_Transfer(pkg_client_recv, MasterID, Commnad, MCCode, SlaveID, u16cmd, Data, (uint16_t)receive_sha);
            err = 1 ;
        }
#endif
    }
    return err ;
}

void Rf_Send_Request_SHA(uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode)
{
#if SABAN_CLIENT
#else
    unsigned char TxBuf[36] = {0};

    uint32_t TimeOnAir = 0;

    Mode1 = SX1276GetMode();
    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(36);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 30);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(36);
    }

    pkg_client_recv[deviceId].masterid = MasterDataFlash[1].Masterid ;
    pkg_client_recv[deviceId].cmd = u8cmd ;
    pkg_client_recv[deviceId].mccode = u8mccode ;
    pkg_client_recv[deviceId].slaveid = deviceId ;
    Creat_Packet_Request_SHA(pkg_client_recv[deviceId].masterid, pkg_client_recv[deviceId].cmd, pkg_client_recv[deviceId].mccode,
                             pkg_client_recv[deviceId].slaveid, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));
//      for(test_cout = 0 ;test_cout < sizeof(TxBuf) ; test_cout ++)
//      {
//            printf("%02X ",TxBuf[test_cout]);
//      }
#endif

}

void Rf_Send_Feedback_SHA(void)
{
#if SABAN_CLIENT
    unsigned char TxBuf[36] = {0};
    uint8_t test_cout = 0;

    uint32_t TimeOnAir = 0;
    Mode1 = SX1276GetMode();

    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(36);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 30);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(36);
    }

    SLAVE_ID = ClientDataFlash[1].SlaveID;

    device[SLAVE_ID].mccode = MCCODE_SLAVE_FEEDBACK ;

    Creat_Packet_Request_SHA(device[SLAVE_ID].masterID, device[SLAVE_ID].cmd, device[SLAVE_ID].mccode,
                             device[SLAVE_ID].slaveID, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));

//      for(test_cout = 0 ;test_cout < sizeof(TxBuf) ; test_cout ++)
//      {
//            printf("%02X ",TxBuf[test_cout]);
//      }
#else
#endif
}

/*---------------------------------------------------------------------------------------------------------------------*/
/* Su dung ma hoa AES va Xac thuc SHA  */
/*---------------------------------------------------------------------------------------------------------------------*/
uint8_t Creat_Packet_Request_AESSHA(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                    unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet)
{
    int TimeStart = 0;
    int TimeStop = 0 ;
    // Kh�a AES-128 (16 byte)
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0xcf, 0x40, 0x3d, 0xae, 0x69, 0x6e
                      };
    // IV (Initialization Vector - 16 byte)
    uint8_t iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                     };
    // Du lieu sau khi padding (16 byte)
    uint8_t padded_data[16];
    memset(padded_data, 0, 16);

    Sha256Context   sha256Context;
    SHA256_HASH     sha256Hash;
    uint8_t        i;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t temp_packet[4] = {0} ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();
    // kiem tra gia tri ma he thong
    if (MasterID >= 16 || Commnad >= 4 || MCCode >= 4)
    {
        log_message(" SYSTERM CODE ERR !!! \n");
        err = 1 ;
    }
    else
    {
        //log_message(" SYSTERM CODE OK !!! ");
        // creat 1 byte ma he thong
        system_code = (MasterID << 4) | (Commnad << 2) | MCCode ;
        // Creat packet data
        temp_packet[0] = system_code;
        temp_packet[1] = SlaveID ;
        temp_packet[2] = u16cmd;
        temp_packet[3] = Data;

        // padding data 16byte
        memcpy(padded_data, temp_packet, 4);
        // Ma hoa
        struct AES_ctx ctx;
        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_encrypt_buffer(&ctx, padded_data, 16);

        // calculate SHA256
        Sha256Initialise(&sha256Context);
        Sha256Update(&sha256Context, (unsigned char*)padded_data, sizeof(padded_data));
        Sha256Finalise(&sha256Context, &sha256Hash);
        // creat packet send

        memcpy(packet, padded_data, 16);
        for (i = 0; i < 32; i++)
        {
            packet[i + 16] = sha256Hash.bytes[i] ;
        }
        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();
        // log_message(" PACKET CREAT DONE !!! [ %d ] us", TimeStop - TimeStart);
        err = 0;
    }
    return err ;
}

uint8_t Decode_Packet_Receive_AESSHA(unsigned char mode, unsigned char * packet_src)
{
    int TimeStart  = 0 ;
    int TimeStop = 0 ;

    // Kh�a AES-128 (16 byte)
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0xcf, 0x40, 0x3d, 0xae, 0x69, 0x6e
                      };
    // IV (Initialization Vector - 16 byte)
    uint8_t iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                     };

    Sha256Context   sha256Context;
    SHA256_HASH     sha256Hash;
    uint8_t        i;

    uint8_t err = 0 ;
    uint8_t system_code = 0 ;
    uint8_t packet[16] ;
    uint8_t receive_crc[32] ;

    uint8_t padded_data[16];
    memset(padded_data, 0, 16);

    unsigned char MasterID ;
    unsigned char Commnad ;
    unsigned char MCCode ;
    unsigned char SlaveID ;
    unsigned char u16cmd;
    unsigned char Data ;

    memcpy(packet, packet_src, 16);

    log_message(" \n Receive SHA :  !!! ");
    for (i = 0; i < 32; i++)
    {
        receive_crc[i] = packet_src[i + 16] ;
    }

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, packet, 16);

    system_code = packet [0];
    SlaveID = packet[1];
    u16cmd = packet[2];
    Data = packet[3];

    // masterid command mccode
    MasterID = (system_code >> 4) & 0x0F ;
    Commnad = (system_code >> 2) & 0x03 ;
    MCCode = system_code & 0x03 ;

    memcpy(padded_data, packet, 4);
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, padded_data, 16);

    // calculate SHA256
    Sha256Initialise(&sha256Context);
    Sha256Update(&sha256Context, (unsigned char*)padded_data, sizeof(padded_data));
    Sha256Finalise(&sha256Context, &sha256Hash);

    // check crc receive and crc calculated
    if (0 != memcmp((char *) receive_crc, (char *) sha256Hash.bytes, 32))
    {
        err = 0;
        //log_message(" DECODE DATA ERR !!! ");
    }
    else
    {
        Timer3_GetTickMs();
        Timer3_ResetTickMs();
        //log_message(" DECODE DATA SLAVE [%2X] OK !!! [%d] us ", SlaveID, TimeStop - TimeStart);
#if SABAN_CLIENT
        if (MCCode == MCCODE_REQUEST_FEEDBACK  && SlaveID ==  ClientDataFlash[1].SlaveID  &&  MasterID == ((ClientDataFlash[1].System_code >> 4) & 0x0F))            // decode for slave
        {
            device[SlaveID].masterID = MasterID ;
            device[SlaveID].cmd = Commnad ;
            device[SlaveID].mccode = MCCode ;
            device[SlaveID].slaveID = SlaveID ;
            device[SlaveID].data_h = u16cmd ;
            device[SlaveID].data_l = Data ;
            log_message(" MasterID : %2X ", device[SLAVE_ID].masterID);
            log_message(" Command : %2X ", device[SLAVE_ID].cmd);
            log_message(" MCcode : %2X ", device[SLAVE_ID].mccode);
            log_message(" Data : %2X ", device[SLAVE_ID].data_h);
            log_message("%2X ", device[SLAVE_ID].data_l);
            Update_DataFlash_From_Master();
            err = 1 ;
        }
#else
        if (MCCode == MCCODE_SLAVE_FEEDBACK &&  MasterID == MasterDataFlash[1].Masterid)               // decode for master
        {
            //Save_Receive_Data_RF_Transfer(pkg_client_recv, MasterID, Commnad, MCCode, SlaveID, u16cmd, Data, (uint16_t)receive_crc);
            pkg_client_recv[SlaveID].masterid = MasterID ;
            pkg_client_recv[SlaveID].cmd = Commnad ;
            pkg_client_recv[SlaveID].mccode = MCCode ;
            pkg_client_recv[SlaveID].slaveid = SlaveID ;
            pkg_client_recv[SlaveID].data_h = u16cmd ;
            pkg_client_recv[SlaveID].data_l = Data ;
            pkg_client_recv[SlaveID].crc_result = (uint16_t) receive_crc ;

            printf(" SlaveID : %2X ", pkg_client_recv[SlaveID].slaveid);
            printf(" Command : %2X ", pkg_client_recv[SlaveID].cmd);
            printf(" MCcode : %2X ", pkg_client_recv[SlaveID].mccode);
            printf(" Data : %2X ", pkg_client_recv[SlaveID].data_h);
            printf("%2X ", pkg_client_recv[SlaveID].data_l);
            err = 1 ;
        }
#endif
    }
    return err ;
}

void Rf_Send_Request_AESSHA(uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode)
{
#if SABAN_CLIENT
#else
    unsigned char TxBuf[48] = {0};
    uint32_t TimeOnAir = 0;

    Mode1 = SX1276GetMode();
    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(48);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 60);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(48);
    }

    pkg_client_recv[deviceId].masterid = MasterDataFlash[1].Masterid ;
    pkg_client_recv[deviceId].cmd = u8cmd ;
    pkg_client_recv[deviceId].mccode = u8mccode ;
    pkg_client_recv[deviceId].slaveid = deviceId ;
    Creat_Packet_Request_AESSHA(pkg_client_recv[deviceId].masterid, pkg_client_recv[deviceId].cmd, pkg_client_recv[deviceId].mccode,
                                pkg_client_recv[deviceId].slaveid, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));
//      for(test_cout = 0 ;test_cout < sizeof(TxBuf) ; test_cout ++)
//      {
//            printf("%02X ",TxBuf[test_cout]);
//      }
#endif
}

void Rf_Send_Feedback_AESSHA(void)
{
#if SABAN_CLIENT
    unsigned char TxBuf[48] = {0};
    uint8_t test_cout = 0;
    uint32_t TimeOnAir = 0;

    Mode1 = SX1276GetMode();
    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(48);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 60);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(48);
    }

    SLAVE_ID = ClientDataFlash[1].SlaveID;

    device[SLAVE_ID].mccode = MCCODE_SLAVE_FEEDBACK ;

    Creat_Packet_Request_AESSHA(device[SLAVE_ID].masterID, device[SLAVE_ID].cmd, device[SLAVE_ID].mccode,
                                device[SLAVE_ID].slaveID, DATA_H, DATA_L, TxBuf);
    SX1276SetTxPacket(TxBuf, sizeof(TxBuf));

//      for(test_cout = 0 ;test_cout < sizeof(TxBuf) ; test_cout ++)
//      {
//            printf("%02X ",TxBuf[test_cout]);
//      }
#else
#endif
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Saban Mode1  */
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void Saban_Mode_IO_Standand(uint8_t ClientID, int number_input_port, int number_output_port, uint8_t port_input_number, uint8_t port_output_number, uint8_t security)
{
    DATA_H = 0 ;
    DATA_L = 0;

    DATA_H = port_input_number ;
    DATA_L = port_output_number ;

    switch (security)
    {
    case 0 :
        Rf_Send_Request_CRC(ClientID, CMD_IO_STANDAND, MCCODE_REQUEST_FEEDBACK);
        break ;
    case 1 :
        Rf_Send_Request_AESCRC(ClientID, CMD_IO_STANDAND, MCCODE_REQUEST_FEEDBACK);
        break ;
    case 2 :
        Rf_Send_Request_SHA(ClientID, CMD_IO_STANDAND, MCCODE_REQUEST_FEEDBACK);
        break ;
    case 3 :
        Rf_Send_Request_AESSHA(ClientID, CMD_IO_STANDAND, MCCODE_REQUEST_FEEDBACK);
        break ;
    }
}

void Saban_Mode_RS485(uint8_t ClientID, uint8_t ModbusMaterID, uint8_t Data, uint8_t security)
{
    DATA_H = 0 ;
    DATA_L = 0 ;

    DATA_H = ModbusMaterID ;
    DATA_L = Data ;

    switch (security)
    {
    case 0 :
        Rf_Send_Request_CRC(ClientID, CMD_RS485, MCCODE_REQUEST_FEEDBACK);
        break ;
    case 1 :
        Rf_Send_Request_AESCRC(ClientID, CMD_RS485, MCCODE_REQUEST_FEEDBACK);
        break ;
    case 2 :
        Rf_Send_Request_SHA(ClientID, CMD_RS485, MCCODE_REQUEST_FEEDBACK);
        break ;
    case 3 :
        Rf_Send_Request_AESSHA(ClientID, CMD_RS485, MCCODE_REQUEST_FEEDBACK);
        break ;
    }
}

void Saban_Feedback_Mode_IOStandand(uint8_t security)
{
#if SABAN_CLIENT
    DATA_H = 0;
    DATA_L = 0 ;

    uint16_t input_status = 0 ;

    input_status = Saban_Input_Control();

    DATA_H = (input_status >> 8) & 0xFF ;
    DATA_L =   input_status & 0xFF ;

    switch (security)
    {
    case 0 :
        Rf_Send_Feedback_CRC();
        break ;
    case 1 :
        Rf_Send_Feedback_AESCRC();
        break ;
    case 2 :
        Rf_Send_Feedback_SHA();
        break ;
    case 3:
        Rf_Send_Feedback_AESSHA();
        break ;
    }
#else
#endif
}

void Saban_Feedback_Mode_RS485(uint8_t rs485address, uint8_t rs485data, uint8_t security)
{
    DATA_H = 0 ;
    DATA_L = 0 ;

    DATA_H = rs485address ;
    DATA_L = rs485data ;
    switch (security)
    {
    case 0 :
        Rf_Send_Feedback_CRC();
        break ;
    case 1 :
        Rf_Send_Feedback_AESCRC();
        break ;
    case 2 :
        Rf_Send_Feedback_SHA();
        break ;
    case 3:
        Rf_Send_Feedback_AESSHA();
        break ;
    }
}

uint8_t Creat_Packet_Master_Get_HMIStatus(unsigned char MasterID, unsigned char Command, unsigned char MCCode, unsigned char SlaveID,
        unsigned char *user, unsigned char *pass, unsigned char *packet)
{
    int TimeStart  = 0 ;
    int TimeStop = 0 ;
    uint8_t err  = 0 ;
    uint8_t system_code = 0 ;
    uint8_t temp_packet[63] = {0} ;
    int copy_cnt = 0 ;
    uint16_t crc = 0 ;
    uint8_t crc_h = 0 ;
    uint8_t crc_l = 0 ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();

    // kiem tra gia tri ma he thong
    if (MasterID >= 16 || Command >= 4 || MCCode >= 4)
    {
        //log_message(" SYSTERM CODE ERR !!! \n");
        err = 1 ;
    }
    else
    {
        // log_message(" SYSTERM CODE OK !!! ");
        // creat 1 byte ma he thong
        system_code = (MasterID << 4) | (Command << 2) | MCCode ;
        // Creat packet data
        temp_packet[0] = system_code;
        temp_packet[1] = SlaveID ;
        temp_packet[2] = MASTER_GET_HMI_STATUS;
        for (copy_cnt = 0 ; copy_cnt <= 30 ; copy_cnt ++)
        {
            temp_packet[copy_cnt + 3] = user[copy_cnt] ;
        }
        for (copy_cnt = 0 ; copy_cnt <= 30 ; copy_cnt ++)
        {
            temp_packet[copy_cnt + 34] = pass[copy_cnt] ;
        }
        // calculate CRC16
//        crc = crc16((char*)temp_packet, sizeof(temp_packet));
//        // creat crc_h crc_l
//        crc_h = (crc >> 8) & 0xFF;
//        crc_l = crc & 0xFF;
//
        // creat packet send
        memcpy(packet, temp_packet, 63);
        //packet[65] = crc_h ;
        //packet[66] = crc_l ;

        TimeStop = Timer3_GetTickMs();
        Timer3_ResetTickMs();

        log_message(" PACKET CREAT DONE !!! [%d] us", TimeStop - TimeStart);
        err = 0;
    }
    return err ;
}

uint8_t Decode_Packet_Client_Feddback_HMIStatus(data_user_pass *user_pass, unsigned char * packet_src)
{
    int TimeStart = 0 ;
    int TimeStop = 0 ;

    uint8_t err = 0 ;
    int check_user_pass = 0 ;
    uint8_t system_code = 0 ;

    uint8_t MasterID ;
    uint8_t Commnad ;
    uint8_t MCCode ;
    uint8_t SlaveID ;
    uint8_t hmi_status;
    int copy_cnt = 0 ;

    Timer3_SetTickMs();
    TimeStart = Timer3_GetTickMs();

    system_code = packet_src[0];
    SlaveID = packet_src[1];
    hmi_status = packet_src[2];               // HMI_status

    // masterid command mccode
    MasterID = (system_code >> 4) & 0x0F ;
    Commnad = (system_code >> 2) & 0x03 ;
    MCCode = system_code & 0x03 ;

    TimeStop = Timer3_GetTickMs();
    Timer3_ResetTickMs();
    if (MasterDataFlash[1].Masterid == MasterID)           //  master id ok
    {
        printf(" DECODE DATA SLAVE [%2X] OK !!! [%d] us ", SlaveID, TimeStop - TimeStart);

        if (MCCode == MCCODE_SLAVE_FEEDBACK && MasterDataFlash[1].Masterid == MasterID)                // decode for master
        {
            for (copy_cnt = 0 ; copy_cnt < 31 ; copy_cnt ++)
            {
                user_pass->HMI_User_recv[copy_cnt] = packet_src[copy_cnt + 3] ;
            }
            for (copy_cnt = 0 ; copy_cnt < 31 ; copy_cnt ++)
            {
                user_pass->HMI_Pass_recv[copy_cnt] = packet_src[copy_cnt + 34] ;
            }
            check_user_pass = memcmp(user_pass->HMI_User_recv, user_pass->HMI_User_send, 30);
            {
                if (check_user_pass == 0)
                {
                    check_user_pass = memcmp(user_pass->HMI_Pass_recv, user_pass->HMI_Pass_send, 30);
                    if (check_user_pass == 0)
                    {
                        user_pass->HMI_Status_rcv = hmi_status ;
                        err = 0 ;             // check user pass ok
                    }
                    else
                    {
                        err = 3 ;             // check pass err
                    }
                }
                else
                {
                    err  = 2 ;      // check user err
                }
            }
        }
    }
    else
    {
        err = 1 ;  // master id Err
    }
    return err ;
}

void Rf_Send_Request_HMIStatus(uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode, uint8_t *user, uint8_t *pass)
{
    Mode1 = SX1276GetMode();
    unsigned char TxBuf[64] = {0};
    uint8_t test_cout = 0;
    uint32_t TimeOnAir = 0;

    if (Mode1 == LORA)
    {
        SX1276LoRaSetPayloadLength(12);
        TimeOnAir = SX1276GetTimeOnAir();
        SX1276LoRaSetRxPacketTimeout(TimeOnAir + 60);
    }
    if (Mode1 == FSK)
    {
        SX1276FskSetPayloadLength(64);
    }
    pkg_client_recv[deviceId].masterid = MasterDataFlash[1].Masterid ;
    pkg_client_recv[deviceId].cmd = u8cmd ;
    pkg_client_recv[deviceId].mccode = u8mccode ;
    pkg_client_recv[deviceId].slaveid = deviceId ;
    Creat_Packet_Master_Get_HMIStatus(pkg_client_recv[deviceId].masterid, pkg_client_recv[deviceId].cmd, pkg_client_recv[deviceId].mccode,
                                      pkg_client_recv[deviceId].slaveid, user, pass, TxBuf);
    printArray8Bit(TxBuf, sizeof(TxBuf));
    SX1276SetTxPacket(TxBuf, 12);
}
