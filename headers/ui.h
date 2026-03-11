// ------------     License & Usage
/*

UI Component
(C) 2026, Blood Rose Records

    #include "ui.h"
    then assign `ui_handler` to a function `(void)(event_t *)`
    to create the main interface loop.
    use stamping functions and dotting for rendering

    UI_H Names
        window_*
        ui_*
        event_*
        pixel_*

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

#include <SDL2/SDL.h>
#include <time.h>

// ------------     Includes, Defines, and Macros

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

// ------------     Types Definitions

typedef struct { void *source; int type; int x; int y; int c; } event_t;

typedef     void                (*ui_handler_t) (event_t *);
typedef     unsigned int        pixel_t;
typedef     SDL_Window          ui_window_t;
typedef     SDL_Renderer        ui_renderer_t;
typedef     SDL_Texture         ui_texture_t;

typedef struct { int w, h, maxw, maxh; pixel_t *pixels; ui_window_t *window; ui_renderer_t *renderer; ui_texture_t *texture; ui_handler_t handler; } ui_t;

// ------------     Function Prototypes

bool        ui_init             (ui_t *window, pixel_t * pixelbuffer, int w, int h, bool fullscreen);
void        ui_icon             (ui_t *window, pixel_t * data, int w, int h);
void        ui_title            (ui_t *window, const char * title);
void        ui_size             (ui_t *window, int *w, int *h);
void        ui_destroy          (ui_t *window);
void        ui_quit             (ui_t *window);
void        ui_present          (ui_t *window);
event_t     ui_poll             (ui_t *window);
event_t     ui_eventloop        (ui_t *window);
int         ui_timer            (double seconds);
void        ui_wait             (double seconds);

void        event_print         (event_t *event);

// ------------     Global Variables

#ifndef UI_H
#define UI_H

char event_filename[255];

enum {
    EVENT_NONE, EVENT_QUIT,
    EVENT_DROP, EVENT_WINDOW, EVENT_AUDIO,
    EVENT_KEYS, EVENT_MOUSE, EVENT_SCROLL, 
};

enum
{ // https://wiki.libsdl.org/SDL2/SDL_WindowEventID
    EVENT_WNONE = SDL_WINDOWEVENT_NONE,
    EVENT_WSHOWN = SDL_WINDOWEVENT_SHOWN,
    EVENT_WHIDDEN = SDL_WINDOWEVENT_HIDDEN,
    EVENT_WMOVED = SDL_WINDOWEVENT_MOVED,
    EVENT_WRESIZED = SDL_WINDOWEVENT_RESIZED, // SDL_WINDOWEVENT_SIZE_CHANGED should also be handled
    EVENT_WMINIMIZE = SDL_WINDOWEVENT_MINIMIZED,
    EVENT_WMAXIMIZE = SDL_WINDOWEVENT_MAXIMIZED,
    EVENT_WRESTORED = SDL_WINDOWEVENT_RESTORED
};

const char event_names[][10] = {
    [EVENT_NONE] =      "NONE", 
    [EVENT_QUIT] =      "QUIT",     // SDL_QuitEvent
    [EVENT_WINDOW] =    "WINDOW",   // SDL_DisplayEvent, SDL_WindowEvent
    [EVENT_AUDIO] =     "AUDIO",    // SDL_AudioDeviceEvent
    [EVENT_DROP] =      "DROP",     // SDL_DropEvent, drag & drop
    [EVENT_KEYS] =      "KEYBOARD", // SDL_KeyboardEvent
    [EVENT_MOUSE] =     "MOUSE",    // SDL_MouseMotionEvent, SDL_MouseButtonEvent, 
    [EVENT_SCROLL] =    "SCROLL",   // SDL_MouseWheelEvent
};

const char event_wnames[][10] = {
    [EVENT_WNONE] =     "NONE",
    [EVENT_WSHOWN] =    "SHOWN",
    [EVENT_WHIDDEN] =   "HIDDEN",
    [EVENT_WMOVED] =    "MOVED",
    [EVENT_WRESIZED] =  "RESIZED",
    [EVENT_WMINIMIZE] = "MINIMIZE",
    [EVENT_WMAXIMIZE] = "MAXIMIZE",
    [EVENT_WRESTORED] = "RESTORED"
};

// ------------     Implementations

void ui_wait (double seconds) 
{
    uint32_t mil = (uint32_t)((double)(seconds * mil));
    SDL_Delay(mil);
}

int ui_timer(double seconds) 
{
    static clock_t timer_last = 0;
    clock_t now = clock();
    double elapsed = (double)(now - timer_last) / CLOCKS_PER_SEC;
    if ( elapsed >= seconds || timer_last == 0 ) {
        timer_last = now; return true;
    }   return false;
}

bool        ui_init                 (ui_t *window, pixel_t * pixelbuffer, int w, int h, bool fullscreen)
{
    window->w = w; window->maxw = w;
    window->h = h; window->maxh = h;
    window->handler = NULL;

    window->pixels = pixelbuffer;
    if (!window->pixels) { return -1; }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { return -1; }

    window->window = SDL_CreateWindow(
        "UI_H",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h,
        SDL_WINDOW_SHOWN | ( fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 )
    );
    if (!window->window)
        return -1;

    window->renderer = SDL_CreateRenderer(window->window, -1, 0);
    if (!window->renderer)
        return -1;

    window->texture = SDL_CreateTexture(
        window->renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        w, h
    );
    if (!window->texture)
        return -1;

    return 0;
}

void        ui_icon             (ui_t *window, pixel_t * pixels, int w, int h)
{
    // Cast pixel data to a Surface, then use that to set the icon
    // This feature apparently just does not work on Wayland
    // It also doesn't work on Mac, but that's like kinda expected
    const int bitdepth =        32;
    const int bytesPerRow =     w * 4;
    // SDL_Surface *icon = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, bitdepth, bytesPerRow, SDL_PIXELFORMAT_ARGB8888);
    
    SDL_Surface *icon = SDL_CreateRGBSurfaceWithFormat(
        0, w, h, 32, SDL_PIXELFORMAT_ARGB8888 // empty, will copy over
    );  if (!icon) { puts("Failed to map icon"); return; }

    memcpy(icon->pixels, pixels, w * h * 4);
    SDL_SetWindowIcon(window->window, icon);
    // SDL_FreeSurface(icon);
}

void        ui_title            (ui_t *window, const char * title)
{
    SDL_SetWindowTitle(window->window, title);
}

void ui_size(ui_t *window, int *w, int *h)
{
    SDL_GetWindowSize(window->window, w, h);
}

void ui_destroy(ui_t *window)
{
    if (window->texture)    { SDL_DestroyTexture  (window->texture);  }
    if (window->renderer)   { SDL_DestroyRenderer (window->renderer); }
    if (window->window)     { SDL_DestroyWindow   (window->window);   }
}

void ui_quit(ui_t *window)
{
    ui_destroy(window);
    SDL_Quit();
    exit(0);
}

void ui_present(ui_t *window)
{
    SDL_UpdateTexture(window->texture, NULL, window->pixels, window->w * sizeof(unsigned int));
    SDL_RenderCopy(window->renderer, window->texture, NULL, NULL); // copy texture first
    SDL_RenderPresent(window->renderer);                       // then present
}

void window_clear(ui_t *window, pixel_t color)
{
    for (int i = 0; i < window->w * window->h; ++i)
        window->pixels[i] = color;
}

event_t ui_poll(ui_t *window) 
{
    SDL_Event e; // https://wiki.libsdl.org/SDL2/SDL_Event
    SDL_PollEvent(&e);
    switch(e.type) {
        default: return (event_t) { .type = EVENT_NONE };
// -----------------    Quit Events
        case SDL_QUIT: {
            return (event_t) { .type = EVENT_QUIT };
        }
// -----------------    Keyboard Events
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            // <Alt+Enter> Toggle Fullscreen
            if (e.key.keysym.sym == SDLK_RETURN && (e.key.keysym.mod & KMOD_ALT) && e.key.state == SDL_PRESSED )
            {
                Uint32 flags = SDL_GetWindowFlags(window->window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) { SDL_SetWindowFullscreen(window->window, 0); }
                else { SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN_DESKTOP); }
                break;
            }
            // Otherwise, process the keystroke and return it
            else {
                return (event_t) { .type = EVENT_KEYS, .x = (e.key.state == SDL_PRESSED ), .c = e.key.keysym.sym, };
            }
        }
// -----------------    Mouse Events
        case SDL_MOUSEMOTION: {
            uint16_t my = e.motion.y; 
            uint16_t mx = e.motion.x;
            return (event_t) { .type = EVENT_MOUSE, .x = mx, .y = my, .c = false };
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            uint16_t my = e.button.y;
            uint16_t mx = e.button.x;
            uint16_t c  = e.button.button;
            if ( e.button.state == SDL_RELEASED ) { c = c | 128;  }
            return (event_t) { .type = EVENT_MOUSE, .x = mx, .y = my, .c = c };
        }
        case SDL_MOUSEWHEEL: {
            uint16_t vector = e.wheel.x + e.wheel.y; // distance moved
            uint16_t my = e.wheel.mouseY;
            uint16_t mx = e.wheel.mouseX;
            return (event_t) { .type = EVENT_SCROLL, .x = my, .y = my, .c = vector };
        }

// -----------------    Windowing Events
        case SDL_WINDOWEVENT: {
            // Fetching the WindowID should be redundant
            // since this function is returning event_t into a window-dependent callback
            uint16_t data1 = e.window.data1;
            uint16_t data2 = e.window.data2;
            uint16_t subtype = e.window.event;
            return (event_t) { .type = EVENT_WINDOW, .x = data1, .y = data2, .c = subtype };
        }

// -----------------    Audio Device Events
        case SDL_AUDIODEVICEADDED: case SDL_AUDIODEVICEREMOVED: {
            bool connected =        ( e.adevice.type == SDL_AUDIODEVICEADDED);
            bool output; output ==  ( e.adevice.iscapture == 0 );
            uint16_t which =        ( e.adevice.which );
            return (event_t) { .type = EVENT_AUDIO, .x = which, .y = output, .c = connected };
        }

// -----------------    Drag and Drop Events
        // case SDL_DROPFILE: case SDL_DROPTEXT: case SDL_DROPBEGIN: case SDL_DROPCOMPLETE:
        case SDL_DROPFILE: {
            if (e.drop.file) {
                strncpy(event_filename, e.drop.file, sizeof(event_filename)); SDL_free(e.drop.file); }
                return (event_t)    { .type = EVENT_DROP };
            }   return (event_t)    { .type = EVENT_NONE };
    }
}

event_t ui_eventloop (ui_t * window)
{
    event_t event;
    while(true) {
        event = ui_poll(window); event.source = window;
        if ( event.type == EVENT_NONE ) { continue; }
        if ( ui_timer(1/300) == false ) { ui_wait(15); continue; }
        window->handler(&event);
        ui_present(window);
    }
}

void event_print (event_t *event)
{
    if ( event->type == EVENT_NONE  ) return;
    char audio_event[][16] = { [false] = "Disconnected", [true] = "Connected" };
    char audio_type[][16] = { [false] = "Playback", [true] = "Input" };
    printf("Event.%s ", event_names[event->type]);
    switch(event->type) {
        default: break;
        case EVENT_DROP:        printf("\"%s\"", event_filename); break;
        case EVENT_AUDIO:       printf("%s %s #%d", audio_event[event->c], audio_type[event->y], event->x); break;
        case EVENT_KEYS:        printf("#%x", event->c); break;
        case EVENT_MOUSE:       printf("(%-4d %-4d %02x)", event->x, event->y, event->c); break;
        case EVENT_SCROLL:      printf("(%2d)", (int16_t)event->c); break;
    }   printf("\n");
}

#endif
