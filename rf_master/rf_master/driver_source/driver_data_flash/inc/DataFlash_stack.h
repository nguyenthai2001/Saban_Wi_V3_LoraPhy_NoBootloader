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

void DataFlash_Master_Init( Saban *t_device,Saban_Master_Dataflash *master_pkg , Saban_Device_Dataflash device_pkg[]);
uint8_t Update_DataFlashDevice_From_PC(Saban_Device_Dataflash device_pkg[],uint8_t stt, uint8_t ClientID, uint8_t Systemcode, uint8_t datah, uint8_t datal);
uint8_t Update_DataFlash_ModbusConfig_From_PC(Saban_Master_Dataflash *master_pkg ,uint8_t mbregister, uint8_t mbslaveid, uint8_t mbbaudrate, uint8_t address);
uint8_t Update_DataFlash_RFConfig_From_PC(Saban_Master_Dataflash *master_pkg ,uint8_t freg, uint8_t power, uint8_t bw, uint8_t sf, uint8_t errcode, uint16_t rxtimeout);
uint8_t Update_DataFlash_ID_From_PC( Saban_Master_Dataflash *master_pkg ,uint8_t data1, uint8_t data2);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__DATAFLASH_STACK_H__ */