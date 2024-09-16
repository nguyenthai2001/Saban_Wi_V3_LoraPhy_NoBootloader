#include "uart_driver.h"

void SB_Uart_Driver_Init(UART_T* uart, uint32_t ulBaudRate, uint8_t ucDataBits, uint8_t eParity)
{
    /* Step 1. GPIO initial */
    /* Set PB multi-function pins for UART1 RXD, TXD */
    /* PB.0 --> UART1 RX, PB.1 --> UART1 TX */

    if (uart == UART1)
    {
        SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB4_Msk | SYS_GPB_MFP_PB5_Msk);
        SYS->GPB_MFP |= (SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD);
        /* Step 2. Enable and Select UART clock source*/
        /* Enable UART1 clock */
        CLK_EnableModuleClock(UART1_MODULE);
        /* UART1 clock source */
        CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
        /* Step 3. Select Operation mode */
        SYS_ResetModule(UART1_RST);
    }
    if (uart == UART0)
    {
        /* Set PB multi-function pins for UART0 RXD and TXD */
        SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
        SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);
        /* Step 2. Enable and Select UART clock source*/
        /* Enable UART1 clock */
        CLK_EnableModuleClock(UART0_MODULE);
        /* UART1 clock source */
        CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
        /* Step 3. Select Operation mode */
        SYS_ResetModule(UART0_RST);
    }
    uart->FCR |= 1 << 2;       //Tx FIFO Reset
    uart->FCR |= 1 << 1;       //Rx FIFO Reset

    /* Step 4. Set Baud-Rate to 38400*/
    /* Configure UART1 and set UART1 Baud-rate: Baud-rate 38400, 8 bits length, None parity , 1 stop bit*/

    switch (eParity)
    {
    case 0:
        uart->LCR |= UART_PARITY_NONE;
        break;
    case 1:
        uart->LCR &= ~UART_PARITY_EVEN;
        uart->LCR |= UART_PARITY_EVEN;
        break;
    case 2:
        uart->LCR &= ~UART_PARITY_EVEN;
        uart->LCR |= UART_PARITY_ODD;
        break;
    }

    switch (ucDataBits)
    {
    case 5:
        uart->LCR |= UART_WORD_LEN_5;      //5 bits Data Length
        break;
    case 6:
        uart->LCR |= UART_WORD_LEN_6;      //6 bits Data Length
        break;
    case 7:
        uart->LCR |= UART_WORD_LEN_7;      //7 bits Data Length
        break;
    case 8:
        uart->LCR |= UART_WORD_LEN_8;      //8 bits Data Length
        break;
    }

    uart->LCR |=  UART_STOP_BIT_1 ;

    uart->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, ulBaudRate) ;
}