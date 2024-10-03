#include "gpio_control.h"
#include "gpio_driver.h"

uint16_t combinedValue = 0;

/**
*    @brief      DvStatus_Display
*
*    @param[in]   pos      vi tri LED
*    @param[in]  status   Trang thai
                DISCONNECT: Led tat
                CONNECT: Led sang
                ERROR: nhap nhay
*    @return

*    @details    Cap nhat trang thai thiet bi len LED v? bao BUZZ
*/
void Led_Display(Packet_Rreceive_Data pkg[], uint16_t pos)
{
    if (pkg[pos].cmd == CMD_IO_STANDAND)
    {
        combinedValue = ((uint16_t)pkg[pos].data_h << 8) | pkg[pos].data_l;
        switch (pos)
        {
        case 1:
            if (combinedValue == DISCONNECT)                     LED1 = 1;
            else if (combinedValue == 0x5555)                                LED1 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)   LED1 = ~LED1;
            break;
        case 2:
            if (combinedValue == DISCONNECT)                     LED2 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED2 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED2 = ~LED2;
            break;

        case 3:
            if (combinedValue == DISCONNECT)                     LED3 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED3 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED3 = ~LED3;
            break;
        case 4:
            if (combinedValue == DISCONNECT)                     LED4 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED4 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED4 = ~LED4;
            break;
        case 5:
            if (combinedValue == DISCONNECT)                     LED5 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED5 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED5 = ~LED5;
            break;
        case 6:
            if (combinedValue == DISCONNECT)                                    LED6 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED6 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED6 = ~LED6;
            break;
        case 7:
            if (combinedValue == DISCONNECT)                     LED7 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED7 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED7 = ~LED7;
            break;
        case 8:
            if (combinedValue == DISCONNECT)                     LED8 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED8 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED8 = ~LED8;
            break;
        case 9:
            if (combinedValue == DISCONNECT)                     LED9 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED9 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED9 = ~LED9;
            break;
        case 10:
            if (combinedValue == DISCONNECT)                     LED10 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED10 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED10 = ~LED10;
            break;
        case 11:
            if (combinedValue == DISCONNECT)                     LED11 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED11 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED11 = ~LED11;
            break;
        case 12:
            if (combinedValue == DISCONNECT)                     LED12 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED12 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED12 = ~LED12;
            break;
        case 13:
            if (combinedValue == DISCONNECT)                     LED13 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED13 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED13 = ~LED13;
            break;
        case 14:
            if (combinedValue == DISCONNECT)                     LED14 = 1;
            else if (combinedValue == 0x5555 && combinedValue != DISCONNECT)  LED14 = 0;
            else if (combinedValue != 0x5555 && combinedValue != DISCONNECT)  LED14 = ~LED14;
            break;

        }
    }

    if (pkg[pos].cmd == CMD_RS485)
    {
        uint8_t rs485addres = pkg[pos].data_h ;
        switch (pos)
        {
        case 1:
            if (rs485addres == DISCONNECT)                       LED1 = 1;
            else                                                    LED1 = 0;
            break;
        case 2:
            if (rs485addres == DISCONNECT)                       LED2 = 1;
            else                                                    LED2 = 0;
            break;

        case 3:
            if (rs485addres == DISCONNECT)                       LED3 = 1;
            else                                                      LED3 = 0;
            break;
        case 4:
            if (rs485addres == DISCONNECT)                       LED4 = 1;
            else                                                    LED4 = 0;
            break;
        case 5:
            if (rs485addres == DISCONNECT)                       LED5 = 1;
            else                                                    LED5 = 0;
            break;
        case 6:
            if (rs485addres == DISCONNECT)                       LED6 = 1;
            else                                                    LED6 = 0;
            break;
        case 7:
            if (rs485addres == DISCONNECT)                       LED7 = 1;
            else                                                    LED7 = 0;
            break;
        case 8:
            if (rs485addres == DISCONNECT)                       LED8 = 1;
            else                                                    LED8 = 0;
            break;
        case 9:
            if (rs485addres == DISCONNECT)                       LED9 = 1;
            else                                                    LED9 = 0 ;
            break;
        case 10:
            if (rs485addres == DISCONNECT)                       LED10 = 1;
            else                                                      LED10 = 0;
            break;
        case 11:
            if (rs485addres == DISCONNECT)                       LED11 = 1;
            else                                                    LED11 = 0;
            break;
        case 12:
            if (rs485addres == DISCONNECT)                       LED12 = 1;
            else                                                    LED12 = 0;
            break;
        case 13:
            if (rs485addres == DISCONNECT)                       LED13 = 1;
            else                                                    LED13 = 0;
            break;
        case 14:
            if (rs485addres == DISCONNECT)                       LED14 = 1;
            else                                                    LED14 = 0;
            break;
        }
    }
}