#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
    #define STBI_ONLY_PNG
    #define STBI_ONLY_BMP
#include "stb_image.h"
    #define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "glyph.h"
#include "license.h"

#define STRX(x)     #x
#define STR(x)      STRX(x)
#define MSG(msg)    puts("glyph, " msg);
#define ERROR(msg)  puts("glyph_error, " msg); return(ERR)
#define OK          0
#define ERR         -1
#define MAJOR       0
#define MINOR       0
#define PATCH       0
#define VERSION     STR(MAJOR) "." STR(MINOR) "." STR(PATCH)

int main (int argc, char **argv)
{
    if ( argc < 2 ) { ERROR("too few arguments, see --help for usage"); }
    if ( argv[1][0] == '-' )
    {
        char * cursor = argv[1]; while(*cursor == '-') { cursor++; }
        switch(*cursor) {
            default:    ERROR("invalid commandline argument");
            case 'h':   MSG("usage <font.ttf> <list of sizes>"); return OK;
            case 'v':   MSG(VERSION); return OK;
            case 'l':   puts(LICENSE); return OK;
        }
    }
    if ( argc < 3 ) { ERROR("too few arguments, see --help for usage"); }
    int sizes = argc-2;

    char * fontname = argv[1];

    glyph_font *font = glyph_loadfont(fontname);
    int size; for(size=0; size<sizes; size++) {
            // Prepare the bitmap
        int pt = atoi( argv[size +2] );
        int width = glyph_width(font, pt);
        int height = glyph_height(font, pt);
        glyph_bitmap bmp;
        bmp.ptr = alloc( sizeof(uint32_t) * width * height );

            // Invoke the bitmapper
        glyph_bitmap * result = glyph_generate(font, &bmp, pt);

            // Write the results to file
        char fname[128]; snprintf(fname, sizeof(fname), "fonts/%dpt.bmp", pt);
        printf("%s\n", fname);
        glyph_write(&bmp, fname);
        dealloc(bmp.ptr);
    }
}