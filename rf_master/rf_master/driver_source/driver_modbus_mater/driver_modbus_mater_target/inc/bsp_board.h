/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_BOARD_H
#define __BSP_BOARD_H
/* Includes ------------------------------------------------------------------*/

#include "NUC200Series.h"
#include "timer_driver.h"
typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;
//extern uint32_t HAL_GetTick(void);
uint32_t Timer0_GetTick(void);
void Timer0_Delay(uint32_t Delay);
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */


#endif
/********END OF FILE****/
