
#include "uart_driver.h"
#include "trans_recieve_buff_control.h"

#include "mb.h"
#include "mbport.h"

static void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

static void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}

/*-----------------------------------------------------------------------*/
/* Uart use for Modbus */
/*-----------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    if (device_word_mode.Modbus_mode == 0)
    {
        if (UART_GET_INT_FLAG(UART1, UART_ISR_RDA_INT_Msk))
        {
            /* Get all the input characters */
            if (UART_IS_RX_READY(UART1))
            {
                prvvUARTRxISR();
            }
        }
        else if (UART_GET_INT_FLAG(UART1, UART_ISR_THRE_INT_Msk))
        {
            prvvUARTTxReadyISR();
            UART_WAIT_TX_EMPTY(UART1) ;
        }
    }
    if (device_word_mode.Modbus_mode == 1)
    {
        uint32_t u32IntSts = UART1->ISR;
        if (u32IntSts & UART_ISR_RDA_INT_Msk)
        {
            Modbus_Master_Rece_Handler();
        }
    }
}