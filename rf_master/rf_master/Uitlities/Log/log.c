
#include "log.h"

void log_message(const char *format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    printf("%s\n", buffer);
}

void printArray8Bit(uint8_t arr[], int size)
{
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
}

void printStringBit(uint8_t arr[], int size)
{
    printf("Array (decimal): ");
    char string[size + 1];

    for (int i = 0; i < size; i++)
    {
        string[i] = arr[i];
        printf("%s", string);
    }
}

void printHexDecAscii(const unsigned char *array, int length)
{
    printf("Hex\tDec\tASCII\n");
    printf("------------------------\n");

    for (int i = 0; i < length; i++)
    {
        printf("%02X\t%3d\t%c\n", array[i], array[i], array[i]);
    }
}
