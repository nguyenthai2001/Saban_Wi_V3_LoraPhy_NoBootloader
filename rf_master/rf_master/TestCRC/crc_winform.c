#include <stdint.h>
#include <stdio.h>

#define POLYNOMIAL 0xA001

uint16_t table[256];

void init_crc16_table() {
    uint16_t remainder;
    int i = 0 ;
    int bit = 0 ;
    for ( i = 0; i < 256; i++) {
        remainder = i;
        for ( bit = 0; bit < 8; bit++) {
            if (remainder & 1) {
                remainder = (remainder >> 1) ^ POLYNOMIAL;
            } else {
                remainder = (remainder >> 1);
            }
        }
        table[i] = remainder;
    }
}

// Hàm tính toán CRC-16
uint16_t compute_checksum(uint8_t *bytes, int length) {
    uint16_t crc = 0;
    int i = 0 ;
    for ( i = 0; i < length; i++) {
        uint8_t index = (uint8_t)(crc ^ bytes[i]);
        crc = (crc >> 8) ^ table[index];
    }
    return crc;
}

int main() {
    int i = 0;
    int b = 0 ;
    init_crc16_table();

    uint8_t data[64] = {0};  
    int length = 64;
     
    data[0] = 0x0c ;
    data[1] = 0x0b;
    for(i = 0 ; i< 59 ; i++)
    {
    	data[i+2] = 0 ;
	}
	
    printf("src data hex : " );
	for( b = 0 ;b < 61 ;b++)
	{
		printf("%2x" , data[b]);
	}
	printf(" \n src data dec : " );
	for( b = 0 ;b < 61 ;b++)
	{
		printf("%d " , data[b]);
	}

    // Tính toán CRC
    uint16_t crc = compute_checksum(data, 61);
    uint8_t high_byte = (crc >> 8) & 0xFF; 
    uint8_t low_byte = crc & 0xFF; 
	data[62] = high_byte ;
	data[63] = low_byte ;
    printf(" \n 64byte data hex : " );
	for( b = 0 ;b < 64 ;b++)
	{
		printf("%2x " , data[b]);
	}  
    printf(" \n 64byte data dec : " );
	for( b = 0 ;b < 64 ;b++)
	{
		printf("%d " , data[b]);
	}      
    printf(" \n CRC-16 hex : %2X\n ", crc);
    printf(" \n CRC-16 size  : %d\n ", sizeof(data));
    
    return 0;
}

