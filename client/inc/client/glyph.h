/*****************************************************************************
 * File:        glyph.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_GLYPH_H
#define CLIENT_GLYPH_H

#include "common/vec.h"

typedef struct {
    struct {
        int x;
        int y;
        int w; /* Glyph width (1 to 8 pixels). */
    } charset;

    struct {
        int x;
        int y;
        int w;
        int h;
    } screen;

    vec3_t color;
} glyph_t;

#endif
