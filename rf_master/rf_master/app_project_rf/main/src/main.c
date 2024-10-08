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

Saban device_word_mode ;
Packet_Rreceive_Data pkg_client_recv[255] ;
Saban_Master_Dataflash MasterDataFlash;
Saban_Device_Dataflash DeviceDataFlash[200];

int main(void)
{
    SB_SYS_Init();

    SB_Master_GPIO_Init();
    All_Led_ON();
    CLK_SysTickDelay(1000000);
    All_Led_Off();
    GPIO_SetMode(PA, BIT8, GPIO_PMD_OUTPUT);

    SB_Uart_Driver_Init(UART1, 115200, 8, 0);
    SB_SPI_Driver_Init();
    SB_WDT_Driver_Init();
    SB_Timer_Driver_Init();
    SB_Usb_Driver_Init();

    DataFlash_Master_Init(&device_word_mode, &MasterDataFlash, DeviceDataFlash);

    Modbus_Init(&device_word_mode);

    Radio_Start(&MasterDataFlash);

    device_word_mode.Mode_work = MODE_WORK_NORMAL ;

    while (TRUE)
    {
        g_u32WDTINTCounts = 0 ;
        if (device_word_mode.Modbus_test == 0x00)
        {
            OnMaster(&device_word_mode, pkg_client_recv, &MasterDataFlash, DeviceDataFlash);
            Modbus_Start(&device_word_mode, &MasterDataFlash);
        }
        else
        {
            Modbus_Test_PC(&device_word_mode, &MasterDataFlash);
        }
    }
}