
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
    for (int i = 0; i < size; i++)
    {
        printf("%d", arr[i]);
        if (i < size - 1)
        {
            printf(", ");
        }
    }
    printf("\n");

    printf("Array (hexadecimal): ");
    for (int i = 0; i < size; i++)
    {
        printf("%02X", arr[i]);
        if (i < size - 1)
        {
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

void covertUint16ToUint8(const uint16_t *src, uint8_t *dest, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        uint8_t lowByte = src[i] & 0xFF;
        uint8_t highByte = (src[i] >> 8) & 0xFF;

        dest[2 * i] = lowByte;
        dest[2 * i + 1] = highByte;
    }
}

size_t copyUint16ToUint8(const uint16_t *src, uint8_t *dest, size_t length_src, size_t length_dest)
{
    size_t destIndex = 0;
    int commaAdded = 0;

    for (size_t i = 0; i < length_src; i++)
    {
        uint8_t lowByte = src[i] & 0xFF;
        uint8_t highByte = (src[i] >> 8) & 0xFF;

        if (highByte == 0 && lowByte == 0)
        {
            if (!commaAdded && destIndex < length_dest)
            {
                dest[destIndex++] = 0x1D;
                commaAdded = 1;
            }
            continue;
        }

        if (highByte == 0 && lowByte != 0)
        {
            if (destIndex < length_dest)
            {
                dest[destIndex++] = lowByte;
            }
        }

        else if (highByte != 0 && lowByte != 0)
        {
            if (destIndex < length_dest)
            {
                dest[destIndex++] = lowByte;
            }
            if (destIndex < length_dest)
            {
                dest[destIndex++] = highByte;
            }
        }
    }

    return destIndex;
}


void splitArrayByMarker(const unsigned char *inputArray, int inputSize, unsigned char **beforeMarker, int *beforeSize, unsigned char **afterMarker, int *afterSize)
{
    int marker = 0x1D;
    *beforeSize = 0;
    *afterSize = 0;
    int i = 0 ;

    for (i = 0; i < inputSize; i++) 
	{
        if (inputArray[i] == marker) 
		{
            break;
        }
        (*beforeSize)++;
    }

    *beforeMarker = (unsigned char *)malloc(*beforeSize * sizeof(unsigned char));
    for ( i = 0; i < *beforeSize; i++) 
	{
        (*beforeMarker)[i] = inputArray[i];
    }

    *afterSize = inputSize - *beforeSize - 1; 
    *afterMarker = (unsigned char *)malloc(*afterSize * sizeof(unsigned char));
    for ( i = *beforeSize + 1; i < inputSize; i++) 
	{
        (*afterMarker)[i - (*beforeSize + 1)] = inputArray[i];
    }
}

void splitArrayByByte(const unsigned char *input, size_t size, unsigned char beforeArray[30], size_t *beforeSize, unsigned char afterArray[30], size_t *afterSize) 
{
    size_t i;

    *beforeSize = 0;
    *afterSize = 0;

    for (i = 0; i < size; i++) 
	  {
        if (input[i] == 0x1D) 
				{
            break;
        }
        if (*beforeSize < 30) 
				{  
            beforeArray[*beforeSize] = input[i];
            (*beforeSize)++;
        }
    }

    for (i++; i < size; i++) 
		{
        if (*afterSize < 30) 
				{  
            afterArray[*afterSize] = input[i];
            (*afterSize)++;
        }
    }
}