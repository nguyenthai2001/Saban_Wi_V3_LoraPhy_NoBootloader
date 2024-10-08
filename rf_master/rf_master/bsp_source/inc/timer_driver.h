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

typedef struct
{
      uint16_t tick ;
      uint16_t tick_timer1 ;
      uint32_t tick_timer3 ;
      uint16_t tick_timer2 ;
}Tick_Timer ;  

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void SB_Timer0_Init(void);
void Timer_SetTickMs(uint16_t ms);
uint16_t Timer_GetTickMs(void);
void Timer_ResetTickMs(void);

void SB_Timer1_Init(void);
void Timer_1_SetTickMs(void);
uint16_t Timer_1_GetTickMs(void);
void Timer1_ResetTickMs(void);

void SB_Timer2_Init(void);
uint16_t Timer2_GetTickMs(void);
void Timer2_ResetTickMs(void);

void SB_Timer3_Init(void);
void Timer3_SetTickMs(void);
uint32_t Timer3_GetTickMs(void);
void Timer3_ResetTickMs(void);

void SB_Timer_Driver_Init(void);


/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__TIMER_DRIVER_H__ */