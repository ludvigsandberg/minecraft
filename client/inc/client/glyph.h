/*****************************************************************************
 * File:        glyph.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_GLYPH_H
#define CLIENT_GLYPH_H

typedef struct {
    struct {
        int x;
        int y;
        int w; // glyph width 1-8px
    } charset;

    struct {
        int x;
        int y;
        int w;
        int h;
    } screen;

    xvec3f32_t color;
} glyph_t;

#endif
