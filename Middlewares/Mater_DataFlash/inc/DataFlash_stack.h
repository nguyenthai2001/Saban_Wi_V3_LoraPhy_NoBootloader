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

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
typedef struct
{
      uint8_t RfFrequence ;                  // PCConfig                                  0
      uint8_t RFPower ;                      // PCConfig                                  1
      uint8_t RFBandwidth ;                  // PCConfig                                  2
      uint8_t RFSpreadingFactor ;            // PCConfig                                  3
      uint8_t ErrCode ;                      // PCConfig                                  4
      uint8_t Masterid ;                     // PCConfig                                  5
      uint8_t DeviceNumber ;                 // PCConfig                                  6
      uint8_t Security ;                     // RFFrequence  = Lora Mode + Freq + Security  
	    uint8_t Modbus_SlaveID ;               // PC config    1...247                      7
      uint8_t Modbus_Baudrate ;              // PC config          default 115200         8
      uint8_t Modbus_Register ;              // PC config          00: input 01: Holding 10 : coil 11: decrete          9
      uint8_t Modbus_Address ;               // PC Config    1...1000                     10
}Saban_Master_Dataflash ;  

extern Saban_Master_Dataflash MasterDataFlash[2];

typedef struct
{
      uint8_t ClientID ;                     // PCConfig                                  11    15    19    23
      uint8_t Systemcode ;                   // PCConfig                                  12    16    20    24
      uint8_t DataH ;                        // PCConfig                                  13    17    21    25
      uint8_t DataL ;                        // PCConfig                                  14    18    22    26

}Saban_Device_Dataflash ;  

extern Saban_Device_Dataflash DeviceDataFlash[200];

void DataFlash_Master_Init(void);
uint8_t Update_DataFlashDevice_From_PC(uint8_t stt , uint8_t ClientID , uint8_t Systemcode , uint8_t datah , uint8_t datal );
uint8_t Update_DataFlash_ModbusConfig_From_PC(uint8_t mbregister , uint8_t mbslaveid , uint8_t mbbaudrate , uint8_t address);
uint8_t Update_DataFlash_RFConfig_From_PC(uint8_t freg , uint8_t power , uint8_t bw , uint8_t sf , uint8_t errcode , uint16_t rxtimeout);
uint8_t Update_DataFlash_ID_From_PC(uint8_t data1  , uint8_t data2 );

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__DATAFLASH_STACK_H__ */