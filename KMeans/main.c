#include "kmeans.h"
#include "picture_decoder.h"
#include "pixel_functions.h"
#include "stopWatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MUTE_PICTURE_OUTPUT

void print_usage(char * const *argv)
{
    fprintf(stderr, "usage: %s -f <bitmap filename>\n", argv[0]);
}

int main(int argc, char *argv[])
{
    pixel_type **pixels;
    uint_least32_t x_size, y_size, header_size;
    uint8_t *header_contents;

    int opt;
    char filename[100];
    filename[0] = '\0';
    while ((opt = getopt(argc, argv, "f:")) != -1)
    {
        switch(opt)
        {
            case 'f':
                strcpy(filename, optarg);
                break;
            default:
                print_usage(argv);
                break;
        }
    }

    if(filename[0] == '\0')
    {
        print_usage(argv);
        return -1;
    }

    startTimer();

    StatusCode retval = bitmap_to_multidimension_array(filename, &pixels, &x_size, &y_size, &header_contents, &header_size);

    if(retval != OK)
    {
        printf("An error occurred... %d\n", retval);
        return -1;
    }

    printf("X size: %d  Y size: %d\n", x_size, y_size);
    int x, y;
#ifndef MUTE_PICTURE_OUTPUT
    printf("Array values...\n");
    for (y = y_size - 1; y >= 0; --y)
    {
        for (x = 0; x < x_size; ++x)
        {
            printf("%02x ", pixels[y][x].rgb.g);
        }
        printf("\n");
    }
    printf("This is the g value of the bmp!\n");
#endif
    printf("Beginning kmeans work...\n");

    printf("Allocating memory for the flattened array\n");
    /* kmeans requires a flat array of objects, so flatten the picture */
    pixel_type *flat_array = malloc((x_size * y_size) * sizeof(pixel_type));
    /* And copy contents into the flat array */
    for (y = 0; y < y_size; ++y)
    {
        memcpy(&flat_array[y * x_size], pixels[y], x_size * sizeof(pixel_type));
    }
    void **pointer_arr = malloc((x_size * y_size) * sizeof(void *));
    for (x = 0; x < x_size * y_size; ++x)
    {
        pointer_arr[x] = &flat_array[x];
    }

    printf("Freeing memory from the initial bmp\n");
    /* Free the arrays inside pixels */
    for (y = 0; y < y_size; ++y)
    {
        free(pixels[y]);
    }
    /* Then free the major pixels */
    free(pixels);

    printf("Configuring kmeans cfg object\n");
    /* Come up with some centers to start with, I guess just full R, full G and full B for now... */
    pixel_type centers[12];
    centers[0].rgb.r = 255;
    centers[0].rgb.g = 0;
    centers[0].rgb.b = 0;
    centers[1].rgb.r = 0;
    centers[1].rgb.g = 255;
    centers[1].rgb.b = 0;
    centers[2].rgb.r = 0;
    centers[2].rgb.g = 0;
    centers[2].rgb.b = 255;
    centers[3].rgb.r = 128;
    centers[3].rgb.g = 128;
    centers[3].rgb.b = 128;
    centers[4].rgb.r = 0;
    centers[4].rgb.g = 0;
    centers[4].rgb.b = 0;
    centers[5].rgb.r = 255;
    centers[5].rgb.g = 255;
    centers[5].rgb.b = 255;
    centers[6].rgb.r = 128;
    centers[6].rgb.g = 0;
    centers[6].rgb.b = 0;
    centers[7].rgb.r = 180;
    centers[7].rgb.g = 0;
    centers[7].rgb.b = 0;
    centers[8].rgb.r = 100;
    centers[8].rgb.g = 0;
    centers[8].rgb.b = 0;
    centers[9].rgb.r = 50;
    centers[9].rgb.g = 0;
    centers[9].rgb.b = 0;
    centers[10].rgb.r = 75;
    centers[10].rgb.g = 0;
    centers[10].rgb.b = 0;
    centers[11].rgb.r = 25;
    centers[11].rgb.g = 0;
    centers[11].rgb.b = 0;
    void *center_pointers[12];
    center_pointers[0] = &centers[0];
    center_pointers[1] = &centers[1];
    center_pointers[2] = &centers[2];
    center_pointers[3] = &centers[3];
    center_pointers[4] = &centers[4];
    center_pointers[5] = &centers[5];
    center_pointers[6] = &centers[6];
    center_pointers[7] = &centers[7];
    center_pointers[8] = &centers[8];
    center_pointers[9] = &centers[9];
    center_pointers[10] = &centers[10];
    center_pointers[11] = &centers[11];

    /* Now set up the kmeans to do the clustering */
    kmeans_config cfg;
    cfg.distance_method = kmeans_pixel_distance;
    cfg.centroid_method = kmeans_pixel_centroid;
    cfg.objs = pointer_arr;
    cfg.num_objs = x_size * y_size;
    cfg.centers = center_pointers;
    cfg.k = 12; // We want to find the r,g,b centers for now I guess
    cfg.max_iterations = 0; // Library default
    cfg.clusters = malloc(cfg.num_objs * sizeof(int)); // I guess we have to allocate and free this?

    printf("Running kmeans\n");
    kmeans_result res = kmeans(&cfg);

    if (res != KMEANS_OK)
    {
        printf("An error occurred in the kmeans cluster finder: %d\n", res);
        return -2;
    }

#ifndef MUTE_PICTURE_OUTPUT
    printf("Kmeans finished, here's the values of the pixel clusters...\n");

    for (y = y_size - 1; y >= 0; --y)
    {
        for (x = 0; x < x_size; ++x)
        {
            printf("%02x ", cfg.clusters[(y * x_size) + x]);
        }
        printf("\n");
    }
#else
    printf("Kmeans finished\n");
#endif

    /* Reuse the flat array and write to it the value of each new pixel */

    int i;
    for (i = 0; i < x_size * y_size; ++i)
    {
        int cluster_value = cfg.clusters[i];
        flat_array[i] = *(pixel_type *)cfg.centers[cluster_value];
        //printf("%x", ((pixel_type *)cfg.centers[cluster_value])->rgb.g);
    }

    printf("Writing to new bitmap\n");
    StatusCode write_status = write_to_bitmap("newfile.bmp", header_contents, header_size, (uint8_t *)flat_array, (x_size * y_size) * sizeof(pixel_type));
    printf("Writing return was %d\n", write_status);

    printElapsedTime();

    return 0;
}
