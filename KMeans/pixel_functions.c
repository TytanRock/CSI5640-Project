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
    size_t average_count = 0;
    double average_r, average_g, average_b;
    average_r = average_g = average_b = 0;
    size_t count;
    for (count = 0; count < num_objs; ++count)
    {
        /* If this item isn't part of the cluster we're calculating for, ignore it */
        if (cluster_list[count] != cluster) continue;
        /* This is part of the cluster, so add to the average value */
        /* This currently makes it impossible to accelerate this loop, maybe come up with a better way to add to an average? */
        average_r = (average_r * average_count) + pixel_list[count]->rgb.r;
        average_g = (average_g * average_count) + pixel_list[count]->rgb.g;
        average_b = (average_b * average_count) + pixel_list[count]->rgb.b;
        average_count ++;
        average_r /= average_count;
        average_g /= average_count;
        average_b /= average_count;
    }
    central_pixel->rgb.r = (average_r + 0.5); // Round to nearest integer
    central_pixel->rgb.g = (average_g + 0.5); // Round to nearest integer
    central_pixel->rgb.b = (average_b + 0.5); // Round to nearest integer
}
