#include "pixel_functions.h"
#include <math.h>
#include <stdio.h>

double rgb_distance_squared(pixel_type const *a, pixel_type const *b)
{
    double r_dist = pow(a->rgb.r - b->rgb.r, 2);
    double g_dist = pow(a->rgb.g - b->rgb.g, 2);
    double b_dist = pow(a->rgb.b - b->rgb.b, 2);
    double sum = r_dist + g_dist + b_dist;
    return sum;
}

void rgb_centroid(pixel_type const **pixel_list, int const *cluster_list, size_t num_objs, int cluster, pixel_type *central_pixel)
{
    double average_r, average_g, average_b;
    average_r = average_g = average_b = 0;
    size_t count = 0;
    size_t i = 0;

    /* Run through one time to get a count of how many pixels to count for the average */
    for (i = 0; i < num_objs; ++i)
    {
        if (cluster_list[i] == cluster) count++;
    }

    /* malloc memory to keep track of averages */
    double *averages = malloc(3 * sizeof(double) * count);
    const pixel_type **actual_pixels = malloc(sizeof(pixel_type *) * count);
    int count_iter = 0;

    /* Copy over the pixels that matter */
    for (i = 0; i < num_objs; ++i)
    {
        if (cluster_list[i] == cluster)
        {
            actual_pixels[count_iter++] = pixel_list[i];
        }
    }
    count_iter = 0;

    #pragma acc parallel loop present(averages[0:count], actual_pixels[0:count])
    for (i = 0; i < count; ++i)
    {
        /* This is part of the cluster, so add to the average value */
        averages[(3 * i) + 0] = (double)actual_pixels[i]->rgb.r / (double)count;
        averages[(3 * i) + 1] = (double)actual_pixels[i]->rgb.g / (double)count;
        averages[(3 * i) + 2] = (double)actual_pixels[i]->rgb.b / (double)count;
    }

    for (i = 0; i < count; ++i)
    {
        average_r += averages[(3 * i) + 0];
        average_g += averages[(3 * i) + 1];
        average_b += averages[(3 * i) + 2];
    }

    central_pixel->rgb.r = (average_r + 0.5); // Round to nearest integer
    central_pixel->rgb.g = (average_g + 0.5); // Round to nearest integer
    central_pixel->rgb.b = (average_b + 0.5); // Round to nearest integer

    free(averages);
    free(actual_pixels);
}
