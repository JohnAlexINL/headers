// ------------     License & Usage
/*

Widgets Component
(C) 2026, Blood Rose Records

    #include "widgets.h"

    WIDGETS_H Names
        widget_*
        button_*

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

// ------------ Dependencies
#ifndef UI_H
    #error WIDGETS_H: need UI_H
#endif
#ifndef GFX_H
    #error WIDGETS_H: need GFX_H
#endif

// ------------ Typedefs

typedef     void                (*trigger_t) (event_t *);
typedef     struct              { pixel_t foreground; pixel_t background; pixel_t hover; trigger_t onclick; gfx_font_t * font; char *text; int x; int y; int w; int h; bool _hover; int _xoff; int _yoff; bool clicked; void *image; } button_t;

// ------------ Functions

void            widgets_renderer    (gfx_t * renderer);
void            button_draw         (button_t *button);
bool            button_test         (button_t *button, event_t *event);
button_t        button_new          (pixel_t foreground, pixel_t background, pixel_t hovercolor, trigger_t onclick, gfx_font_t *font, char *text, int x, int y, int w, int h);
int             widget_centerx      (int width);
int             widget_centery      (int height);

// ------------ Globals

#ifndef WIDGETS_H
#define WIDGETS_H

gfx_t *__widgets_renderer;

const pixel_t   black =             0xFF000000 ;
const pixel_t   white =             0xFFFFFFFF ;
const pixel_t   dark =              0xFF404040 ;
const pixel_t   gray =              0xFF808080 ;
const pixel_t   red =               0xFFFF0000 ;
const pixel_t   green =             0xFF00C000 ; // Green needs to be softened a little
const pixel_t   blue =              0xFF0000FF ;

// ------------ Implementation

void            widgets_renderer    (gfx_t * renderer)
{
    __widgets_renderer = renderer;
}

int             widget_centerx      (int width)
{
    return (__widgets_renderer->w - width) / 2;
}

int             widget_centery      (int height)
{
    return (__widgets_renderer->h - height) / 2;
}

button_t        button_new
    (pixel_t foreground, pixel_t background, pixel_t hovercolor, trigger_t onclick, gfx_font_t *font, char *text, int x, int y, int w, int h)
{
    button_t button = { 0 };
    button.foreground = foreground;
    button.background = background;
    button.hover = hovercolor;
    button.onclick = onclick;
    button.font = font;
    button.text = text;
    button.x = x;
    button.y = y;
    button.w = w;
    button.h = h;
    return button;
}

void            button_draw         (button_t *button)
{
    // One-time calculation for text centering
    if ( button->_xoff == 0 ) {
            int len = strlen(button->text);
            int width = button->font->px * len;
        button->_xoff = button->x + ((button->w - width)/2);
            int perline = ( width / button->font->px );
            int lines = len / perline;
            int height = lines * button->font->py;
        button->_yoff = button->y + ((button->h - height)/2);
    }
    
    // Print image if not NULL
    if ( button->image != NULL ) {
        gfx_stampraw(__widgets_renderer, button->image, button->w, button->h, button->x, button->y, 0, 0);
    }
    
    // Print background if not sentinel -1
    if ( button->background != -1 ) {
        gfx_color(button->background);
        if (button->_hover == true ) { gfx_color(button->hover); } 
        gfx_rect(__widgets_renderer, button->x, button->y, button->w, button->h);
    }

    // Print text if not sentinel NULL
    if ( button->text != NULL ) {
        gfx_color(button->foreground);
            gfx_font(button->font);
            gfx_text(__widgets_renderer, button->text, button->_xoff, button->_yoff, button->w, button->h);
    }
}

bool            button_test         (button_t *button, event_t *event)
{
    if ( event->type != EVENT_MOUSE ) { return button->_hover; }
        // Fail when outside the bounding box
    int withx = (event->x - button->x);
    int withy = (event->y - button->y);
    if ( withx < 0 || withy < 0 )                   { button->_hover = false; return button->_hover; }
    if ( withx > button->w || withy > button->h )   { button->_hover = false; return button->_hover; }
        // Redraw on hovers
    button->_hover = true; button_draw (button);
        // Check if clicked
    if ( event->c == (128 | 1) ) { button->clicked = true; } else { button->clicked = false; }
    if ( button->clicked == true ) { button->onclick(event); }
    return button->_hover;
}

#endif