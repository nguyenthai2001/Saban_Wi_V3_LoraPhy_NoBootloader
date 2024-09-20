/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>  // C?n d? dùng malloc và free
#include <stdarg.h>  // Thu vi?n h? tr? truy?n d?i s? bi?n d?i
/* USER CODE BEGIN Includes */ 

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

void log_message(const char *format, ...) ;
void printArray8Bit(uint8_t arr[], int size);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__LOG_H__ */