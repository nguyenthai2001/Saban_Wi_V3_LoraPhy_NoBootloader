#include "main.h"
#include "Systerm.h"
#include "gpio_driver.h"
#include "uart_driver.h"
#include "usb_driver.h"
#include "wdt_driver.h"
#include "timer_driver.h"
#include "dataflash_driver.h"

int main (void)
{
	  uint32_t BaseAddr = 0;
	  SB_SYS_Init(); 
	  
	  SB_Uart_Driver_Init(UART1,115200,8,0);
	 /* Enable FMC ISP function */
		FMC_Open();
		/* Set Data Flash base address */
		if (SetDataFlashBase(DATA_FLASH_EEPROM_BASE) < 0)
		{
					printf("Failed to set Data Flash base address!\n");
					while (1) {}
		}
		/* Read Data Flash base address */
		BaseAddr = FMC_ReadDataFlashBaseAddr();
		printf("[Simulate EEPROM base address at Data Flash 0x%x]\n\n", BaseAddr);
	 
		while(1)
		{
			  g_u32WDTINTCounts = 0 ;			 
		}
			
		return 0 ;
}