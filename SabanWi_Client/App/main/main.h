/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "NUC200Series.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
#define SABAN_CLIENT 1

void Error_Handler(void);
extern volatile uint32_t g_u32WDTINTCounts;
extern volatile uint32_t g_u8IsWDTWakeupINT ;

typedef struct
{ 
		uint8_t masterID;
		uint8_t cmd;
		uint8_t mccode ;
		uint8_t slaveID;
		uint8_t data_h;                         
		uint8_t data_l;                        
		uint8_t input_type ;                   // 0: NORMAL 1: FLASH 
		uint16_t crcResult;
	  uint8_t Modbus_test ;                  // 0: Normal 1: Test Modbus Slave 02 : Test Modbus Master 
	  uint8_t Modbus_mode ;                  // 1: Master 0: Slave
	  uint16_t Modbus_value[16] ;
		int err ;
} Saban;

extern Saban device[200] ;

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */