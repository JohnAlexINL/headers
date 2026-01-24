// ------------     License & Usage
/*

Font-to-Glyph Component
(C) 2026, Blood Rose Records

    cc -o glyph glyph.c
    
        then invoke with

    glyph <font.ttf> <list of point-sizes>

        and it will generate

    ./fonts/*_<size>pt.bin

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

// ------------ Hard Dependencies

    // STB_IMAGE
#ifndef STBI_INCLUDE_STB_IMAGE_H
    #error STBI_INCLUDE_STB_IMAGE_H MISSING
#endif

    // STB_TRUETYPE
#ifndef STB_TRUETYPE_IMPLEMENTATION
    #error STB_TRUETYPE_IMPLEMENTATION
#endif

// ------------ Macros and Definitions

#define GLYPHS " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define glyph_alpha2argb(alpha) (uint32_t)((alpha<<24)|(alpha << 16)|(alpha << 8)|(alpha))

// ------------ Typedef

typedef struct { uint32_t *ptr; uint32_t width; uint32_t height; } glyph_bitmap;
struct stbtt_fontinfo; typedef struct stbtt_fontinfo stbtt_fontinfo;
typedef struct { stbtt_fontinfo font; char *data; size_t size; } glyph_font;
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
} glyph_bmp32h_t;

// ------------ Function Prototypes

    // ALLOC_H

void * alloc(size_t size);
void dealloc(void *);

    // ATOI_H

int atoi(const char *);

    // COPY_H

void * copy(void *, void *, size_t);

    // FILE_H

int file_size(char *filename);
int file_read(char *filename, char *buffer, int max);
int file_write(char *filename, char *buffer, int max);

    // STB_IMAGE

unsigned char * stbi_load(const char *filename, int *w, int *h, int *components, int requested_channels);
void stbi_image_free(void *data);

    // STB_TRUETYPE

int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset);
int stbtt_GetFontOffsetForIndex(const unsigned char *buffer, int index);
float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float pixels);
void stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap);
void stbtt_GetCodepointHMetrics(const stbtt_fontinfo *info, int codepoint, int *advanceWidth, int *leftSideBearing);
unsigned char *stbtt_GetCodepointBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int codepoint, int *width, int *height, int *xoff, int *yoff);

    // GLYPH_H

glyph_font *glyph_loadfont(char *filename); // allocates memory for the buffer, free when done
glyph_bitmap *glyph_generate(glyph_font *font, glyph_bitmap *dest, int pt); // allocates memory for the buffer, free when done
void glyph_write(glyph_bitmap *bmp, char *filename);
void glyph_putc(glyph_font *font, glyph_bitmap *dest, char self, int pt, int x, int y);
void glyph_freefont(glyph_font *font);
void glyph_freebitmap(glyph_bitmap *bmp);
int glyph_width(glyph_font *font, int pt);
int glyph_height(glyph_font *font, int pt);

// ------------ Implementations

#ifndef GLYPH_H
#define GLYPH_H

glyph_font *glyph_loadfont(char *filename)
{
        // Read the file into memory

    size_t filesize = file_size(filename);                          if ( filesize <= 0 ) { return NULL; }
    char * filebuffer = alloc(filesize);                            if ( !filebuffer ) { return NULL; }
    int read = file_read(filename, filebuffer, filesize);           if ( read <= 0 ) { return NULL; }

        // Get the font data

    stbtt_fontinfo font;
    int offset = stbtt_GetFontOffsetForIndex(filebuffer, 0);
    int init = stbtt_InitFont
        (&font, (const unsigned char *)(filebuffer), offset);       if ( !init )    { dealloc(filebuffer); return NULL; }

        // Populate the struct

    glyph_font *result = alloc(sizeof(glyph_font));                 if ( !result )  { dealloc(filebuffer); return NULL; }
        result->font = font;
        result->data = filebuffer;
        result->size = filesize;

    return result;
}

void glyph_freefont(glyph_font *font)
{
    dealloc(font->data);
    dealloc(font);
}

void glyph_freebitmap(glyph_bitmap *bmp)
{
    dealloc(bmp->ptr);
    dealloc(bmp);
}

int glyph_height(glyph_font *font, int pt)
{
    int px_height = pt * 96.0f / 72.0f;
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font->font, &ascent, &descent, &lineGap);
    float scale = stbtt_ScaleForPixelHeight(&font->font, px_height);
    int font_height = (int)((ascent - descent + lineGap) * scale);
    return font_height;
}

int glyph_width(glyph_font *font, int pt)
{
    int px_height = pt * 96.0f / 72.0f;
    int advance, lsb;
    stbtt_GetCodepointHMetrics(&font->font, ' ', &advance, &lsb);
    float scale = stbtt_ScaleForPixelHeight(&font->font, px_height);
    int font_width = (int)(advance * scale);
    return font_width;
}

glyph_bitmap *glyph_generate(glyph_font *font, glyph_bitmap *dest, int pt)
{
    const int glyph_count = sizeof(GLYPHS) / sizeof(GLYPHS[0]);
    int font_height = glyph_height(font, pt);
    int font_width = glyph_width(font, pt);

        // Create our bitmap
    glyph_bitmap *self = dest;
    self->width = font_width * glyph_count;
    self->height = font_height;
    uint32_t *data = alloc( sizeof(uint32_t) * (self->width * self->height) );
    self->ptr = data;

    int i; for(i=0; i<sizeof(GLYPHS); i++) {
        int x = font_width * i;
        glyph_putc(font, self, GLYPHS[i], pt, x, 0);
    }

    return self;
}

void glyph_putc(glyph_font *font, glyph_bitmap *dest, char self, int pt, int x, int y)
{
    int i, j;
        // Scale & Clear
    int height = glyph_height(font, pt);
    int width = glyph_width(font, pt);
    float scale = stbtt_ScaleForPixelHeight(&font->font, height);

        // Truetype Kerning
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font->font, &ascent, &descent, &lineGap);
    int baseline = (int)(ascent * scale);

        // Rasterization
    int w, h, xoff, yoff;
    unsigned char *raster = stbtt_GetCodepointBitmap(&font->font, scale, scale, (int)self, &w, &h, &xoff, &yoff);

        // Output to bitmap

    printf("char '%c': bbox w=%d h=%d xoff=%d yoff=%d, cx_range=(%d..%d) cy_range=(%d..%d)\n",
       self, w, h, xoff, yoff,
       x + xoff, x + xoff + w - 1,
       dest->height - 1 - (y + baseline + h + yoff - 1),
       dest->height - 1 - (y + baseline + yoff));

    for(j = 0; j < h; j++) {
    for(i = 0; i < w; i++) {
        unsigned char alpha = raster[j*w + i]; if(!alpha) { continue; }

        int cx = x + i + xoff;
        int cy = dest->height - 1 - (y + baseline + j + yoff);

        if(cx < 0 || cx >= (int)dest->width)  { alpha = 0; continue; }
        if(cy < 0 || cy >= (int)dest->height) { alpha = 0; continue; }

        dest->ptr[cy * dest->width + cx] = glyph_alpha2argb(alpha);
    }}
}

void glyph_write(glyph_bitmap *bmp, char *filename)
{
    // Prepare the raw buffer
    int size = sizeof(uint32_t) * ((bmp->width * bmp->height));
    char filebuffer[sizeof(glyph_bmp32h_t) + size];

    // Prepare our bitmap header
    glyph_bmp32h_t header = (glyph_bmp32h_t) {
        .magic = "BM",
        .filesize = size,
        .offset = sizeof(glyph_bmp32h_t),
        .headersize = 40,
        .width = bmp->width,
        .height = bmp->height,
        .planes = 1,
        .depth = 32,
        .compression = 0,
        .palette = 0,
    };

        // Prepare a string buffer for writing

    copy(filebuffer, &header, sizeof(glyph_bmp32h_t));
    char * body = (char *)(filebuffer + sizeof(glyph_bmp32h_t));
    copy(body, bmp->ptr, size);
    int write = file_write(filename, filebuffer, size); // TODO: errors
}



// ------------ Swappable Dependencies

    // ifndef ALLOC_H, bring GLIBC

#ifndef ALLOC_H
#define ALLOC_H

#include <stdio.h>
#include <stdlib.h>

    void * alloc(size_t size) { return calloc(size, sizeof(char)); }
    void dealloc(void * item) { free(item); }

#endif

    // ifndef ATOI_H bring GLIBC

#ifndef ATOI_H
#define ATOI_H

#include <stdio.h>
#include <stdlib.h>

#endif

    // ifndef COPY_H, bring GLIBC
#ifndef COPY_H
#define COPY_H

    void * copy(void * dest, void * src, size_t size) { return memcpy(dest, src, size); }

#endif

    // ifndef FILE_H, bring GLIBC

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

    int file_size(char *filename) { struct stat buffer; return (stat(filename, &buffer) == 0) ? buffer.st_size : -1; }

    int file_read(char *filename, char *buffer, int max) {
        FILE *fp = fopen(filename, "rb");
        if ( fp == NULL ) { return false; }
        int cursor = 0; char c; while(cursor < max -1) {
        c = fgetc(fp);
        if( feof(fp) ) { break; }
        buffer[cursor] = c; cursor++;
    } fclose(fp); return cursor;
    }

    int file_write (char *filename, char *buffer, int max) {
        FILE *fp = fopen(filename, "w+");
        if ( fp == NULL ) { return false; }
        int cursor; for (cursor = 0; cursor < max; cursor++) {
            if (fputc(buffer[cursor], fp) == EOF) { fclose(fp); return false; }
        } fclose(fp); return cursor;
    }

#endif
#endif