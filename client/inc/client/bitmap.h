/*****************************************************************************
 * File:        bitmap.h
 * Author:      ludvigsandberg
 * Date:        2026-07-13
 * Description: A8R8G8B8 BMP loader.
 *****************************************************************************/

#ifndef CLIENT_BITMAP_H
#define CLIENT_BITMAP_H

#include "common/types.h"

void bitmap_load(u8 **dst, int *width, int *height, const char *path);

#endif