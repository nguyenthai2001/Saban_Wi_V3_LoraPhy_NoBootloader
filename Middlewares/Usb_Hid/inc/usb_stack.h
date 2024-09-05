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

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen);
void HID_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size);
void HID_SetInReport(void) ;

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__USB_STACK_H__ */