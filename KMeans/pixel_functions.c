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
    #pragma acc kernels
    for (i = 0; i < num_objs; ++i)
    {
        if (cluster_list[i] == cluster) count++;
    }

    /* malloc memory to keep track of averages */
    uint8_t *pixel_values = malloc(3 * sizeof(uint8_t) * count);
    int count_iter = 0;
    /* Copy over the pixels that matter */
    for (i = 0; i < num_objs; ++i)
    {
        if (cluster_list[i] == cluster)
        {
            pixel_values[(count_iter * 3) + 0] = pixel_list[i]->rgb.r;
            pixel_values[(count_iter * 3) + 1] = pixel_list[i]->rgb.g;
            pixel_values[(count_iter * 3) + 2] = pixel_list[i]->rgb.b;
            count_iter ++;
        }
    }
    count_iter = 0;

    //#pragma acc parallel loop copyin(pixel_values[0:count]) copy(average_r, average_g, average_b, count) reduction(+:average_r, average_g, average_b)
    #pragma acc kernels
    for (i = 0; i < count; ++i)
    {
        /* This is part of the cluster, so add to the average value */
        average_r += (double)pixel_values[(i * 3) + 0] / (double)count;
        average_g += (double)pixel_values[(i * 3) + 1] / (double)count;
        average_b += (double)pixel_values[(i * 3) + 2] / (double)count;
    }

    central_pixel->rgb.r = (average_r + 0.5); // Round to nearest integer
    central_pixel->rgb.g = (average_g + 0.5); // Round to nearest integer
    central_pixel->rgb.b = (average_b + 0.5); // Round to nearest integer
    free(pixel_values);
}
