#ifndef FILE_IO_H
#define FILE_IO_H

/* Load 32-bit ARGB bitmap. Must free() returned memory. */
unsigned char *load_bmp(const char *path, int *out_width, int *out_height);

#endif