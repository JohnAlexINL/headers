# stb-like Headers & Utilities

This repo contains a set of stb-like header files
and standalone utilities.

As a very simple way to get these includes into your project,
you could run something like

```sh
HEADERS="https://github.com/JohnAlexINL/headers/archive/refs/heads/stabs.zip"
ZIP="headers.zip"
TMP="tmp"
DIR="headers/"
mkdir -p "$DIR"
mkdir -p "$TMP"
wget -O $ZIP $HEADERS
unzip -q $ZIP -d $TMP
cp -r $TMP/headers-stabs/headers/* $DIR
rm -rf $ZIP $TMP
```

This would extract the `headers` directory from this project
into your local directory without any Git "subrepo"/"--recursive" weirdness.

| Utility | Purpose | Usage |
|--|--|--|
| glyph.c | Make monospace bitmap spritesheets from a `*.ttf` font  | `glyph <font.ttf> <list of sizes>` |

| Header | Purpose | Dependencies |
|--|--|--|
| file.h | Functions for reading and writing to file | glibc |
| glyph.h | Functions for making mono spritesheets from `*.ttf` | glibc, [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h), [stb_truetype.h](https://github.com/nothings/stb/blob/master/stb_truetype.h) |
| ui.h | Functions for creating a window with an event loop exposing a framebuffer for graphics | glibc, [SDL2](https://www.libsdl.org) |
| gfx.h | Functions for drawing to a framebuffer, including glyph-based text | `ui.h` |
| widgets.h | Functions for drawing interactive elements | `ui.h`, `gfx.h` |

## Todos

- Write a `architecture.h` who sets up our define-guards for platform-specific code.
- Factor Bitmap functions out from UI and GFX into their own reusable header
- Rewrite `file.h` to use syscalls and compiler guards instead, freeing it of glibc
- Rewrite `ui.h` to grab its own framebuffer and events. Use system guards
- Add additional headers for:
    - JSON and RPC protocol
    - QOI, Bitmap, and JPG images
    - MP3, WAV, AAV, and QOA audio

## License

Unless elsewise specified, all code in this project is made available under
[BloodRose.org's](https://bloodrose.org)
4 Clause license.
It is a ~40-line BSD-style license with commercial-use restrictions.
