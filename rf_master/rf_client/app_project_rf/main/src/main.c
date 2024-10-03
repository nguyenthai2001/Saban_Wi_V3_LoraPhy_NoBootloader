
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

Saban device ;
Saban_Client_Dataflash client_dataflash;

int main(void)
{
    SB_SYS_Init();

    SB_Uart_Driver_Init(UART1, 115200, 8, 0);
    SB_SPI_Driver_Init();
    SB_WDT_Driver_Init();
    SB_Timer_Driver_Init();
    SB_Usb_Driver_Init();
    SB_Client_GPIO_Init(&client_dataflash);
    All_Led_ON();
    CLK_SysTickDelay(5000000);
    All_Led_Off();

    DataFlash_Client_Init(&device, &client_dataflash);

#if MODBUS_ENABLE
    Modbus_Init(&device, &client_dataflash);
#else
#endif
    Radio_Start(&client_dataflash);

    while (TRUE)
    {
        g_u32WDTINTCounts = 0 ;
        if (device.Modbus_test == 0x00)
        {
            OnClient(&device, &client_dataflash);
#if MODBUS_ENABLE
            Modbus_Start(&device, &client_dataflash);
#else
#endif
        }
        else
        {
            Modbus_Test_PC(&device, &client_dataflash);
        }
    }
    return 0 ;
}