#pragma once
#include "lib/types.h"

extern "C"
{
    // color depths
    typedef enum
    {
        COLOR_DEPTH_4   = 4,
        COLOR_DEPTH_8   = 8,
        COLOR_DEPTH_16  = 16,
        COLOR_DEPTH_24  = 24,
        COLOR_DEPTH_32  = 32,
    } COLOR_DEPTH;

    // color orders
    typedef enum
    {
        COLOR_ORDER_RGBA,
        COLOR_ORDER_ARGB,
        COLOR_ORDER_BGRA,
        COLOR_ORDER_RGB,
        COLOR_ORDER_BGR,
        COLOR_ORDER_PALETTE_16,
        COLOR_ORDER_PALETTE_256,
    } COLOR_ORDER;

    // 4 bit color enumerator
    typedef enum
    {
        COL4_BLACK,
        COL4_DARK_BLUE,
        COL4_DARK_GREEN,
        COL4_DARK_CYAN,
        COL4_DARK_RED,
        COL4_DARK_MAGENTA,
        COL4_DARK_YELLOW,
        COL4_GRAY,
        COL4_DARK_GRAY,
        COL4_BLUE,
        COL4_GREEN,
        COL4_CYAN,
        COL4_RED,
        COL4_MAGENTA,
        COL4_YELLOW,
        COL4_WHITE,
    } COL4;

    // 8 bit color enumerator
    typedef enum
    {
        COL8_BLACK,
        COL8_DARK_BLUE,
        COL8_DARK_GREEN,
        COL8_DARK_CYAN,
        COL8_DARK_RED,
        COL8_DARK_MAGENTA,
        COL8_DARK_YELLOW,
        COL8_GRAY,
        COL8_DARK_GRAY,
        COL8_BLUE,
        COL8_GREEN,
        COL8_CYAN,
        COL8_RED,
        COL8_MAGENTA,
        COL8_YELLOW,
        COL8_WHITE,
    } COL8;
}

// rgb color
typedef struct
{
    uint8_t A;
    uint8_t R;
    uint8_t G;
    uint8_t B;
} __attribute__((packed)) Color;

namespace Graphics
{
    // brute force method for converting vga palette color to rgb
    uint32_t VGAPaletteToRGB(uint8_t color);

    // brute force method for converting rgb to palette color
    uint8_t RGBToVGAPallete(uint8_t r, uint8_t g, uint8_t b);
    uint8_t RGBToVGAPallete(Color color);

    // convert rgb value to packed color
    uint32_t RGBToPackedValue(uint8_t r, uint8_t g, uint8_t b);

    // pre-defined rgb colors
    namespace Colors
    {
        extern const Color Black;
        extern const Color White;
        extern const Color Red;
        extern const Color Green;
        extern const Color Blue;
        extern const Color Yellow;
        extern const Color Cyan;
        extern const Color Magenta;
        extern const Color Gray;
    }
    
    COL4 GetColorFromName(char* name);
}