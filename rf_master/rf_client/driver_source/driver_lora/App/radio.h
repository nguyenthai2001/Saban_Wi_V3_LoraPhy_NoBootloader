#include <stdio.h>
#include "stdint.h"
#include "NUC200Series.h"
#include "SX1278.h"

/*!
 * SX1272 and SX1276 General parameters definition
 */
#define LORA 1
#define FSK 0

/*!
 * RF process function return codes
 */
typedef enum
{
    RF_IDLE,
    RF_BUSY,
    RF_RX_DONE,
    RF_RX_RUNNING,
    RF_RX_TIMEOUT,
    RF_TX_DONE,
    RF_TX_RUNNING,
    RF_TX_TIMEOUT,
    RF_LEN_ERROR,
    RF_CHANNEL_EMPTY,
    RF_CHANNEL_ACTIVITY_DETECTED,
} tRFProcessReturnCodes;

/*!
 * Radio driver structure defining the different function pointers
 */
typedef struct
{
    void (*Init)(void);
    void (*Reset)(void);
    void (*StartRx)(void);
    void (*GetRxPacket)(void *buffer, unsigned short int size_in);
    void (*SetTxPacket)(const void *buffer,  unsigned short int size_in);
    unsigned long int (*Process)(void);
} tRadioDriver;

/*!
 * \brief Initializes the RadioDriver structure with specific radio
 *        functions.
 *
 * \retval radioDriver Pointer to the radio driver variable
 */
tRadioDriver *RadioDriverInit(void);

