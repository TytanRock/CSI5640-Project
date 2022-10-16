#include "picture_decoder.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct __attribute__((packed)) _bmp_header_t {
    uint8_t header_field[2]; //!< Determines what kind of bmp file this is
    uint32_t file_size; //!< The overall size of the file in bytes
    uint8_t reserved[4]; //!< unused
    uint32_t offset; //!< How many bytes from 0 the image starts
} bmp_header_t;

typedef struct __attribute__((packed)) _windows_bitmapinfoheader_t {
    uint32_t header_size; //!< Always 40
    uint32_t width; //!< Width of the image in pixels
    uint32_t height; //!< Heigh of the image in pixels
    uint16_t color_planes; //!< Always 1
    uint16_t bits_per_pixel; //!< Only support 24
    uint32_t compression_method; //!< Only support BI_RGB (0)
    uint32_t image_size; //!< Size of image only in bytes
    int32_t horizontal_resolution; //!< Resolution in pixels per meter
    int32_t vertical_resolution; //!< Resolution in pixels per meter
    uint32_t colors_in_color_palette; // ignored
    uint32_t important_colors; // ignored
} windows_bitmapinfoheader_t;

/**
 * Helper function that will clean up a file pointer once it falls out of scope
 */
static void cleanup_file(FILE **fp)
{
    /* Can't clean up the file if it's never been allocated */
    if(*fp != NULL)
    {
        fclose(*fp);
    }
}

StatusCode write_to_bitmap(char const *filename, uint8_t const *data, uint_least32_t data_size, uint_least32_t width, uint_least32_t height)
{
    /* Get the header contents ready */
    bmp_header_t header;
    windows_bitmapinfoheader_t bitmap_info;

    header.header_field[0] ='B';
    header.header_field[1] = 'M';
    header.file_size = sizeof(bmp_header_t) + sizeof(windows_bitmapinfoheader_t) + data_size;
    header.offset = sizeof(bmp_header_t) + sizeof(windows_bitmapinfoheader_t);

    bitmap_info.header_size = 40;
    bitmap_info.width = width;
    bitmap_info.height = height;
    bitmap_info.color_planes = 1;
    bitmap_info.bits_per_pixel = 24;
    bitmap_info.compression_method = 0;
    bitmap_info.image_size = data_size;
    bitmap_info.horizontal_resolution = 0;
    bitmap_info.vertical_resolution = 0;
    bitmap_info.colors_in_color_palette = 0;
    bitmap_info.important_colors = 0;

    FILE *fp __attribute__((cleanup(cleanup_file)));
    printf("opening...\n");
    fp = fopen(filename, "wb");
    if (fp == NULL) return CouldNotOpenFile;
    printf("writing bitmap header\n");
    if (fwrite(&header, 1, sizeof(header), fp) != sizeof(header)) return CouldNotWriteToFile;
    printf("writing bitmap info\n");
    if (fwrite(&bitmap_info, 1, sizeof(bitmap_info), fp) != sizeof(bitmap_info)) return CouldNotWriteToFile;
    printf("writing data\n");

    int i;
    for(i = 0; i < height; ++i)
    {
        /* We need to account for the padding bytes when writing */
        unsigned int offset = width * i * sizeof(pixel_type);
        /* First write a row of data */
        if (fwrite(data + offset, 1, width * sizeof(pixel_type), fp) != width * sizeof(pixel_type)) return CouldNotWriteToFile;
        /* Then if there's padding, write the padding as 0's */
        int pad_size = 4 - (((width) * 3) % 4); // Find a better way to represent this
        if (pad_size == 4) pad_size = 0;

        /* Do some assertions to ensure I don't have a math bug */
        assert(pad_size >= 0);
        assert(pad_size < 4);

        /* And write the padding, the data doesn't matter so just repeat start of data I guess */
        if (fwrite(data, 1, pad_size, fp) != pad_size) return CouldNotWriteToFile;
    }
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
 * \return Status code of function
 */
StatusCode bitmap_to_multidimension_array(char const *filename, pixel_type ***arr, uint_least32_t *x_size, uint_least32_t *y_size)
{
    /* Make sure none of the parameters are null */
    if (arr == NULL || x_size == NULL || y_size == NULL) return InvalidParameters;
    /* Initialize values to default values so we know they're all properly set */
    *arr = NULL;
    *x_size = 0;
    *y_size = 0;

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

    // /* We can do more checking on the file if we want, but I'm going straight to getting the pixel values */
    // /* First seek to where the data actually starts */
    if (fseek(fp, pixel_array_offset, SEEK_SET) != 0) return FileLengthIsIncorrect;
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
        /* Every row must be on a 4-byte boundary, so we read move the cursor by the appropriate amount */
        int amount_to_move_by = 4 - (((*x_size) * 3) % 4); // Find a better way to represent this
        if (amount_to_move_by == 4) amount_to_move_by = 0;

        /* Do some assertions to ensure I don't have a math bug */
        assert(amount_to_move_by >= 0);
        assert(amount_to_move_by < 4);
        if(fseek(fp, amount_to_move_by, SEEK_CUR) != 0) return FileLengthIsIncorrect;
    }

    return OK;
}
