#include "picture_decoder.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pixel_type **pixels;
    uint_least32_t x_size, y_size;

    StatusCode retval = bitmap_to_multidimension_array("../rgb.bmp", &pixels, &x_size, &y_size);

    if(retval != OK)
    {
        printf("An error occurred... %d\n", retval);
        return -1;
    }

    printf("X size: %d  Y size: %d\n", x_size, y_size);
    printf("Array values...\n");
    int x, y;
    for (y = 0; y < y_size; ++y)
    {
        for (x = 0; x < x_size; ++x)
        {
            printf("%x ", pixels[y][x].rgb.g);
        }
        printf("\n");
    }
    printf("Done!\n");

    free(pixels);
    return 0;
}
