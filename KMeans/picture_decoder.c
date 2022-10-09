#include "picture_decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    uint8_t file_header_text[14];
    size_t read_size = fread(file_header_text, 1, sizeof(file_header_text) / sizeof(*file_header_text), fp);
    if (read_size != sizeof(file_header_text) / sizeof(*file_header_text)) return FileLengthIsIncorrect;

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
    else if (memcmp(file_header_text, "BM", 2) == 0) { format = Windows;}
    else if (memcmp(file_header_text, "BA", 2) == 0) { format = StructBitmap; }
    else if (memcmp(file_header_text, "CI", 2) == 0) { format = StructColor; }
    else if (memcmp(file_header_text, "CP", 2) == 0) { format = ConstColorPointer; }
    else if (memcmp(file_header_text, "IC", 2) == 0) { format = StructIcon; }
    else if (memcmp(file_header_text, "PT", 2) == 0) { format = Pointer; }
    else
    {
        /* Invalid header */
        return FileFormatIsIncorrect;
    }

    /* Get bitmap size from header */
    uint32_t bitmap_size;
    memcpy(&bitmap_size, &file_header_text[2], sizeof(bitmap_size));
    /* Next 4 bytes are useless */
    /* Get the offset of where the pixel array is */
    uint32_t pixel_array_offset;
    memcpy(&pixel_array_offset, &file_header_text[10], sizeof(pixel_array_offset));

    /* Need to get the x and y dimensions from the bitmapinfoheader */
    switch (format)
    {
        case Windows:
        {
            /* Width is 4 bytes at offset 0x12, Height is 4 bytes at offset 0x16 */
            /* So get the next 12 bytes and we can find the height and width */
            uint8_t bitmap_info_header[12];
            read_size = fread(bitmap_info_header, 1, sizeof(bitmap_info_header) / sizeof(*bitmap_info_header), fp);
            if (read_size != sizeof(bitmap_info_header) / sizeof(*bitmap_info_header)) return FileLengthIsIncorrect;
            memcpy(x_size, &bitmap_info_header[4], 4);
            memcpy(y_size, &bitmap_info_header[8], 4);
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
