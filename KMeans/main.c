#include "kmeans.h"
#include "picture_decoder.h"
#include "pixel_functions.h"
#include "stopWatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_usage(char * const *argv)
{
    fprintf(stderr, "usage: %s -f <bitmap filename> -c <center_count>\n", argv[0]);
    fprintf(stderr, " -f Filename of bitmap relative to where this binary is\n");
    fprintf(stderr, " -c Number of centers to generate a kmeans for. Must be at least 2 centers.\n");
}

int main(int argc, char *argv[])
{
    pixel_type *pixels;
    uint_least32_t x_size, y_size;

    int center_count = -1;

    int opt;
    char *filename = NULL;
    while ((opt = getopt(argc, argv, "f:c:")) != -1)
    {
        switch(opt)
        {
            case 'f':
                filename = malloc(strlen(optarg));
                strcpy(filename, optarg);
                break;
            case 'c':
                center_count = atoi(optarg);
                if (center_count < 2)
                {
                    fprintf(stderr, "Center count must be greater than 2!\n");
                    return -1;
                }
                break;
            default:
                print_usage(argv);
                break;
        }
    }

    if(filename == NULL || center_count == -1)
    {
        print_usage(argv);
        return -1;
    }

    printf("Reading file %s and generating %d centers\n", filename, center_count);

    startTimer();
    StatusCode retval = bitmap_to_multidimension_array(filename, &pixels, &x_size, &y_size);
    uint_least32_t total_pixels = x_size * y_size;

    if(retval != OK)
    {
        fprintf(stderr, "An error occurred... %d\n", retval);
        return -1;
    }

    void **pointer_arr = malloc(total_pixels * sizeof(void *));
    int i;
    for (i = 0; i < total_pixels; ++i)
    {
        pointer_arr[i] = &pixels[i];
    }

    /* Come up with some centers to start with, I guess just full R, full G and full B for now... */
    pixel_type *centers;
    generate_pixel_centers(&centers, center_count);
    void **center_array = malloc(center_count * sizeof(void *));
    for (i = 0; i < center_count; ++i)
    {
        center_array[i] = &centers[i];
    }

    /* Now set up the kmeans to do the clustering */
    kmeans_config cfg;
    cfg.distance_method = kmeans_pixel_distance;
    cfg.centroid_method = kmeans_pixel_centroid;
    cfg.objs = pointer_arr;
    cfg.num_objs = total_pixels;
    cfg.centers = center_array;
    cfg.k = center_count; // We want to find the r,g,b centers for now I guess
    cfg.max_iterations = 0; // Library default
    cfg.clusters = malloc(total_pixels * sizeof(int)); // I guess we have to allocate and free this?

    kmeans_result res = kmeans(&cfg);

    if (res != KMEANS_OK)
    {
        fprintf(stderr, "An error occurred in the kmeans cluster finder: %d\n", res);
        return -2;
    }

    /* Reuse the flat array and write to it the value of each new pixel */
    for (i = 0; i < total_pixels; ++i)
    {
        int cluster_value = cfg.clusters[i];
        pixels[i] = *(pixel_type *)cfg.centers[cluster_value];
    }

    StatusCode write_status = write_to_bitmap("newfile.bmp", (uint8_t *)pixels, total_pixels * sizeof(pixel_type), x_size, y_size);

    printElapsedTime();

    return 0;
}
