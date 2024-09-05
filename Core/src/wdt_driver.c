#include "wdt_driver.h"

void SB_WDT_Driver_Init(void)
{
		CLK_EnableModuleClock(WDT_MODULE);
		CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDT_S_LIRC, MODULE_NoMsk);      
		/* To check if system has been reset by WDT time-out reset or not */      
		if(WDT_GET_RESET_FLAG() == 1)                              
		{                                      
					WDT_CLEAR_RESET_FLAG();
					printf("*** System has been reset by WDT time-out event ***\n\n");
					while(1);
		} 
		/* Set WDT :
			- clock source: LIRC
			- Time-out interval: 2POWn :  2^n * WDT clock ( LIRC is 10kHz) [ms]
			- Reset delay: WDT_RESET_DELAY_nCLK : n * WDT clock  [ms]
			- Enable Reset
			- Enable Wake-up
		*/    
		WDT_Open( WDT_TIMEOUT_2POW16, WDT_RESET_DELAY_1026CLK, TRUE, TRUE);
		/* Enable WDT interrupt */
		WDT_EnableInt();
		NVIC_EnableIRQ(WDT_IRQn);	
		NVIC_SetPriority(WDT_IRQn,1);
}