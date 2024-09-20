#ifndef _CRC_MODBUS_H_
#define _CRC_MODBUS_H_

#include <stdio.h>
#include "string.h"
#include "stdint.h"

uint32_t crc32_by_table(char *data, uint32_t size);
uint16_t crc16_by_table(char *data, uint32_t size);
uint16_t crc16(char *data, uint32_t size);
uint8_t crc8_by_table(char *data, uint32_t size);
uint8_t crc8(char *data, uint32_t size);
void init_crc16_table(void);
uint16_t compute_checksum(uint8_t *bytes, int length);
void CRC_Test (void);

#endif // _CRC_MODBUS_H_