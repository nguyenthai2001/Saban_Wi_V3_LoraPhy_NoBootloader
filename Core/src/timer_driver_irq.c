#include "timer_driver.h"

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
		if(TIMER_GetIntFlag(TIMER1) == 1)
		{                      
				TIMER_ClearIntFlag(TIMER1);
				tick_timer1 ++ ;				
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

