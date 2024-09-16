#include "gpio_driver.h"

void SB_Master_GPIO_Init(void)
{
    /*****************************OUTPUT**************************************/
    GPIO_SetMode(PORT_OUT1, PIN_OUT1, GPIO_PMD_OUTPUT);                  // Client 9
    GPIO_SetMode(PORT_OUT2, PIN_OUT2, GPIO_PMD_OUTPUT);                  // Client 10
    GPIO_SetMode(PORT_OUT3, PIN_OUT3, GPIO_PMD_OUTPUT);                  // Client 11
    GPIO_SetMode(PORT_OUT4, PIN_OUT4, GPIO_PMD_OUTPUT);                  // Client 12
    GPIO_SetMode(PORT_OUT5, PIN_OUT5, GPIO_PMD_OUTPUT);                  // Cliemt 13
    GPIO_SetMode(PORT_OUT6, PIN_OUT6, GPIO_PMD_OUTPUT);                 // Client 14
    GPIO_SetMode(PORT_OUT7, PIN_OUT7, GPIO_PMD_OUTPUT);                 // Client 15
    GPIO_SetMode(PORT_OUT8, PIN_OUT8, GPIO_PMD_OUTPUT);                  // Client 16

    /**********************************INPUT*********************************/
    GPIO_SetMode(PORT_IN1, PIN_IN1, GPIO_PMD_OUTPUT);                   // Client 1
    GPIO_SetMode(PORT_IN2, PIN_IN2, GPIO_PMD_OUTPUT);                   // Client 2
    GPIO_SetMode(PORT_IN3, PIN_IN3, GPIO_PMD_OUTPUT);                   // Client 3
    GPIO_SetMode(PORT_IN4, PIN_IN4, GPIO_PMD_OUTPUT);                  // Client 4
    GPIO_SetMode(PORT_IN5, PIN_IN5, GPIO_PMD_OUTPUT);                   // Client 5
    GPIO_SetMode(PORT_IN6, PIN_IN6, GPIO_PMD_OUTPUT);                  // Client 6
    GPIO_SetMode(PORT_IN7, PIN_IN7, GPIO_PMD_OUTPUT);                  // Client 7
    GPIO_SetMode(PORT_IN8, PIN_IN8, GPIO_PMD_OUTPUT);                   // Client 8
}

/*--------------------------- LED Control Functions------------------------------------*/
void All_Led_Off(void)
{
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
    LED6 = 1;
    LED7 = 1;
    LED8 = 1;
    LED9 = 0;
    LED10 = 0;
    LED11 = 0;
    LED12 = 0;
    LED13 = 0;
    LED14 = 0;
    LED15 = 0;
    LED16 = 0;
}

void All_Led_ON(void)
{
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
    LED5 = 0;
    LED6 = 0;
    LED7 = 0;
    LED8 = 0;
    LED9 = 1;
    LED10 = 1;
    LED11 = 1;
    LED12 = 1;
    LED13 = 1;
    LED14 = 1;
    LED15 = 1;
    LED16 = 1;
}

void GPIO_Test(void)
{
    All_Led_Off();
    int i = 0 ;
    for (i = 0 ; i < 16 ; i++)
    {
        switch (i)
        {
        case 1 :
            LED1 = 0 ;
            break ;
        case 2 :
            LED2 = 0 ;
            break ;
        case 3 :
            LED3 = 0 ;
            break ;
        case 4 :
            LED4 = 0 ;
            break ;
        case 5 :
            LED5 = 0 ;
            break ;
        case 6 :
            LED6 = 0 ;
            break ;
        case 7 :
            LED7 = 0 ;
            break ;
        case 8 :
            LED8 = 0 ;
            break ;
        case 9 :
            LED9 = 1 ;
            break ;
        case 10 :
            LED10 = 1 ;
            break ;
        case 11 :
            LED11 = 1 ;
            break ;
        case 12 :
            LED12 = 1;
            break ;
        case 13 :
            LED13 = 1 ;
            break ;
        case 14 :
            LED14 = 1 ;
            break ;
        case 15 :
            LED15 = 1 ;
            break ;
        case 16 :
            LED16 = 1 ;
            break ;
        }
    }
}
