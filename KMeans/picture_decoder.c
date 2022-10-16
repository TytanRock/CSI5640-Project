#include "picture_decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct __attribute__((packed)) _bmp_header_t {
    uint8_t header_field[2];
    uint32_t file_size;
    uint8_t reserved[4];
    uint32_t offset;
} bmp_header_t;

typedef struct __attribute__((packed)) _windows_bitmapinfoheader_t {
    uint32_t header_size; // Always 40
    uint32_t width;
    uint32_t height;
    uint16_t color_planes; // Always 1
    uint16_t bits_per_pixel; // Only support 24
    uint32_t compression_method; // Only support BI_RGB (0)
    uint32_t image_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t colors_in_color_palette; // ignored
    uint32_t important_colors; // ignored
} windows_bitmapinfoheader_t;

/**
 * Helper function that will clean up a file pointer once it falls out of scope
 */
static void cleanup_file(FILE **fp)
{
    fclose(*fp);
}

StatusCode write_to_bitmap(char const *filename, uint8_t const *header, uint_least32_t header_size, uint8_t const *data, uint_least32_t data_size)
{
    FILE *fp __attribute__((cleanup(cleanup_file)));
    printf("opening...\n");
    fp = fopen(filename, "wb");
    if (fp == NULL) return CouldNotOpenFile;
    printf("writing 1\n");
    if (fwrite(header, 1, header_size, fp) != header_size) return CouldNotWriteToFile;
    printf("writing 2\n");
    if (fwrite(data, 1, data_size, fp) != data_size) return CouldNotWriteToFile;
    printf("finished\n");
    return OK;
}

/**
 * \brief Read file as a bitmap and convert to 2d array of pixels
 *
 * \param filename Name of file to read
 * \param arr pointer to 2d array. Will be malloc'd or NULL so caller must free
 * \param x_size pointer to size of x-dimension.
 * \param y_size pointer to size of y-dimension.
 * \param header_val pointer to byte array. Will be malloc'd or NULL so caller must free
 * \param header_size pointer to size of header
 * \return Status code of function
 */
StatusCode bitmap_to_multidimension_array(char const *filename, pixel_type ***arr, uint_least32_t *x_size, uint_least32_t *y_size, uint8_t **header_val, uint_least32_t *header_size)
{
    /* Make sure none of the parameters are null */
    if (arr == NULL || x_size == NULL || y_size == NULL || header_val == NULL || header_size == NULL) return InvalidParameters;
    /* Initialize values to default values so we know they're all properly set */
    *arr = NULL;
    *header_val = NULL;
    *x_size = 0;
    *y_size = 0;
    *header_size = 0;

    /**
     * Bitmap file has the following contents:
     *  - File header (14 bytes)
     *  - DIB header (fixed-size with 7 variations)
     *  - Extra bit masks (optional) (12 or 16 bytes)
     *  - Color table (semi-optional) (variable size)
     *  - Gap1 (optional) (variable size)
     *  - Pixel array (variable size)
     *  - Gap2 (optional) (variable size)
     *  - ICC color profile (optional) (variable size)
     */
     FILE *fp __attribute__((cleanup(cleanup_file)));
     fp = fopen(filename, "rb");
     if (fp == NULL) return CouldNotOpenFile;

    /* Read file header at 14 bytes */
    bmp_header_t header;
    size_t read_size = fread(&header, 1, sizeof(header), fp);
    if (read_size != sizeof(header)) return FileLengthIsIncorrect;

    /* And verify header matches expectations */
    enum bitmap_format {
        Windows,
        StructBitmap,
        StructColor,
        ConstColorPointer,
        StructIcon,
        Pointer
    } format;
    if (0) {}
    else if (memcmp(header.header_field, "BM", 2) == 0) { format = Windows;}
    else if (memcmp(header.header_field, "BA", 2) == 0) { format = StructBitmap; }
    else if (memcmp(header.header_field, "CI", 2) == 0) { format = StructColor; }
    else if (memcmp(header.header_field, "CP", 2) == 0) { format = ConstColorPointer; }
    else if (memcmp(header.header_field, "IC", 2) == 0) { format = StructIcon; }
    else if (memcmp(header.header_field, "PT", 2) == 0) { format = Pointer; }
    else
    {
        /* Invalid header */
        return FileFormatIsIncorrect;
    }

    /* Get the offset of where the pixel array is */
    uint32_t pixel_array_offset = header.offset;

    /* Need to get the x and y dimensions from the bitmapinfoheader */
    windows_bitmapinfoheader_t bitmap_info_header;
    switch (format)
    {
        case Windows:
        {
            /* Width is 4 bytes at offset 0x12, Height is 4 bytes at offset 0x16 */
            /* So get the next 12 bytes and we can find the height and width */
            read_size = fread(&bitmap_info_header, 1, sizeof(bitmap_info_header), fp);
            if (read_size != sizeof(bitmap_info_header)) return FileLengthIsIncorrect;
            *x_size = bitmap_info_header.width;
            *y_size = bitmap_info_header.height;
        }   break;
        default:
            /* We can pretty trivially support OS/2, but I don't want to just yet */
            return FileFormatNotSupported;
    }

    /* We can do more checking, but instead we'll just copy this header over, since it will be basically identical anyway */
    if(fseek(fp, 0, SEEK_SET)) return FileLengthIsIncorrect;
    *header_val = malloc(pixel_array_offset);
    *header_size = pixel_array_offset;
    fread(*header_val, 1, pixel_array_offset, fp);

    // /* We can do more checking on the file if we want, but I'm going straight to getting the pixel values */
    // /* First seek to where the data actually starts */
    // if (fseek(fp, pixel_array_offset, SEEK_SET) != 0) return FileLengthIsIncorrect;
    /* Now allocate memory for the bitmap */

    /* Allocate memory for all the pointers first, one for each pixel across y */
    *arr = malloc(*y_size * sizeof(pixel_type *));
    /* And allocate memory for each pixel in that row */
    int y;
    for(y = 0; y < *y_size; ++y)
    {
        (*arr)[y] = malloc(*x_size * sizeof(pixel_type));
        /* And go ahead and read it out from the file since we're here */
        size_t data_read_size = fread((*arr)[y], 3, *x_size, fp);
        if (data_read_size != *x_size) return FileLengthIsIncorrect;
    }

    return OK;
}
