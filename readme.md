# stb-like Headers & Utilities

This repo contains a set of stb-like header files
and standalone utilities.

| Utility | Purpose | Usage |
|--|--|--|
| Glyph.c | Make monospace bitmap spritesheets from a `*.ttf` font  | `glyph <font.ttf> <list of sizes>` |

| Header | Purpose | Dependencies |
|--|--|--|
| Glyph.h | Functions for making mono spritesheets from `*.ttf` | glibc, [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h), [stb_truetype.h](https://github.com/nothings/stb/blob/master/stb_truetype.h) |

## License

[Blood Rose.org's](https://bloodrose.org)
4 Clause license, contains commercial restriction. BSD-style, 40 lines.