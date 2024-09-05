#include "usb_driver.h"

void SB_Usb_Driver_Init(void)
{
		CLK_SetModuleClock(USBD_MODULE, MODULE_NoMsk, CLK_CLKDIV_USB(1));
		CLK_EnableModuleClock(USBD_MODULE);
		USBD_Open(&gsInfo, HID_ClassRequest, NULL);
		HID_Init();
		USBD_Start();
		NVIC_EnableIRQ(USBD_IRQn);
		NVIC_SetPriority(USBD_IRQn , 1);
}