/*
 * FreeModbus Libary: LPC214X Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include <NUC200Series.h>
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define MODBUS_SLAVE_V1

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit(USHORT usTim1Timerout50us)
{
    /* Enable Timer0 clock source */
    CLK_EnableModuleClock(TMR2_MODULE);
    /* Select Timer0 clock source as external 12M */
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2_S_HIRC, MODULE_NoMsk);
    /* Reset IP TMR0 */
    SYS_ResetModule(TMR2_RST);

    /* Select timer0 Operation mode as period mode */
    TIMER2->TCSR &= ~TIMER_TCSR_MODE_Msk;
    TIMER2->TCSR |= TIMER_PERIODIC_MODE;

    /* Select Time out period = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
    TIMER_SET_PRESCALE_VALUE(TIMER2, 0);        // Set Prescale [0~255]
    TIMER_SET_CMP_VALUE(TIMER2, 12 * 50 * usTim1Timerout50us);  // Set TCMPR [0~16777215]

    /* Enable Timer0 interrupt */
    TIMER_EnableInt(TIMER2);
    /* Enable Timer0 NVIC */
    NVIC_EnableIRQ(TMR2_IRQn);

    return TRUE;
}

void
vMBPortTimersEnable()
{
    /* Reset Timer0 counter */
    TIMER2->TCSR |= TIMER_TCSR_CRST_Msk;
    /* Enable Timer0 */
    TIMER2->TCSR |= TIMER_TCSR_CEN_Msk;

}

void
vMBPortTimersDisable()
{

    /* Disable Timer0 */
    TIMER2->TCSR &= ~TIMER_TCSR_CEN_Msk;
    /* Reset Timer0 counter */
    TIMER2->TCSR |= TIMER_TCSR_CRST_Msk;

}

//#ifdef MODBUS_SLAVE_V1
//void TMR2_IRQHandler(void)
//{
//    /* Clear Timer0 interrupt flag */
//    TIMER2->TISR |= TIMER_TISR_TIF_Msk;

//    (void)pxMBPortCBTimerExpired();
//
////}
//#endif



