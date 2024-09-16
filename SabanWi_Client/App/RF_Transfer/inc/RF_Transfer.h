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

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__RF_TRANSFER_H__ */