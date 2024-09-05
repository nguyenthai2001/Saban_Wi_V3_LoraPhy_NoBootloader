/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_DRIVER_H__
#define __TIMER_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "timer.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void SB_Timer_Driver_Init(void);

void Timer_SetTickMs(uint16_t ms);
uint16_t Timer_GetTickMs(void);
void Timer_ResetTickMs(void);

void Timer_1_SetTickMs(uint16_t ms);
uint16_t Timer_1_GetTickMs(void);
void Timer1_ResetTickMs(void);

void Timer3_SetTickMs(void);
uint16_t Timer3_GetTickMs(void);
void Timer3_ResetTickMs(void);

extern uint16_t tick ;
extern uint16_t tick_timer1 ;
extern uint16_t tick_timer3 ;
extern uint16_t tick_timer2 ;

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__TIMER_DRIVER_H__ */