/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "NUC200Series.h"
#include "log.h"

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

#define MODBUS_ENABLE 1
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

typedef struct
{
      uint8_t System_code ;                  // Data Master send                          0
      uint8_t SlaveID ;                      // PC Config   1 ... 199                     1
      uint8_t input_type ;                   // PC config   0: NORMAL 1: FLASH            2
      uint8_t output_number ;                // Data Master Send 0: OFF 1: ON             3
      uint32_t timefalsh ;                   // PC config                                 4 5 6 7
      uint8_t Modbus_SlaveID ;               // Data Master send   1...247                8
      uint8_t Modbus_Baudrate ;              // PC config          default 115200         9
      uint8_t Modbus_Register ;              // PC config          00: input 01: Holding 10 : coil 11: decrete          10
      uint8_t Modbus_Address ;               // Data Master Send   1...1000               11
      uint16_t Modbus_Timeout ;              // PCconfig                                  12 13
      uint8_t RfFrequence ;                  // PCConfig                                  14
      uint8_t RFPower ;                      // PCConfig                                  15
      uint8_t RFBandwidth ;                  // PCConfig                                  16
      uint8_t RFSpreadingFactor ;            // PCConfig                                  17
      uint8_t ErrCode ;                      // PCConfig                                  18
      uint16_t RXTimeout ;                   // PCConfig                                  19 20
      uint8_t Security ;
}Saban_Client_Dataflash ;

extern Saban device ;
extern Saban_Client_Dataflash client_dataflash;

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */