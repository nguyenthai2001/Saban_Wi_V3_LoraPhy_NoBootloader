#include "main.h"
#include "Systerm.h"
#include "gpio_driver.h"
#include "uart_driver.h"
#include "usb_driver.h"
#include "wdt_driver.h"
#include "timer_driver.h"
#include "spi_driver.h"
#include "DataFlash_stack.h"
#include "operation.h"

Saban device[200] ;

int main (void)
{
	  uint32_t BaseAddr = 0;
	  SB_SYS_Init(); 
	  
	  SB_Uart_Driver_Init(UART1,115200,8,0);
	  SB_SPI_Driver_Init();
	  SB_WDT_Driver_Init();
	  SB_Timer_Driver_Init();
	  SB_Usb_Driver_Init();
	  SB_Master_GPIO_Init();
	
	  All_Led_ON();
	  CLK_SysTickDelay(1000000);
	  All_Led_Off();
	  
	  DataFlash_Master_Init();
	
	  Radio_Start();
	 
		while(TRUE)
		{
			  g_u32WDTINTCounts = 0 ;	
        OnMaster();			
		}
			
		return 0 ;
}