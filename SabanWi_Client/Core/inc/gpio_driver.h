/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_DRIVER_H__
#define __GPIO_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define PORT_IN1 PA 
#define PORT_IN2 PC 
#define PORT_IN3 PC 
#define PORT_IN4 PB 
#define PORT_IN5 PB 
#define PORT_IN6 PA 
#define PORT_IN7 PA 
#define PORT_IN8 PA 

#define PORT_OUT1 PA 
#define PORT_OUT2 PA 
#define PORT_OUT3 PA 
#define PORT_OUT4 PA 
#define PORT_OUT5 PA 
#define PORT_OUT6 PA 
#define PORT_OUT7 PA 
#define PORT_OUT8 PA 

#define PIN_IN1 BIT6
#define PIN_IN2 BIT7
#define PIN_IN3 BIT6
#define PIN_IN4 BIT15
#define PIN_IN5 BIT8
#define PIN_IN6 BIT11
#define PIN_IN7 BIT10
#define PIN_IN8 BIT9

#define PIN_OUT1 BIT4
#define PIN_OUT2 BIT3
#define PIN_OUT3 BIT2
#define PIN_OUT4 BIT1
#define PIN_OUT5 BIT0
#define PIN_OUT6 BIT12
#define PIN_OUT7 BIT13
#define PIN_OUT8 BIT5

#define LED1	PA6
#define LED2	PC7
#define LED3	PC6
#define LED4	PB15
#define LED5	PB8
#define LED6	PA11
#define LED7	PA10
#define LED8	PA9

#define LED9	PA5
#define LED10	PA4
#define LED11	PA3
#define LED12	PA2
#define LED13	PA1
#define LED14	PA0
#define LED15	PA12
#define LED16	PA13

/* USER CODE END Private defines */

void GPIO_Client_Init(void);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_DRIVER_H__ */