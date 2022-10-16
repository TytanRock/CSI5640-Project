/**
 * File that defines the types and structs we're using
 */
#pragma once

#include "status_codes.h"
#include <stdint.h>
#include <stddef.h>

typedef union _pixel_type{
    uint8_t data[3];
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } rgb;
} pixel_type;

/**
 * \brief Create an array of pixel_types to use for centers
 *
 * \param centers Pointer to pixel_type array. Will be malloc'd or NULL so caller must free
 * \param center_count Number of centers to allocate and generate. Cannot be less than 2.
 * \return StatusCode of function
 */
StatusCode generate_pixel_centers(pixel_type **centers, int center_count);
