#include "Systerm.h"

void SB_SYS_Init(void)
{
    SYS_UnlockReg();
    /* Enable Internal RC 22.1184 MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC10K_EN_Msk | CLK_PWRCON_OSC22M_EN_Msk | CLK_PWRCON_XTL12M_EN_Msk);
    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC10K_STB_Msk | CLK_CLKSTATUS_OSC22M_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk);

    CLK_DisablePLL();
    /* Set PLL frequency */
    CLK->PLLCON = (CLK->PLLCON & ~(0x000FFFFFUL)) | 0x0000C22EUL;
    /* Waiting for PLL ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk);

    /*Set HCLK Clock */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));
    /* Set core clock rate as PLL_CLOCK from PLL */
    CLK_SetCoreClock(50000000);

    CLK_EnableCKO(CLK_CLKSEL2_FRQDIV_S_HIRC, 0, 0);
    CLK_EnableModuleClock(ISP_MODULE);
    CLK_EnableSysTick(CLK_CLKSEL0_STCLK_S_HIRC_DIV2, 0);

}