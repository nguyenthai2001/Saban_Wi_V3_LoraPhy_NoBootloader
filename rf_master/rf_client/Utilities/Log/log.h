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
void printArray8Bit(uint8_t arr[], int size) ;
void print_uint8_array_as_chars(uint8_t arr[], size_t length) ;
size_t copyUint16ToUint8(const uint16_t *src, uint8_t *dest, size_t length_src, size_t length_dest) ;
void covertUint16ToUint8(const uint16_t *src, uint8_t *dest, size_t length) ;
void splitArrayByMarker(const unsigned char *inputArray, int inputSize, unsigned char **beforeMarker, int *beforeSize, unsigned char **afterMarker, int *afterSize);
void splitArrayByByte(const unsigned char *input, size_t size, unsigned char beforeArray[30], size_t *beforeSize, unsigned char afterArray[30], size_t *afterSize) ;

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__LOG_H__ */