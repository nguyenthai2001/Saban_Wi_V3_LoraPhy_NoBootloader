
#include "bsp_board.h"

//This function provides accurate delay (in milliseconds) based on variable incremented.
void Timer0_Delay(uint32_t Delay)
{
    uint32_t tickstart = Timer2_GetTickMs();
    uint32_t wait = Delay;

    while ((Timer2_GetTickMs() - tickstart) < wait)
    {
    }
}
