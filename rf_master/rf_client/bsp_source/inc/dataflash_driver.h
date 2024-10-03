/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATAFLASH_DRIVER_H__
#define __DATAFLASH_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#define DATA_FLASH_EEPROM_BASE      0x1F000  /* Configure Data Flash base address for EEPROM */
#define DATA_FLASH_EEPROM_END       0x1FF00 /* Configure Data Flash end address for EEPROM */
#define DATA_FLASH_EEPROM_SIZE      (DATA_FLASH_EEPROM_END - DATA_FLASH_EEPROM_BASE)    /* Data Flash size for EEPROM */
#define EEPROM_BASE                 (DATA_FLASH_EEPROM_BASE)  /* Set EEPROM base address */
#define EEPROM_SIZE                 (DATA_FLASH_EEPROM_SIZE)  /* Set EEPROM size */

uint8_t SIM_EEPROM_READ(uint32_t address);
void SIM_EEPROM_WRITE(uint32_t address, uint8_t data);
int32_t SetDataFlashBase(uint32_t u32DFBA);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__DATAFLASH_DRIVER_H__ */