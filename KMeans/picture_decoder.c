#include "status_codes.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

/**
 * \brief Read file as a bitmap and convert to 2d array of pixels
 *
 * \param filename Name of file to read
 * \param arr pointer to 2d array. Will be malloc'd or NULL so caller must free
 * \param x_size pointer to size of x-dimension.
 * \param y_size pointer to size of y-dimension.
 * \return Status code of function
 */
StatusCode bitmap_to_multidimension_array(char const *filename, pixel_type ***arr, uint_least32_t *x_size, uint_least32_t *y_size)
{
    /* Make sure none of the parameters are null */
    if (arr == NULL || x_size == NULL || y_size == NULL) return InvalidParameters;
    /* Initialize pointers to NULL so caller knows if they've been malloc'd or not */
    *arr = NULL;
    *x_size = NULL;
    *y_size = NULL;

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
     FILE *fp = fopen(filename, "rb");
     if (fp == NULL) return CouldNotOpenFile;

    /* Read file header at 14 bytes */
    uint8_t file_header_text[14];
    size_t read_size = fread(file_header_text, sizeof(uint8_t), sizeof(file_header_text) / sizeof(*file_header_text), fp);
    if (read_size != sizeof(file_header_text) / sizeof(*file_header_text)) return FileIsIncorrectLength;

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
            read_size = fread(bitmap_info_header, sizeof(uint8_t), sizeof(bitmap_info_header) / sizeof(*bitmap_info_header), fp);
            if (read_size != sizeof(bitmap_info_header) / sizeof(*bitmap_info_header)) return FileIsIncorrectLength;
            memcpy(x_size, &bitmap_info_header[4], 4);
            memcpy(y_size, &bitmap_info_header[8], 4);
        }   break;
        default:
            /* We can pretty trivially support OS/2, but I don't want to just yet */
            return FileFormatNotSupported;
    }

    /* We can do more checking on the file if we want, but I'm going straight to getting the pixel values */
    /* First seek to where the data actually starts */
    if (fseek(fp, pixel_array_offset, SEEK_SET) != 0) return FileLengthIsIncorrect;
    /* Now allocate memory for the bitmap */
    size_t pixels_to_read = (*x_size) * (*y_size);
    *arr = malloc(pixels_to_read);
    /* And copy the bitmap data over, we're just assuming 32-bit per pixel format for now */
    size_t data_read_size = fread(*arr, 4, pixels_to_read, fp);

    if (data_read_size != pixels_to_read) return FileLengthIsIncorrect;

    return OK;
}
