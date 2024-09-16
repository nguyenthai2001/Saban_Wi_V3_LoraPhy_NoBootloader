/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_STACK_H__
#define __USB_STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#define CMD_NONE					0x00
#define CMD_READ_ADD				0x01
#define CMD_WRITE_ADD				0x02
#define CMD_WRITE_RF_CONFIG		      0x03
#define CMD_WRITE_INPUT_TYPE			0x04
#define CMD_WRITE_MODBSU_CONFIG	      0x05
#define CMD_TEST_RF				0x06
#define CMD_TEST_MODBUS	                  0x07
#define CMD_DELETE_RUNNING_INFO		0x08
#define CMD_READ_SYS_STATUS			0x09	

#define DATA_FLASH_BASE	 0x1F000
#define DATA_FLASH_END	 0x1FFFF

#define INFO_FLASH_START 0x1F200

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen);
void SendBackTestModbus (uint8_t address , uint16_t *result);
void HID_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size);
void HID_SetInReport(void) ;

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__USB_STACK_H__ */