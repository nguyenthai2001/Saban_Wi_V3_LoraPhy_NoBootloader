/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATAFLASH_STACK_H__
#define __DATAFLASH_STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dataflash_driver.h"

/* USER CODE BEGIN Includes */

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
extern Saban_Client_Dataflash ClientDataFlash[2];

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void DataFlash_Client_Init(void);
void Update_DataFlash_From_Master (void);
uint8_t Update_DataFlash_InputType_From_PC(uint8_t inputtype , uint32_t timeflash );
uint8_t Update_DataFlash_ID_From_PC( uint8_t data1  , uint8_t data2 );
uint8_t Update_DataFlash_RFConfig_From_PC(uint8_t freg , uint8_t power , uint8_t bw , uint8_t sf , uint8_t errcode , uint16_t rxtimeout);
uint8_t Update_DataFlash_ModbusConfig_From_PC(uint8_t mbregister , uint8_t mbslaveid , uint8_t mbbaudrate , uint8_t address , uint16_t timeout);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__DATAFLASH_STACK_H__ */