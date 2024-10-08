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
#define Master 0 
#define Client 1 

#define CMD_IO_STANDAND  0
#define CMD_RS485  1 
#define CMD_UART_SPI 2
#define CMD_I2C 3

#define MCCODE_REQUEST_FEEDBACK 0
#define MCCODE_REQUEST_NO_FEEDBACK 1
#define MCCODE_SLAVE_FEEDBACK 3

#define MASTER_GET_I2C_DATA 0x00
#define MASTER_GET_HMI_STATUS 0x01
#define MASTER_FEEDBACK_LOGIN_STATUS 0x02

#define HMI_LOGIN 0x01 
#define HMI_LOGOUT 0x02
#define HMI_BUSY  0x03

typedef struct
{
    uint8_t hmi_state ;
	  uint8_t hmi_cmd ;
	  uint8_t hmi_data[58];    
} HMI_DATA_60BYTE;
extern HMI_DATA_60BYTE hmi_data_60byte ;

typedef enum 
{
		NON,
		GET_STATE,
		SET_STATUS_LOGIN,
		SET_STATUS_REQUEST,
} sendToHmicmd;

/* USER CODE END Private defines */

uint8_t Creat_Packet_Request_CRC(unsigned char MasterID , unsigned char Commnad , unsigned char MCCode , 
                          unsigned char SlaveID ,unsigned char u16cmd, unsigned char Data , unsigned char * packet );
uint8_t Decode_Packet_Receive_CRC ( uint8_t mode , unsigned char * packet_src );

uint8_t Creat_Packet_Request_AESCRC(unsigned char MasterID , unsigned char Commnad , unsigned char MCCode , 
                          unsigned char SlaveID ,unsigned char u16cmd, unsigned char Data , unsigned char * packet );
uint8_t Decode_Packet_Receive_AESCRC ( unsigned char mode  ,unsigned char * packet_src );

uint8_t Creat_Packet_Request_SHA(unsigned char MasterID , unsigned char Commnad , unsigned char MCCode , 
                          unsigned char SlaveID ,unsigned char u16cmd, unsigned char Data , unsigned char * packet );
uint8_t Decode_Packet_Receive_SHA (unsigned char mode ,unsigned char * packet_src );

uint8_t Creat_Packet_Request_AESSHA(unsigned char MasterID , unsigned char Commnad , unsigned char MCCode , 
                          unsigned char SlaveID ,unsigned char u16cmd, unsigned char Data , unsigned char * packet );
uint8_t Decode_Packet_Receive_AESSHA (unsigned char mode , unsigned char * packet_src );                                

void Rf_Send_Request_CRC (uint8_t deviceId , uint8_t u8cmd , uint8_t u8mccode);
void Rf_Send_Feedback_CRC (void);
void Saban_Mode_IO_Standand(uint8_t ClientID,int number_input_port , int number_output_port, uint8_t port_input_number , uint8_t port_output_number , uint8_t security);
void Saban_Mode_RS485 (uint8_t ClientID , uint8_t ModbusMaterID , uint8_t Data , uint8_t security );
void Saban_Feedback_Mode_IOStandand(uint8_t security);
void Saban_Feedback_Mode_RS485(uint8_t rs485address , uint8_t rs485data , uint8_t security );

uint8_t Decode_Package_Master_Send_HMIStatus( unsigned char * packet_src );
void Rf_Send_Feedback_HMIStatus( uint8_t Cmd_HMI, uint8_t HMIdata[]);
uint8_t Decode_Package_Master_Send_LOGINStatus(unsigned char * packet_src);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__RF_TRANSFER_H__ */