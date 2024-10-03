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
 * File: $Id: portserial.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

#include "uart_driver.h"
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define MODBUS_SLAVE_V1
//static void     prvvUARTTxReadyISR(void);
//static void     prvvUARTRxISR(void);

/* ----------------------- Start implementation -----------------------------*/
void  vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    __disable_irq();

    if(xRxEnable)
    {          
        UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk);
          PA8 = 0;
    }
    else
    {
          PA8 = 1;
        UART_DISABLE_INT(UART1, UART_IER_RDA_IEN_Msk);
    }
    if(xTxEnable)
    {           
        UART_ENABLE_INT(UART1, UART_IER_THRE_IEN_Msk);
    }
    else
    {         
        UART_DISABLE_INT(UART1, UART_IER_THRE_IEN_Msk);
    }
        
    NVIC_EnableIRQ(UART1_IRQn);
		NVIC_SetPriority(UART1_IRQn,2);
    __enable_irq();
    
}

void vMBPortClose(void)
{
    /* Disable Interrupt */
    UART_DISABLE_INT(UART1, UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk);
    NVIC_DisableIRQ(UART1_IRQn);
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
      
    GPIO_SetMode(PA,BIT8,GPIO_PMD_OUTPUT);                // PA8 = 1 send PA8 = 0 recv 
      
    if(ucPORT != 0) return FALSE;
    switch(eParity)
    {
        case MB_PAR_NONE:
            SB_Uart_Driver_Init(UART1,ulBaudRate,ucDataBits,0);
            break;
        case MB_PAR_EVEN:
            SB_Uart_Driver_Init(UART1,ulBaudRate,ucDataBits,1);
            break;
        case MB_PAR_ODD:
            SB_Uart_Driver_Init(UART1,ulBaudRate,ucDataBits,2);
            break;
    }
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{

    UART1->THR = (uint8_t) ucByte;
    while(UART_GET_TX_EMPTY(UART1) != 0x00); //check Tx Empty
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{

    while(UART_GET_RX_FULL(UART1) != 0x00); //check Rx Empty
    *pucByte = UART1->RBR;
    return TRUE;
}

///*
// * Create an interrupt handler for the transmit buffer empty interrupt
// * (or an equivalent) for your target processor. This function should then
// * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
// * a new character can be sent. The protocol stack will then call
// * xMBPortSerialPutByte( ) to send the character.
// */
//static void
//prvvUARTTxReadyISR(void)
//{
//    pxMBFrameCBTransmitterEmpty();
//}

///*
// * Create an interrupt handler for the receive interrupt for your target
// * processor. This function should then call pxMBFrameCBByteReceived( ). The
// * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
// * character.
// */
//static void
//prvvUARTRxISR(void)
//{
//    pxMBFrameCBByteReceived();
//}
