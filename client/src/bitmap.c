/*****************************************************************************
 * File:        bitmap.c
 * Author:      ludvigsandberg
 * Date:        2026-07-13
 * Description: A8R8G8B8 BMP loader.
 *****************************************************************************/

#include "client/bitmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common/mem.h"

void bitmap_load(u8 **dst, int *out_width, int *out_height, const char *path) {
    FILE *f = NULL;
    u8 header[54];
    u8 *data   = NULL;
    u8 *pixels = NULL;
    int width;
    int height;
    int row_padding;
    int data_size;
    int y;
    int x;
    s16 bpp;
    int flip;
    int offset;

    assert(dst);
    assert(out_width);
    assert(out_height);
    assert(path);

    *out_width  = 0;
    *out_height = 0;

    f = fopen(path, "rb");
    if (!f) {
        printf("Failed to open \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    if (fread(header, 1, 54, f) != 54) {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    width  = *(int *)&header[18];
    height = *(int *)&header[22];
    bpp    = *(s16 *)&header[28];

    if (bpp != 32) {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    if (width <= 0 || height == 0 || (bpp != 24 && bpp != 32)) {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    offset = *(int *)&header[10];

    row_padding = ((width * (bpp / 8) + 3) & ~3);
    data_size   = row_padding * (height > 0 ? height : -height);

    data = (u8 *)checked_malloc((size_t)data_size);
    if (!data) {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    fseek(f, offset, SEEK_SET);
    if (fread(data, 1, (size_t)data_size, f) != (size_t)data_size) {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    fclose(f);
    f = NULL;

    pixels = (u8 *)checked_malloc((u32)width *
                                  (u32)(height > 0 ? height : -height) * 4);
    if (!pixels) {
        printf("File was invalid: \"%s\".\r\n", path);
        exit(EXIT_FAILURE);
    }

    *out_width  = width;
    *out_height = (height > 0 ? height : -height);

    flip = height > 0;

    for (y = 0; y < *out_height; y++) {
        int src_y   = flip ? (*out_height - 1 - y) : y;
        u8 *src_row = data + src_y * row_padding;
        u8 *dst_row = pixels + y * width * 4;

        for (x = 0; x < width; x++) {
            u8 *src  = src_row + x * (bpp / 8);
            u8 *dst2 = dst_row + x * 4;

            dst2[0] = src[2];
            dst2[1] = src[1];
            dst2[2] = src[0];
            dst2[3] = src[3];
        }
    }

    free(data);
    *dst = pixels;
}
