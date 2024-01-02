#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

static void swap_int32(int32_t* xp, int32_t* yp)
{
    int32_t temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubbleSort_int32(int32_t arr[], size_t n)
{
    for (size_t i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap_int32(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
}
