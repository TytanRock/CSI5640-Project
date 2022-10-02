#pragma once

#include "types.h"
#include "kmeans.h"

double rgb_distance_squared(pixel_type const *a, pixel_type const *b);
void rgb_centroid(pixel_type const **pixel_list, int const *cluster_list, size_t num_objs, int cluster, pixel_type *central_pixel);

static inline double kmeans_pixel_distance(const Pointer a, const Pointer b)
{
    return rgb_distance_squared((pixel_type const *)a, (pixel_type const *)b);
}
static inline void kmeans_pixel_centroid(const Pointer * objs, const int * clusters, size_t num_objs, int cluster, Pointer centroid)
{
    rgb_centroid((pixel_type const **)objs, clusters, num_objs, cluster, (pixel_type *)centroid);
}
