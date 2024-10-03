
#include "SX1278_HW.h"

void SX1276InitIO(void)
{
    /*SPI1 IO define */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 1000000);      //Configure as a master, clock idle low, 8-bit transaction ,SPI clock rate = 1 MHz  , SPI clock rate = 2MHz
    SPI_SET_DATA_WIDTH(SPI0, 8);                             // datawidth 8 bit
    SPI_SET_MSB_FIRST(SPI0);                                 // set msb firt
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);      // none
    SPI_DisableAutoSS(SPI0);                                 // none

    /*Init DIO define */
#if SabanWi_Version_1
    GPIO_SetMode(DIO0_PORT, DIO0_PIN, GPIO_PMD_INPUT);   // DIO0
#if Enable_Test
    GPIO_EnableInt(PB, 2, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPAB_IRQn);
#else
#endif
    GPIO_SetMode(DIO1_PORT, DIO1_PIN, GPIO_PMD_INPUT);   // DIO1
    GPIO_SetMode(DIO2_PORT, DIO2_PIN, GPIO_PMD_INPUT);   // DIO2
    GPIO_SetMode(DIO3_PORT, DIO3_PIN, GPIO_PMD_INPUT);   // DIO3
    GPIO_SetMode(DIO4_PORT, DIO4_PIN, GPIO_PMD_INPUT);   // DIO4
//      GPIO_SetMode(PB,BIT3,GPIO_PMD_INPUT);     // DIO5
    GPIO_SetMode(RESET_PORT, RESET_PIN, GPIO_PMD_OUTPUT);  // RESET

#else
    GPIO_SetMode(DIO0_PORT, DIO0_PIN, GPIO_PMD_INPUT);   // DIO0
    GPIO_EnableInt(PB, 0, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPAB_IRQn);

    GPIO_SetMode(DIO1_PORT, DIO1_PIN, GPIO_PMD_INPUT);   // DIO1
    GPIO_SetMode(DIO2_PORT, DIO2_PIN, GPIO_PMD_INPUT);   // DIO2
//      GPIO_SetMode(DIO3_PORT,DIO3_PIN,GPIO_PMD_INPUT);     // DIO3
//      GPIO_SetMode(DIO4_PORT,DIO4_PIN,GPIO_PMD_INPUT);     // DIO4
//      GPIO_SetMode(PB,BIT3,GPIO_PMD_INPUT);     // DIO5
    GPIO_SetMode(RESET_PORT, RESET_PIN, GPIO_PMD_OUTPUT);  // RESET

#endif
}

void SX1276SetReset(uint8_t state)
{
    if (state == RADIO_RESET_ON)
    {
        RESET = 0;
    }
    else
    {
        GPIO_SetMode(PB, BIT3, GPIO_PMD_INPUT);  // RESET
    }
}

uint8_t SX1276_GetDIO0(void)
{
    uint8_t status ;
    if (DIO0 == 0)
    {
        status = 0;
    }
    else
    {
        status = 1;
    }
    return status ;
}

void SPI_Command(uint8_t cmd)
{
    SPI0 ->SSR = 1;
    SPI_WRITE_TX0(SPI0, cmd);
    SPI_TRIGGER(SPI0);
    while (SPI_IS_BUSY(SPI0));
}

uint8_t SPI_Readbyte(void)
{
    uint8_t rxByte = 0;
    SPI0->SSR = 1;
    SPI_WRITE_TX0(SPI0, 0x00);
    SPI_TRIGGER(SPI0);
    while (SPI_IS_BUSY(SPI0));
    rxByte = SPI_READ_RX0(SPI0);
    return rxByte ;
}

void SX1276WriteBuffer(uint8_t addr, uint8_t *buf, uint8_t len)
{
    uint8_t i ;
    SPI0->SSR = 1;
    SPI_Command(addr | 0x80);
    for (i = 0 ; i < len ; i++)
    {
        SPI_Command(buf[i]);
    }
    SPI0->SSR = 0;
}

void SX1276ReadBuffer(uint8_t addr, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    SPI0->SSR = 1;
    SPI_Command(addr & 0x7F);
    for (i = 0; i < len; i++)
    {
        buf[i] = SPI_Readbyte();
    }
    SPI0->SSR = 0;
}

void SX1276Write(uint8_t addr, uint8_t cmd)
{
    SX1276WriteBuffer(addr, &cmd, 1);
}

void SX1276Read(uint8_t addr, uint8_t *cmd)
{
    SX1276ReadBuffer(addr, cmd, 1);
}

void SX1276WriteFifo(uint8_t *buf, uint8_t len)
{
    SX1276WriteBuffer(0, buf, len);
}

void SX1276ReadFifo(uint8_t *buf, uint8_t len)
{
    SX1276ReadBuffer(0, buf, len);
}
