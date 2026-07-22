#include "client/file_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned char *load_bmp(const char *path, int *out_width, int *out_height) {
    FILE *f = NULL;
    unsigned char header[54];
    unsigned char *data   = NULL;
    unsigned char *pixels = NULL;
    int width;
    int height;
    int row_padding;
    int data_size;
    int y;
    int x;
    short bpp;
    int flip;
    int offset;

    assert(path);
    assert(out_width);
    assert(out_height);

    *out_width  = 0;
    *out_height = 0;

    f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    if (fread(header, 1, 54, f) != 54) {
        return NULL;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        return NULL;
    }

    width  = *(int *)&header[18];
    height = *(int *)&header[22];
    bpp    = *(short *)&header[28];

    if (bpp != 32) {
        return NULL;
    }

    if (width <= 0 || height == 0 || (bpp != 24 && bpp != 32)) {
        return NULL;
    }

    offset = *(int *)&header[10];

    row_padding = ((width * (bpp / 8) + 3) & ~3);
    data_size   = row_padding * (height > 0 ? height : -height);

    data = (unsigned char *)malloc((size_t)data_size);
    if (!data) {
        return NULL;
    }

    fseek(f, offset, SEEK_SET);
    if (fread(data, 1, (size_t)data_size, f) != (size_t)data_size) {
        return NULL;
    }

    fclose(f);
    f = NULL;

    pixels = (unsigned char *)malloc(
        (unsigned int)width * (unsigned int)(height > 0 ? height : -height) *
        4);

    *out_width  = width;
    *out_height = (height > 0 ? height : -height);

    flip = height > 0;

    for (y = 0; y < *out_height; y++) {
        int src_y              = flip ? (*out_height - 1 - y) : y;
        unsigned char *src_row = data + src_y * row_padding;
        unsigned char *dst_row = pixels + y * width * 4;

        for (x = 0; x < width; x++) {
            unsigned char *src  = src_row + x * (bpp / 8);
            unsigned char *dst2 = dst_row + x * 4;

            dst2[0] = src[2];
            dst2[1] = src[1];
            dst2[2] = src[0];
            dst2[3] = src[3];
        }
    }

    free(data);

    return pixels;
}
