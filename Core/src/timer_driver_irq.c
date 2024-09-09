#include "timer_driver.h"
#include "gpio_control.h"
/*-----------------------------------------------------------------------*/
/* Timer use for Lora Rxtimeout */
/*-----------------------------------------------------------------------*/
void TMR0_IRQHandler(void)                            
{
		if(TIMER_GetIntFlag(TIMER0) == 1)
		{
				tick ++ ;
				TIMER_ClearIntFlag(TIMER0);
		}
}

/*---------------------------------------------------------------------*/
/* Timer use for Client status */
/*----------------------------------------------------------------------*/
void TMR1_IRQHandler(void)
{
	  uint16_t pos = 0 ;
		if(TIMER_GetIntFlag(TIMER1) == 1)
		{                      
				TIMER_ClearIntFlag(TIMER1);
				tick_timer1 ++ ;	
        for(pos = 0; pos<20;pos++)
				{
						Led_Display(pos);                  
				}  			
		}
}

/*-------------------------------------------------------------*/
/* Timer use for Modbus */
/*-------------------------------------------------------------*/
void TMR2_IRQHandler(void)
{
		tick_timer2 ++ ;
}

/*-----------------------------------------------------------------------*/
/* Timer use for calaculation function timeout  */
/*-----------------------------------------------------------------------*/
void TMR3_IRQHandler(void)                            
{
		if(TIMER_GetIntFlag(TIMER3) == 1)
		{
				tick_timer3 ++ ;
				TIMER_ClearIntFlag(TIMER3);
		}
}

