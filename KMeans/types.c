#include "types.h"
#include <stdlib.h>

StatusCode generate_pixel_centers(pixel_type **centers, int center_count)
{
    if (centers == NULL || center_count < 2) return InvalidParameters;
    *centers = malloc(center_count * sizeof(pixel_type));

    /* We'll just create centers evenly spaced along the middle line */
    int i;
    for(i = 0; i < center_count; ++i)
    {
        (*centers)[i].rgb.r = (*centers)[i].rgb.g = (*centers)[i].rgb.b =
            255 * ((double)i / (center_count - 1));
    }
    return OK;
}
