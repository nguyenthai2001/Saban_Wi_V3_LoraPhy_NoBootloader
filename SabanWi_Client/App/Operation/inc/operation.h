/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OPERATION_H__
#define __OPERATION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void Radio_Start(void);
void OnClient (void) ;
void Modbus_Init(void);
uint8_t Modbus_Start(void);
void Modbus_Test_PC(void);
void Client_Get_HMI_Data(uint16_t t_modbus_result[]);

typedef struct
{
    uint8_t machineCode;	  
    uint8_t line;
	 uint8_t lane ;
	 uint8_t partNumber[20];
	 uint8_t slot ;
	 uint16_t Number ;
	 uint8_t level ;
	 uint8_t status ;	
} pkg_feedback_status_request;


/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__OPERATION_H__ */