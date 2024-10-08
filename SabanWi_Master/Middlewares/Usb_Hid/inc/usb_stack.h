/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_STACK_H__
#define __USB_STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>  // C?n d? d�ng malloc v� free
/* USER CODE BEGIN Includes */

extern uint8_t g_hmi_data_recv_pc[60];       // Array address slave hmi 

typedef struct {
    uint8_t cmd;               // 1 byte for command
    uint8_t addrHMI;           // 1 byte for total device 
    uint8_t HMIData[60];       // Array address slave hmi 
    uint16_t crc;              // 2 bytes for CRC
} RF_HMI_Package_Send;
extern RF_HMI_Package_Send hmi_pkg ;  

typedef enum {
    HMI_NON,        
    HMI_LOGIN,      
    HMI_REQUEST,    
    HMI_GET_STATUS
} hmiResponseCmd;

typedef enum 
{ 
    START = 0,
    LOGOUT = 1,
    LOGED = 2,
    CHECKSTATUS = 3, 
} hmiState ;

typedef enum 
{
		NON,
		GET_STATE,
		SET_STATUS_LOGIN,
		SET_STATUS_REQUEST,
} sendToHmicmd;
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#define CMD_NONE					        0x00
#define CMD_READ_ADD				      0x01
#define CMD_WRITE_ADD				      0x02
#define CMD_WRITE_RF_CONFIG		    0x03
#define CMD_WRITE_CLIENT_SETTING	0x04
#define CMD_READ_DEVICE_INFO_1		0x05
#define CMD_READ_DEVICE_INFO_2		0x06
#define CMD_READ_DEVICE_INFO_3		0x07
#define CMD_READ_DEVICE_INFO_4		0x08
#define CMD_READ_SYS_STATUS			  0x09	
#define CMD_WRITE_MODBUS_CONFIG   0x0A
#define CMD_TEST_MODBUS           0x0B
#define CMD_GET_HMI_STATUS        0x0C 
#define CMD_GET_HMI_LOGIN_STATUS  0x0D

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen);
void SendBackTestModbus (uint8_t * result );
void HID_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size);
void HID_SetInReport(void) ;
void SendHMIDataFromMasterToPC(RF_HMI_Package_Send *pkg, uint8_t u8cmd, uint8_t u8addrHMI, uint8_t *u8HMIData);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__USB_STACK_H__ */