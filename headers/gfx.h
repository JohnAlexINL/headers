// ------------     License & Usage
/*

Graphics Component
(C) 2026, Blood Rose Records

    #include "gfx.h"
    implements dotting, stamping, and other graphical functions

    GFX_H Names
        GFX_*

License:		Blood Rose 4 Clause License v1.0
Licensor:		Blood Rose Records <https://bloodrose.org>

Redistribution and use in source and binary forms,
with or without modification,
are permitted provided that the following conditions are met:

1. Redistribution of source code must retain 
the above copyright notice, this list of conditions, and the following disclaimer.

2. Redistribution in binary form must reproduce 
the above copyright notice, this list of conditions, and the following disclaimer
in the documentation and/or other materials provided with the distribution.

3. This license does not grant the right to sell the software.
"Sell" means practicing any or all of the rights granted by this license
to provide a product or service to third parties for a fee or other consideration
(including, without limitation, fees for hosting, consulting, or support services),
where the product or service derives substantially or wholly from the software.

4. Neither the name of the licensor nor the names of
the software's contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO event_t SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

*/

// ------------     Dependencies

// typedef struct ui_t;
// typedef pixel_t;

// ------------     Includes, Defines, and Macros

#define gfx_context(b, x, y) (gfx_t) { .buffer=b, .w=x, .h=y }

#ifndef PRIMITVES
    #define PRIMITVES
    #ifndef true
        typedef _Bool           bool;
        #define true            1
        #define false           0
    #endif
    #ifndef uint8_t
        typedef unsigned char   uint8_t;
        typedef unsigned short  uint16_t;
        typedef unsigned int    uint32_t;
        typedef unsigned long   uint64_t;
        typedef signed char     int8_t;
        typedef signed short    int16_t;
        typedef signed int      int32_t;
        typedef signed long     int64_t;
    #endif
#endif

#define GFX_ERR                 -1

// ------------     Type Definitions

typedef struct { pixel_t *buffer; int w; int h; } gfx_t;
typedef struct { gfx_t *source; int px; int py; } gfx_font_t;
typedef char gfx_rgb_t[3];
typedef struct __attribute__((packed)) {
    char        magic[2];
    uint32_t    filesize;
    uint16_t    reserved0;
    uint16_t    reserved1;
    uint32_t    offset;
    uint32_t    headersize;
    int32_t     width;
    int32_t     height;
    uint16_t    planes;
    uint16_t    depth;
    uint32_t    compression;
    uint32_t    imagesize; // allowed to be zero, or w*h*depth/bitsperbyte
    uint32_t    resolution[2];
    uint32_t    palette; // must be 0
    uint32_t    important; // ignored lmao    
} gfx_bmp_t;

// ------------     Function Prototypes

pixel_t     gfx_rgb             (int r, int g, int b);
pixel_t     gfx_rgba            (int r, int g, int b, int a);
pixel_t     gfx_mix             (pixel_t base, pixel_t adder);
void        gfx_color           (pixel_t color);
void        gfx_clear           (gfx_t *canvas);
    // Basic Shapes
void        gfx_dot             (gfx_t *canvas, int x, int y);
void        gfx_line            (gfx_t *canvas, int x, int y, int w, int h);
void        gfx_rect            (gfx_t *canvas, int x, int y, int w, int h);
void        gfx_rect_nofill     (gfx_t *canvas, int x, int y, int w, int h);
    // Text Rendering
void        gfx_font            (gfx_font_t *font);
gfx_font_t *gfx_fontmap         (gfx_font_t *font, gfx_t *canvas);
void        gfx_glyph           (gfx_t *canvas, int x, int y, char character);
void        gfx_text            (gfx_t *canvas, char * text, int x, int y, int w, int h);
    // Bitmaps
int         gfx_sizebmp         (char * source); // returns how many pixels in size this bitmap needs
void        gfx_loadbmp         (gfx_t *dest, char * source); // overwrites the canvas with data from the bitmap including w and h
    // Stamping Functions
pixel_t     gfx_sample          (gfx_t *canvas, int x, int y);
void        gfx_stampraw        (gfx_t *dest, gfx_t *source, int w, int h, int dx, int dy, int sx, int sy); // 1:1 scale stamps
void        gfx_stampcolor      (gfx_t *dest, gfx_t *source, int w, int h, int dx, int dy, int sx, int sy);
void        gfx_stamp           (gfx_t *dest, gfx_t *source, int x, int y);

// ------------     Global Variables

#ifndef GFX_H
#define GFX_H

#define GFX_GLYPHS " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

pixel_t gfx_global_color = 0;
gfx_font_t * gfx_global_font = NULL;

// ------------     Implementations

inline pixel_t gfx_rgb          (int r, int g, int b)
{
    return ((255) << 24) | ((r&255) << 16) | ((g&255) << 8) | (b&255);
}

inline pixel_t gfx_rgba         (int r, int g, int b, int a)
{
    return ((a&255) << 24) | ((r&255) << 16) | ((g&255) << 8) | (b&255);
}

void        gfx_color           (pixel_t color)
{
    gfx_global_color = color;
}

void        gfx_clear           (gfx_t *canvas)
{
    int i; for(i=0;i<canvas->w * canvas->h;i++) {
        canvas->buffer[i] = gfx_global_color;
    }
}

inline void gfx_dot             (gfx_t *canvas, int x, int y)
{
    if ( x >= canvas->w || y >= canvas->h ) { return; }
    unsigned int i = y * (canvas->w) + x;
    // pixel_t source = gfx_sample(canvas, x, y);
    // canvas->buffer[i] = gfx_mix(source, gfx_global_color);
    canvas->buffer[i] = gfx_global_color;
}

inline void gfx_line            (gfx_t *canvas, int x, int y, int w, int h)
{
        // Simplest cases: vertical and horizontal. Just simple iter on those cases
    if ( x == w ) { int i; for(i=y;i<h;i++) { gfx_dot(canvas, x, i); } return; }
    if ( y == h ) { int i; for(i=x;i<w;i++) { gfx_dot(canvas, i, y); } return; }
    
        // Then the other two cases, based on if delta of x or delta of y is larger
    int dx = (x<w) ? ( w-x ) : ( x-w );
    int dy = (y<h) ? ( h-y ) : ( y-h );

        // If the delta x is larger, then we iterate over x for values of y
    if ( dx > dy ) {
        int slope = ((dx <<1) / dy) -1;
        int cy=y; int cx=x; while(cx <= w) {
            gfx_line(canvas, cx, cy, cx+slope, cy);
            cy++; cx += slope;
        }   return;
    }

        // else if the delta y is larger, iterate over values of y instead
    int slope = ((dy <<1) / dx) -1;
    int cy=y; int cx=x; while(cy <= h) {
        gfx_line(canvas, cx, cy, cx, cy+slope);
        cx++; cy += slope;
    }   return;
}

void        gfx_rect            (gfx_t *canvas, int x, int y, int w, int h)
{
    int cy;
    for(cy=y; cy<y+h; cy++) {
        gfx_line(canvas, x, cy, x+w, cy);
    }
}

void        gfx_rect_nofill     (gfx_t *canvas, int x, int y, int w, int h)
{
    gfx_line(canvas, x,   y,   x,   y+h);   // LEFT
    gfx_line(canvas, x+w, y,   x+w, y+h);   // RIGHT
    gfx_line(canvas, x,   y,   x+w, y);     // TOP
    gfx_line(canvas, x,   y+h, x+w, y+h);   // BOTTOM
}

void        gfx_font            (gfx_font_t *font)
{
    gfx_global_font = font;
}

gfx_font_t *gfx_fontmap         (gfx_font_t *font, gfx_t *canvas)
{
    font->source = canvas;
    font->py = canvas->h;
    font->px = canvas->w / (96-1);
    return font;
}
void        gfx_glyph           (gfx_t *canvas, int x, int y, char character)
{
    if ( !gfx_global_font ) { printf("gfx_glyph cannot be called before gfx_font\n"); exit(-1); }
    gfx_font_t *font = gfx_global_font;
    if ( character < ' ' || character > '~' ) { return; } character -= ' ';
    int offset = character * gfx_global_font->px; 
    gfx_stampcolor(canvas, font->source, font->px, font->py, x, y, offset, 0);
}

void        gfx_text            (gfx_t *canvas, char * text, int x, int y, int w, int h)
{
    if ( !gfx_global_font ) { printf("gfx_text cannot be called before gfx_font\n"); exit(-1); }
    gfx_font_t * font = gfx_global_font;
    int i; 
    int perline = w/font->px; int maxline = h/font->py;
    int line = 0; int col = 0;
    for(i=0;i<0xFFF;i++) {
        if ( text[i] < ' ' || text[i] > '~' ) { return; }
        if ( col >= perline ) { col = 0; line++; }
        if ( line > maxline ) { return; }
        int xoff = col *    font->px;
        int yoff = line *   font->py;
        gfx_glyph(canvas, x+xoff, y+yoff, text[i]);
        col++;
    }
}

// Allow 32-bit, 24-bit, and 8-bit only
// Do not allow indexed color or other formats
int         gfx_sizebmp         (char * source)
{
    // BMP HEADER
    gfx_bmp_t * self = (gfx_bmp_t *)(source);
    if ( self->magic[0] != 'B' || self->magic[1] != 'M' )   { return GFX_ERR *1; }
    if ( self->offset < 40 )                                { return GFX_ERR *2; }
    switch(self->headersize) {
        case 40: case 56: case 124: break;
        default:                                            return GFX_ERR *3;
    }
    if ( self->width % 4 != 0 )                             { return GFX_ERR *4; }
    if ( self->planes != 1 )                                { return GFX_ERR *5; }
    if ( self->depth != 24 && self->depth != 32 )           { return GFX_ERR *6; }
    if ( self->compression != 0 && self->compression != 3 ) { return GFX_ERR *7; }
    if ( self->palette < 256 && self->palette != 0 )        { return GFX_ERR *8; }
    if ( self->width*self->height > self->filesize )        { return GFX_ERR *9; }
    return self->width * self->height;
}

void        gfx_loadbmp         (gfx_t *dest, char * source)
{
    pixel_t restore_color = gfx_global_color;
    gfx_bmp_t * self = (gfx_bmp_t *)(source);
    dest->w = self->width; dest->h = self->height;
    dest->buffer = malloc(sizeof(uint32_t) * self->width * self->height+1);
    int truesize = self->width * self->height;

    // Bitmaps are bottom-up, so we need to flip it upside down
    int x; int y;

    // RGBA Bitmaps
    if ( self->depth == 32 ) {
        uint32_t * datastream = (uint32_t *)(source + self->offset);
    for(y=0; y<self->height;y++) {
        int loadOffset = (self->width) * (self->height -y -1);
    for(x=0; x<self->width;x++) {
        int index = (y*self->width)+x;
        char *sauce; sauce = (char *)&datastream[index];
        int alpha = sauce[3];
        pixel_t color = gfx_rgba(sauce[2],sauce[1],sauce[0], alpha);
        gfx_color(color);
        gfx_dot(dest, x, self->height - y);
    }}}

    // RGB Bitmaps
    else {
    for(y=0; y<self->height;y++) {
        gfx_rgb_t * datastream = (gfx_rgb_t *)(source + self->offset);
        int loadOffset = (self->width) * (self->height -y -1);
    for(x=0; x<self->width;x++) {
        int index = (y*self->width)+x;
        char *sauce; sauce = (char *)&datastream[index];
        pixel_t color = gfx_rgb( sauce[2],sauce[1],sauce[0] );
        gfx_color(color);
        gfx_dot(dest, x, self->height - y);
    }}}
    gfx_color(restore_color);
}

inline pixel_t gfx_sample       (gfx_t *canvas, int x, int y)
{
    if (canvas->w < x )         { return 0; }
    if (canvas->h < y )         { return 0; }
    return ( canvas->buffer[
        (canvas->w * y) + x
    ]);
}

void        gfx_stampraw        (gfx_t *dest, gfx_t *source, int w, int h, int dx, int dy, int sx, int sy)
{
    pixel_t mask = gfx_sample(source, 1, 1);
    pixel_t lastcolor = gfx_global_color;
    if ( source->w < w + sx )   { return; }
    if ( source->h < h + sy )   { return; }
    int x; int y;
    for(y=1;y<h;y++) {
    for(x=1;x<w;x++) {
        pixel_t here = gfx_sample(source, sx+x, sy+y);
        if ( here == mask ) { continue; }
        gfx_color(here);
        gfx_dot(dest, dx+x, dy+y);
    }}
    gfx_color(lastcolor);
}

void        gfx_stampcolor      (gfx_t *dest, gfx_t *source, int w, int h, int dx, int dy, int sx, int sy)
{
    pixel_t mask = gfx_sample(source, 1, 1);
    pixel_t lastcolor = gfx_global_color;
    if ( source->w < w + sx )   { return; }
    if ( source->h < h + sy )   { return; }
    int x; int y;
    for(y=1;y<h;y++) {
    for(x=1;x<w;x++) {
        pixel_t here = gfx_sample(source, sx+x, sy+y);
        if ( here == mask ) { continue; }
        gfx_color(here & lastcolor);
        gfx_dot(dest, dx+x, dy+y);
    }}
    gfx_color(lastcolor);
}

inline void gfx_stamp           (gfx_t *dest, gfx_t *source, int x, int y)
{
    gfx_stampraw(dest, source, source->w, source->h, x, y, 0, 0);
}

#endif
