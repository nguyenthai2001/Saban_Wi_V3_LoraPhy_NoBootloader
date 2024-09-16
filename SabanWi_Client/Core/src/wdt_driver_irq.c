#include "main.h"

volatile uint32_t g_u32WDTINTCounts;
volatile uint32_t g_u8IsWDTWakeupINT ;

void WDT_IRQHandler(void)
{
    if (g_u32WDTINTCounts < 1)
        WDT_RESET_COUNTER();

    if (WDT_GET_TIMEOUT_INT_FLAG() == 1)
    {
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG();
        g_u32WDTINTCounts ++;
    }

    if (WDT_GET_TIMEOUT_WAKEUP_FLAG() == 1)
    {
        /* Clear WDT time-out wake-up flag */
        WDT_CLEAR_TIMEOUT_WAKEUP_FLAG();
        printf(" WDT Wake Up!!!! \n");
        g_u8IsWDTWakeupINT = 1;
    }

}