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
#define SABAN_CLIENT 0

void Error_Handler(void);
extern volatile uint32_t g_u32WDTINTCounts;
extern volatile uint32_t g_u8IsWDTWakeupINT ;

typedef struct
{
	  uint8_t Mode_work ;                    // 0 : NORMAL 1: HMI Goi Lieu 
    uint8_t Modbus_test ;                  // 0: Normal 1: Test Modbus Slave 02 : Test Modbus Master
    uint8_t Modbus_mode ;                  // 1: Master 0: Slave
} Saban;

typedef struct
{
      uint8_t masterid ;                                                   
      uint8_t cmd ;                          
      uint8_t mccode ;
      uint8_t slaveid ;	
      uint8_t data_h ;                                                       
      uint8_t data_l ;
      uint16_t crc_result ; 	
	    int err ;
}Packet_Rreceive_Data ;  

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

typedef struct
{
      uint8_t ClientID ;                     // PCConfig                                  11    15    19    23
      uint8_t Systemcode ;                   // PCConfig                                  12    16    20    24
      uint8_t DataH ;                        // PCConfig                                  13    17    21    25
      uint8_t DataL ;                        // PCConfig                                  14    18    22    26

}Saban_Device_Dataflash ;  

#define MODE_WORK_NORMAL  0 
#define MODE_WORK_HMI     1
#define MODE_WORK_HMI_FEEDBACK_HMI_LOGIN     2

extern Saban device_word_mode ;
extern Saban_Master_Dataflash MasterDataFlash;
extern Saban_Device_Dataflash DeviceDataFlash[200];
extern Packet_Rreceive_Data pkg_client_recv[255] ; 

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */