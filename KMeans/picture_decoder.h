#pragma once
#include "status_codes.h"
#include "types.h"

StatusCode write_to_bitmap(char const *filename, uint8_t const *header, uint_least32_t header_size, uint8_t const *data, uint_least32_t data_size);

/**
 * \brief Read file as a bitmap and convert to 2d array of pixels
 *
 * Some notes:
 *  - BMP files are stored left-to-right, bottom-to-top. So the origin is 0,0.
 *  - Due to memory allocation, it was easiest to make arr's indices y-dimension first, then x.
 *    So to index arr, do arr[y][x], so the bottom right corner of a 255x255 image is arr[0][255]
 *
 * \param filename Name of file to read
 * \param arr pointer to 2d array. Will be malloc'd or NULL so caller must free
 * \param x_size pointer to size of x-dimension.
 * \param y_size pointer to size of y-dimension.
 * \param header_val pointer to byte array. Will be malloc'd or NULL so caller must free
 * \param header_size pointer to size of header
 * \return Status code of function
 */
StatusCode bitmap_to_multidimension_array(char const *filename, pixel_type ***arr, uint_least32_t *x_size, uint_least32_t *y_size, uint8_t **header_val, uint_least32_t *header_size);
