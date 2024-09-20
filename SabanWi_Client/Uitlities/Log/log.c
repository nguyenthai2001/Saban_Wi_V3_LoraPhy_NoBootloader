
#include "log.h"

// Hàm log nh?n chu?i d?nh d?ng và các d?i s? bi?n d?i
void log_message(const char *format, ...)
{
    // T?o m?t buffer d? ch?a chu?i log
    char buffer[256]; // Gi?i h?n chu?i log không quá 256 ký t?

    // Kh?i t?o danh sách d?i s? bi?n d?i
    va_list args;
    va_start(args, format);

    // G?p chu?i và d?i s? vào buffer
    vsnprintf(buffer, sizeof(buffer), format, args);

    // K?t thúc x? lý danh sách d?i s?
    va_end(args);

    // In ra chu?i dã g?p
    printf("%s\n", buffer);
}

void printArray8Bit(uint8_t arr[], int size) 
{
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
}

void print_uint8_array_as_chars(uint8_t arr[], size_t length) 
{
    for (size_t i = 0; i < length; i++) 
	  {
        printf("%c", arr[i]);  
    }
}