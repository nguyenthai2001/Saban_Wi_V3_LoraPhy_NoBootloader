/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RF_TRANSFER_H__
#define __RF_TRANSFER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

typedef struct
{
    uint8_t HMI_User_send[30];            //PC
    uint8_t HMI_Pass_send[30];            //PC
    uint8_t HMI_User_recv[30];            // Client
    uint8_t HMI_Pass_recv[30];            // Client
    uint8_t HMI_Status_rcv ;
} data_user_pass;
extern data_user_pass hmi_user_pass ;

typedef enum
{
    NOERR, 	
    ERR_MASTERID,
    ERR_CMD,
    ERR_MCCODE,
    ERR_CRC	,
	  ERR_CREAT_SYSTERM_CODE,
} DecodeErrorCode;

#define Master 0
#define Client 1

#define CMD_IO_STANDAND  0
#define CMD_RS485  1
#define CMD_UART_SPI 2
#define CMD_I2C 3
#define CMD_HMI_GET_STATUS 4

#define MCCODE_REQUEST_FEEDBACK 0
#define MCCODE_REQUEST_NO_FEEDBACK 1
#define MCCODE_SLAVE_FEEDBACK 3

#define MASTER_GET_I2C_DATA 0x00
#define MASTER_GET_HMI_STATUS 0x01
#define MASTER_FEEDBACK_LOGIN_STATUS 0x02

#define MASTER_GRT_HMI_STATUS_OK
#define MASTER_GRT_HMI_STATUS_OK
#define MASTER_GRT_HMI_STATUS_OK
#define MASTER_GRT_HMI_STATUS_OK

/* USER CODE END Private defines */

DecodeErrorCode Creat_Packet_Request_CRC(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                 unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet);
DecodeErrorCode Decode_Packet_Receive_CRC(Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg,unsigned char *packet_src);

DecodeErrorCode Creat_Packet_Request_AESCRC(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                    unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet);
DecodeErrorCode Decode_Packet_Receive_AESCRC(Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg ,unsigned char *packet_src);

DecodeErrorCode Creat_Packet_Request_SHA(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                 unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet);
DecodeErrorCode Decode_Packet_Receive_SHA(Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg,unsigned char *packet_src);

DecodeErrorCode Creat_Packet_Request_AESSHA(unsigned char MasterID, unsigned char Commnad, unsigned char MCCode,
                                    unsigned char SlaveID, unsigned char u16cmd, unsigned char Data, unsigned char *packet);
DecodeErrorCode Decode_Packet_Receive_AESSHA(Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg,unsigned char *packet_src);

void Rf_Send_Request_CRC(Packet_Rreceive_Data recv_pkg[],Saban_Master_Dataflash *master_pkg,uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode);
void Saban_Mode_IO_Standand(Packet_Rreceive_Data recv_pkg[],Saban_Master_Dataflash *master_pkg,uint8_t ClientID, uint8_t port_input_number, uint8_t port_output_number, uint8_t security);
void Saban_Mode_RS485(Packet_Rreceive_Data recv_pkg[],Saban_Master_Dataflash *master_pkg,uint8_t ClientID, uint8_t ModbusMaterID, uint8_t Data, uint8_t security);
void Saban_Mode_I2C(Packet_Rreceive_Data recv_pkg[],Saban_Master_Dataflash *master_pkg,uint8_t ClientID, uint8_t I2CID, uint8_t Data, uint8_t security);
void Saban_Feedback_Mode_IOStandand(uint8_t security);
void Saban_Feedback_Mode_RS485(uint8_t rs485address, uint8_t rs485data, uint8_t security );

DecodeErrorCode Decode_Packet_Client_Feddback_HMIStatus(Saban_Master_Dataflash *master_pkg, unsigned char Cmd_HMI, unsigned char * packet_src);
void RF_Send_Set_Parameter_HMIstatus(Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg, uint8_t deviceId,uint8_t u8cmd, uint8_t u8mccode, uint8_t Cmd_HMI, uint8_t HMIdata[]);
void Rf_Send_Request_HMIStatus(Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg,uint8_t deviceId, uint8_t u8cmd, uint8_t u8mccode, uint8_t Cmd_HMI, uint8_t HMIdata[]);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__RF_TRANSFER_H__ */