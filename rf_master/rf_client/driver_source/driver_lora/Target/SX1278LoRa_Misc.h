
#include "NUC200Series.h"

/*!
 * \brief Writes the new RF frequency value
 *
 * \param [IN] freq New RF frequency value in [Hz]
 */
void SX1276LoRaSetRFFrequency(unsigned long int freq);

/*!
 * \brief Reads the current RF frequency value
 *
 * \retval freq Current RF frequency value in [Hz]
 */
unsigned long int SX1276LoRaGetRFFrequency(void);

/*!
 * \brief Writes the new RF output power value
 *
 * \param [IN] power New output power value in [dBm]
 */
void SX1276LoRaSetRFPower(int8_t power);

/*!
 * \brief Reads the current RF output power value
 *
 * \retval power Current output power value in [dBm]
 */
char SX1276LoRaGetRFPower(void);

/*!
 * \brief Writes the new Signal Bandwidth value
 *
 * \remark This function sets the IF frequency according to the datasheet
 *
 * \param [IN] factor New Signal Bandwidth value [0: 125 kHz, 1: 250 kHz, 2: 500 kHz]
 */
void SX1276LoRaSetSignalBandwidth(unsigned char bw);

/*!
 * \brief Reads the current Signal Bandwidth value
 *
 * \retval factor Current Signal Bandwidth value [0: 125 kHz, 1: 250 kHz, 2: 500 kHz]
 */
unsigned char SX1276LoRaGetSignalBandwidth(void);

/*!
 * \brief Writes the new Spreading Factor value
 *
 * \param [IN] factor New Spreading Factor value [7, 8, 9, 10, 11, 12]
 */
void SX1276LoRaSetSpreadingFactor(unsigned char factor);

/*!
 * \brief Reads the current Spreading Factor value
 *
 * \retval factor Current Spreading Factor value [7, 8, 9, 10, 11, 12]
 */
unsigned char SX1276LoRaGetSpreadingFactor(void);

/*!
 * \brief Writes the new Error Coding value
 *
 * \param [IN] value New Error Coding value [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 */
void SX1276LoRaSetErrorCoding(unsigned char value);

/*!
 * \brief Reads the current Error Coding value
 *
 * \retval value Current Error Coding value [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 */
unsigned char SX1276LoRaGetErrorCoding(void);

/*!
 * \brief Enables/Disables the packet CRC generation
 *
 * \param [IN] enaable [TRUE, FALSE]
 */
void SX1276LoRaSetPacketCrcOn(unsigned char enable);

/*!
 * \brief Reads the current packet CRC generation status
 *
 * \retval enable [TRUE, FALSE]
 */
unsigned char SX1276LoRaGetPacketCrcOn(void);

/*!
 * \brief Enables/Disables the Implicit Header mode in LoRa
 *
 * \param [IN] enable [TRUE, FALSE]
 */
void SX1276LoRaSetImplicitHeaderOn(unsigned char enable);

/*!
 * \brief Check if implicit header mode in LoRa in enabled or disabled
 *
 * \retval enable [TRUE, FALSE]
 */
unsigned char SX1276LoRaGetImplicitHeaderOn(void);

/*!
 * \brief Enables/Disables Rx single instead of Rx continuous
 *
 * \param [IN] enable [TRUE, FALSE]
 */
void SX1276LoRaSetRxSingleOn(unsigned char enable);

/*!
 * \brief Check if LoRa is in Rx Single mode
 *
 * \retval enable [TRUE, FALSE]
 */
unsigned char SX1276LoRaGetRxSingleOn(void);

/*!
 * \brief Enables/Disables the frequency hopping
 *
 * \param [IN] enable [TRUE, FALSE]
 */

void SX1276LoRaSetFreqHopOn(unsigned char enable);

/*!
 * \brief Get the frequency hopping status
 *
 * \param [IN] enable [TRUE, FALSE]
 */
unsigned char SX1276LoRaGetFreqHopOn(void);

/*!
 * \brief Set symbol period between frequency hops
 *
 * \param [IN] value
 */
void SX1276LoRaSetHopPeriod(unsigned char value);

/*!
 * \brief Get symbol period between frequency hops
 *
 * \retval value symbol period between frequency hops
 */
unsigned char SX1276LoRaGetHopPeriod(void);

/*!
 * \brief Set timeout Tx packet (based on MCU timer, timeout between Tx Mode entry Tx Done IRQ)
 *
 * \param [IN] value timeout (ms)
 */
void SX1276LoRaSetTxPacketTimeout(unsigned long int value);

/*!
 * \brief Get timeout between Tx packet (based on MCU timer, timeout between Tx Mode entry Tx Done IRQ)
 *
 * \retval value timeout (ms)
 */
unsigned long int SX1276LoRaGetTxPacketTimeout(void);

/*!
 * \brief Set timeout Rx packet (based on MCU timer, timeout between Rx Mode entry and Rx Done IRQ)
 *
 * \param [IN] value timeout (ms)
 */
void SX1276LoRaSetRxPacketTimeout(unsigned long int value);

/*!
 * \brief Get timeout Rx packet (based on MCU timer, timeout between Rx Mode entry and Rx Done IRQ)
 *
 * \retval value timeout (ms)
 */
unsigned long int SX1276LoRaGetRxPacketTimeout(void);

/*!
 * \brief Set payload length
 *
 * \param [IN] value payload length
 */
void SX1276LoRaSetPayloadLength(unsigned char value);

/*!
 * \brief Get payload length
 *
 * \retval value payload length
 */
unsigned char SX1276LoRaGetPayloadLength(void);

/*!
 * \brief Enables/Disables the 20 dBm PA
 *
 * \param [IN] enable [TRUE, FALSE]
 */
void SX1276LoRaSetPa20dBm(unsigned char enale);

/*!
 * \brief Gets the current 20 dBm PA status
 *
 * \retval enable [TRUE, FALSE]
 */
unsigned char SX1276LoRaGetPa20dBm(void);

/*!
 * \brief Set the RF Output pin
 *
 * \param [IN] RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
 */
void SX1276LoRaSetPAOutput(unsigned char outputPin);

/*!
 * \brief Gets the used RF Ouptut pin
 *
 * \retval RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
 */
unsigned char SX1276LoRaGetPAOutput(void);

/*!
 * \brief Writes the new PA rise/fall time of ramp up/down value
 *
 * \param [IN] value New PaRamp value
 */
void SX1276LoRaSetPaRamp(unsigned char value);

/*!
 * \brief Reads the current PA rise/fall time of ramp up/down value
 *
 * \retval freq Current PaRamp value
 */
unsigned char SX1276LoRaGetPaRamp(void);

/*!
 * \brief Set Symbol Timeout based on symbol length
 *
 * \param [IN] value number of symbol
 */
void SX1276LoRaSetSymbTimeout(unsigned short int value);

/*!
 * \brief  Get Symbol Timeout based on symbol length
 *
 * \retval value number of symbol
 */
unsigned short int SX1276LoRaGetSymbTimeout(void);

/*!
 * \brief  Configure the device to optimize low datarate transfers
 *
 * \param [IN] enable Enables/Disables the low datarate optimization
 */
void SX1276LoRaSetLowDatarateOptimize(unsigned char enable);

/*!
 * \brief  Get the status of optimize low datarate transfers
 *
 * \retval LowDatarateOptimize enable or disable
 */
unsigned char SX1276LoRaGetLowDatarateOptimize(void);

/*!
 * \brief Get the preamble length
 *
 * \retval value preamble length
 */
unsigned short int SX1276LoRaGetPreambleLength(void);

/*!
 * \brief Set the preamble length
 *
 * \param [IN] value preamble length
 */
void SX1276LoRaSetPreambleLength(unsigned short int value);

/*!
 * \brief Set the number or rolling preamble symbol needed for detection
 *
 * \param [IN] value number of preamble symbol
 */
void SX1276LoRaSetNbTrigPeaks(unsigned char value);

/*!
 * \brief Get the number or rolling preamble symbol needed for detection
 *
 * \retval value number of preamble symbol
 */
unsigned char SX1276LoRaGetNbTrigPeaks(void);






