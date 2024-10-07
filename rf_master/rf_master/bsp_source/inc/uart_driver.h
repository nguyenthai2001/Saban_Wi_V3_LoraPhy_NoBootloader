/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void SB_Uart_Driver_Init( UART_T* uart, uint32_t ulBaudRate, uint8_t ucDataBits , uint8_t eParity );

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__UART_DRIVER_H__ */