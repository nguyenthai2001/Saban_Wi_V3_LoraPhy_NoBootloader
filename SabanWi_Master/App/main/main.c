#include "main.h"
#include "Systerm.h"
#include "gpio_driver.h"
#include "uart_driver.h"
#include "usb_driver.h"
#include "wdt_driver.h"
#include "timer_driver.h"
#include "spi_driver.h"
#include "DataFlash_stack.h"
#include "operation.h"

Saban device[200] ;

int main(void)
{
    SB_SYS_Init();

    SB_Uart_Driver_Init(UART1, 115200, 8, 0);
    SB_SPI_Driver_Init();
    SB_WDT_Driver_Init();
    SB_Timer_Driver_Init();
    SB_Usb_Driver_Init();
    SB_Master_GPIO_Init();

    All_Led_ON();
    CLK_SysTickDelay(1000000);
    All_Led_Off();
    GPIO_SetMode(PA, BIT8, GPIO_PMD_OUTPUT);

    DataFlash_Master_Init();

    //Modbus_Init();

    Radio_Start();

    while (TRUE)
    {
        g_u32WDTINTCounts = 0 ;
        if (device[1].Modbus_test == 0x00)
        {
            OnMaster();
            //Modbus_Start();
        }
        else
        {
            //Modbus_Test_PC();
        }
    }

}