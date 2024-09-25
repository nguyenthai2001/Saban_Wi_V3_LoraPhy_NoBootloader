
#include "log.h"

void log_message(const char *format, ...)
{
#if MODBUS_ENABLE
#else
    char buffer[256]; 

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    printf("%s\n", buffer);
#endif
}

void printArray8Bit(uint8_t arr[], int size) 
{
#if MODBUS_ENABLE
#else
    printf("Array (decimal): ");
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);  
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("\n");

    printf("Array (hexadecimal): ");
    for (int i = 0; i < size; i++) {
        printf("%02X", arr[i]);  
        if (i < size - 1) {
            printf(" ");
        }
    }
    printf("\n");
#endif 
}

void print_uint8_array_as_chars(uint8_t arr[], size_t length) 
{
#if MODBUS_ENABLE
#else
    for (size_t i = 0; i < length; i++) 
	  {
        printf("%c", arr[i]);  
    }
#endif 
}