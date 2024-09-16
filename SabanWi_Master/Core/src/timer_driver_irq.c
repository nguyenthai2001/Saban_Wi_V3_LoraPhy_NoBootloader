
#include "timer_driver.h"
#include "gpio_control.h"
#include "mb.h"
#include "mbport.h"

/*-----------------------------------------------------------------------*/
/* Timer use for Lora Rxtimeout */
/*-----------------------------------------------------------------------*/
void TMR0_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER0) == 1)
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
    if (TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
        tick_timer1 ++ ;
        for (pos = 0; pos < 20; pos++)
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
    if (device[1].Modbus_mode == 0)
    {
        /* Clear Timer0 interrupt flag */
        TIMER2->TISR |= TIMER_TISR_TIF_Msk;
        (void)pxMBPortCBTimerExpired();
    }
    if (device[1].Modbus_mode == 1)
    {
        if (TIMER_GetIntFlag(TIMER2) == 1)
        {
            /* Clear Timer0 time-out interrupt flag */
            TIMER_ClearIntFlag(TIMER2);
            tick_timer2++;
        }
    }
}

/*-----------------------------------------------------------------------*/
/* Timer use for calaculation function timeout  */
/*-----------------------------------------------------------------------*/
void TMR3_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER3) == 1)
    {
        tick_timer3 ++ ;
        TIMER_ClearIntFlag(TIMER3);
    }
}

