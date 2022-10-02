/**
 * File that defines the types and structs we're using
 */
#pragma once

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
