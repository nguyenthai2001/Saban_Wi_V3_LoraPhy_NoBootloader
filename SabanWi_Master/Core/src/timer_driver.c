#include "timer_driver.h"

uint16_t tick ;
uint16_t tick_timer1 ;
uint16_t tick_timer3 ;
uint16_t tick_timer2 ;

void SB_Timer0_Init(void)
{
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, MODULE_NoMsk);
    CLK_EnableModuleClock(TMR0_MODULE);
    SYS_ResetModule(TMR0_RST);
    TIMER0->TCSR &= ~TIMER_TCSR_CRST_Msk; // Reset TIMER1
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);   // 1tick
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0x17);       // 23
    TIMER_SET_CMP_VALUE(TIMER0, 0x3E8);           // 1ms
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
}

void SB_Timer1_Init(void)
{
    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HIRC, MODULE_NoMsk);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(TMR1_MODULE);
    SYS_ResetModule(TMR1_RST);
    TIMER1->TCSR &= ~TIMER_TCSR_CRST_Msk; // Reset TIMER1
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);   // 1tick
    TIMER_SET_PRESCALE_VALUE(TIMER1, 0x17);       // 23
    TIMER_SET_CMP_VALUE(TIMER1, 0x3E8);           // 1ms
    TIMER_EnableInt(TIMER1);
    tick_timer1 = 0 ;
	TIMER_Start(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
}

void SB_Timer2_Init(void)
{
    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2_S_HIRC, MODULE_NoMsk);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(TMR2_MODULE);

    SYS_ResetModule(TMR2_RST);
    TIMER2->TCSR &= ~TIMER_TCSR_CRST_Msk; // Reset TIMER1
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 1);  // 1tick
    TIMER_SET_PRESCALE_VALUE(TIMER2, 0x17);       // 23
    TIMER_SET_CMP_VALUE(TIMER2, 0x3E8);           // 500ms
    TIMER_EnableInt(TIMER2);
    NVIC_EnableIRQ(TMR2_IRQn);
    NVIC_SetPriority(TMR2_IRQn, 5);
    tick_timer2 = 0 ;
//      TIMER_Start(TIMER2);
}

void SB_Timer3_Init(void)
{
    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3_S_HIRC, MODULE_NoMsk);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(TMR3_MODULE);

    SYS_ResetModule(TMR3_RST);
    TIMER3->TCSR &= ~TIMER_TCSR_CRST_Msk; // Reset TIMER1
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1);   // 1tick
    TIMER_SET_PRESCALE_VALUE(TIMER3, 0x17);       // 23
    TIMER_SET_CMP_VALUE(TIMER3, 0x1);           // 1ms
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);
}

void Timer_SetTickMs(uint16_t ms)
{
    TIMER_Start(TIMER0);
    tick = 0 ;
}

uint16_t Timer_GetTickMs(void)
{
    return tick ;
}

void Timer_ResetTickMs(void)
{
    TIMER_Stop(TIMER0);
    tick = 0;
}

void Timer_1_SetTickMs(void)
{
    TIMER_Start(TIMER1);
    tick_timer1 = 0 ;
}

uint16_t Timer_1_GetTickMs(void)
{
    return tick_timer1 ;
}

void Timer1_ResetTickMs(void)
{
    TIMER_Stop(TIMER1);
    tick_timer1 = 0;
}

uint16_t Timer2_GetTickMs(void)
{
    return tick_timer2 ;
}

void Timer2_ResetTickMs(void)
{
    TIMER_Stop(TIMER2);
    tick_timer2 = 0;
}

void Timer3_SetTickMs(void)
{
    TIMER_Start(TIMER3);
    tick_timer3 = 0 ;
}

uint16_t Timer3_GetTickMs(void)
{
    return tick_timer3 ;
}

void Timer3_ResetTickMs(void)
{
    TIMER_Stop(TIMER3);
    tick_timer3 = 0;
}

void SB_Timer_Driver_Init(void)
{
    SB_Timer0_Init();
    SB_Timer1_Init();
    SB_Timer3_Init();
}

