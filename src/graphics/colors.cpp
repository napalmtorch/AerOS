#include "graphics/colors.hpp"

namespace Graphics
{
    // brute force method for converting vga palette color to rgb
    uint32_t VGAPaletteToRGB(uint8_t color)
    { 
        /* TODO: not yet implemented */
        UNUSED(color);
        return 0;
    }

    // brute force method for converting rgb to palette color
    uint8_t RGBToVGAPallete(uint8_t r, uint8_t g, uint8_t b)
    {
        uint32_t packed = (r << 16) | (g << 8) | b;

        // black
        if (packed == 0x000000) { return (uint8_t)COL4_BLACK; }
        // white
        if (packed == 0xFFFFFF) { return (uint8_t)COL4_WHITE; }
        // red
        if (packed == 0xFF0000) { return (uint8_t)COL4_RED; }
        // blue
        if (packed == 0x0000FF) { return (uint8_t)COL4_BLUE; }

        return 0;
    }
    uint8_t RGBToVGAPallete(Color color) { return RGBToVGAPallete(color.GetRed(), color.GetGreen(), color.GetBlue()); }

    // convert rgb value to packed color
    uint32_t RGBToPackedValue(uint8_t r, uint8_t g, uint8_t b)
    {
        /* TODO: not yet implemented */
        UNUSED(r);
        UNUSED(g);
        UNUSED(b);
        return 0;
    }
}