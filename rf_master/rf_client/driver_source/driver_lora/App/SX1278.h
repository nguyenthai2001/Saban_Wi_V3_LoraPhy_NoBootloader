#include <stdio.h>
#include "stdint.h"
#include "NUC200Series.h"

/*!
 * \brief SX1276 registers array
 */
extern unsigned char SX1276Regs[0x70];

uint8_t SX1276SetMode(uint8_t mode);

uint8_t SX1276GetMode(void);

/*!
 * \brief Enables LoRa modem or FSK modem
 *
 * \param [IN] opMode New operating mode
 */
void SX1276SetLoRaOn(uint8_t enable);

/*!
 * \brief Initializes the SX1276
 */
void SX1276Init(void);

/*!
 * \brief Resets the SX1276
 */
void SX1276Reset(void);

/*!
 * \brief Sets the SX1276 operating mode
 *
 * \param [IN] opMode New operating mode
 */
void SX1276SetOpMode(unsigned char opMode);

/*!
 * \brief Gets the SX1276 operating mode
 *
 * \retval opMode Current operating mode
 */
unsigned char SX1276GetOpMode(void);

/*!
 * \brief Reads the current Rx gain setting
 *
 * \retval rxGain Current gain setting
 */
unsigned char SX1276ReadRxGain(void);

/*!
 * \brief Trigs and reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1276ReadRssi(void);

/*!
 * \brief Gets the Rx gain value measured while receiving the packet
 *
 * \retval rxGainValue Current Rx gain value
 */
unsigned char SX1276GetPacketRxGain(void);

/*!
 * \brief Gets the SNR value measured while receiving the packet
 *
 * \retval snrValue Current SNR value in [dB]
 */
unsigned char SX1276GetPacketSnr(void);

/*!
 * \brief Gets the RSSI value measured while receiving the packet
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1276GetPacketRssi(void);

/*!
 * \brief Sets the radio in Rx mode. Waiting for a packet
 */
void SX1276StartRx(void);

/*!
 * \brief Gets a copy of the current received buffer
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size_in       Buffer size
 */
void SX1276GetRxPacket(void *buffer, unsigned short int size_in);

/*!
 * \brief Sets a copy of the buffer to be transmitted and starts the
 *        transmission
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size_in    Buffer size
 */
void SX1276SetTxPacket(const void *buffer, unsigned short int size_in);

/*!
 * \brief Gets the current RFState
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY,
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
unsigned char SX1276GetRFState(void);

/*!
 * \brief Sets the new state of the RF state machine
 *
 * \param [IN]: state New RF state machine state
 */
void SX1276SetRFState(unsigned char state);

/*!
 * \brief Process the Rx and Tx state machines depending on the
 *       SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY,
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
unsigned long int SX1276Process(void);

void SX1276StartCad(void) ;
