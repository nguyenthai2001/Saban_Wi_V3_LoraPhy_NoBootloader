
#include "gpio_driver.h"
#include "DataFlash_stack.h"

/**
 *    @brief        GPIO_Client_Init(void)
 *
 *    @param[in]    None
 *    @return       None

 *    @details      Input Output for Client
 */

void SB_Client_GPIO_Init(Saban_Client_Dataflash *client_pkg)
{
    int type_1[8] = {0};
    int j ;
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
    GPIO_SetMode(PORT_IN1, PIN_IN1, GPIO_PMD_INPUT);                   // Client 1
    GPIO_SetMode(PORT_IN2, PIN_IN2, GPIO_PMD_INPUT);                   // Client 2
    GPIO_SetMode(PORT_IN3, PIN_IN3, GPIO_PMD_INPUT);                   // Client 3
    GPIO_SetMode(PORT_IN4, PIN_IN4, GPIO_PMD_INPUT);                  // Client 4
    GPIO_SetMode(PORT_IN5, PIN_IN5, GPIO_PMD_INPUT);                   // Client 5
    GPIO_SetMode(PORT_IN6, PIN_IN6, GPIO_PMD_INPUT);                  // Client 6
    GPIO_SetMode(PORT_IN7, PIN_IN7, GPIO_PMD_INPUT);                  // Client 7
    GPIO_SetMode(PORT_IN8, PIN_IN8, GPIO_PMD_INPUT);                   // Client 8


    /**********************************INPUT*********************************/

    for (j = 0; j < 8 ; ++j)
    {
        if (client_pkg->input_type & (1 << j))
        {
            type_1[j] = j + 1 ;
        }
        else
        {
            type_1[j] = 0 ;
        }
    }
    if (type_1[0] == 1)
    {
        GPIO_EnableInt(PA, 6, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPAB_IRQn);
    }
    if (type_1[1] != 0)
    {
        GPIO_EnableInt(PC, 7, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPCDEF_IRQn);
    }
    if (type_1[2] != 0)
    {
        GPIO_EnableInt(PC, 6, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPCDEF_IRQn);
    }
    if (type_1[3] != 0)
    {
        GPIO_EnableInt(PB, 15, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPAB_IRQn);
    }
    if (type_1[4] != 0)
    {
        GPIO_EnableInt(PB, 8, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPAB_IRQn);
    }
    if (type_1[5] != 0)
    {
        GPIO_EnableInt(PA, 11, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPAB_IRQn);
    }
    if (type_1[6] != 0)
    {
        GPIO_EnableInt(PA, 10, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPAB_IRQn);
    }
    if (type_1[7] != 0)
    {
        GPIO_EnableInt(PA, 9, GPIO_INT_BOTH_EDGE);
        NVIC_EnableIRQ(GPAB_IRQn);
    }

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