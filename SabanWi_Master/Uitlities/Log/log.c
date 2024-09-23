
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