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
}

// rgb color
struct Color
{
    private:
        uint8_t A;
        uint8_t R;
        uint8_t G;
        uint8_t B;
    public:
        Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : A(a), R(r), G(g), B(b) { }
        uint8_t GetAlpha() {return A; }
        uint8_t GetRed() { return R; }
        uint8_t GetGreen() { return G; }
        uint8_t GetBlue() { return B; }
        uint32_t ToPackedValue(COLOR_ORDER order)
        {
            /* TODO: not yet implemented */
            UNUSED(order);
            return 0;
        }
};

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
        const Color Black           = { 0xFF, 0x00, 0x00, 0x00 };
        const Color White           = { 0xFF, 0xFF, 0xFF, 0xFF };
        const Color Red             = { 0xFF, 0xFF, 0x00, 0x00 };
        const Color Green           = { 0xFF, 0x00, 0xFF, 0x00 };
        const Color Blue            = { 0xFF, 0x00, 0x00, 0xFF };
        const Color Yellow          = { 0xFF, 0xFF, 0xFF, 0x00 };
        const Color Cyan            = { 0xFF, 0x00, 0xFF, 0xFF };
        const Color Magenta         = { 0xFF, 0xFF, 0x00, 0xFF };
        const Color Gray            = { 0xFF, 0x7F, 0x7F, 0x7F }; 
    }
}