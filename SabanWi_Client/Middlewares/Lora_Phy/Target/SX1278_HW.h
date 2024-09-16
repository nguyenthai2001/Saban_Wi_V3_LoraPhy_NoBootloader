#include <stdio.h>
#include  "stdint.h"
#include "NUC200Series.h"

#define Enable_Test 0      // 0 : test 1
// 1 : test 2

#define SabanWi_Version_1  1        // 0: version 1
// 1: version 2
#if SabanWi_Version_1

    #define DIO0_PORT PB
    #define DIO1_PORT PA
    #define DIO2_PORT PA
    #define DIO3_PORT PB
    #define DIO4_PORT PB
    #define DIO5_PORT PA
    #define RESET_PORT PB

    #define DIO0_PIN BIT2
    #define DIO1_PIN BIT15
    #define DIO2_PIN BIT14
    #define DIO3_PIN BIT0
    #define DIO4_PIN BIT1
    #define DIO5_PIN BIT5
    #define RESET_PIN BIT3

    #define DIO0 PB2
    #define DIO1 PA15
    #define DIO2 PA14
    #define DIO3 PB0
    #define DIO4 PB1
    #define DIO5 PA5
    #define RESET PB3

#else

    #define DIO0_PORT PB
    #define DIO1_PORT PB
    #define DIO2_PORT PB
    #define DIO3_PORT PD
    #define DIO4_PORT PD
    #define DIO5_PORT PA
    #define RESET_PORT PB

    #define DIO0_PIN BIT0
    #define DIO1_PIN BIT1
    #define DIO2_PIN BIT2
    #define DIO3_PIN BIT5
    #define DIO4_PIN BIT1
    #define DIO5_PIN BIT5
    #define RESET_PIN BIT3

    #define DIO0 PB0
    #define DIO1 PB1
    #define DIO2 PB2
    #define DIO3 PD5
    #define DIO4 PD1
    #define DIO5 PA5
    #define RESET PB3

#endif


typedef enum
{
    RADIO_RESET_OFF,
    RADIO_RESET_ON
} tRadioResetState ;

void SX1276InitIO(void);
void SX1276SetReset(uint8_t state);
uint8_t SX1276_GetDIO0(void);

void SPI_Command(uint8_t cmd);
uint8_t SPI_Readbyte(void);
void SX1276WriteBuffer(uint8_t addr, uint8_t *buf, uint8_t len);
void SX1276ReadBuffer(uint8_t addr, uint8_t *buf, uint8_t len);
void SX1276Write(uint8_t addr, uint8_t cmd);
void SX1276Read(uint8_t addr, uint8_t *cmd);
void SX1276WriteFifo(uint8_t *buf, uint8_t len);
void SX1276ReadFifo(uint8_t *buf, uint8_t len);


