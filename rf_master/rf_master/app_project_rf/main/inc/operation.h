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

void Radio_Start(Saban_Master_Dataflash *master_pkg);
void OnMaster(Saban *t_device ,Packet_Rreceive_Data recv_pkg[], Saban_Master_Dataflash *master_pkg 
	            , Saban_Device_Dataflash device_pkg[]);
void Modbus_Init(Saban *t_device);
void Modbus_Start(Saban *t_device , Saban_Master_Dataflash *master_pkg);
void Modbus_Test_PC(Saban *t_device , Saban_Master_Dataflash *master_pkg);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__OPERATION_H__ */