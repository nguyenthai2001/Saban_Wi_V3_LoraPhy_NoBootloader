#include "usb_stack.h"

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen)
{
		return 0 ;
}

/*write cmd*/
void HID_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    ProcessCommand((uint8_t *)&pu8EpBuf[0], u32Size);	
}

/*read cmd*/
void HID_SetInReport(void)
{

}