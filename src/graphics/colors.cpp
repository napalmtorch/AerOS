#include "graphics/colors.hpp"
#include "lib/string.hpp"

namespace Graphics
{
    // brute force method for converting vga palette color to rgb
    uint32_t VGAPaletteToRGB(uint8_t color)
    {
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
        UNUSED(r);
        UNUSED(g);
        UNUSED(b);
        return 0;
    }

    COL4 GetColorFromName(char* colorname)
    {
        COL4 background = COL4_BLACK;
        if(streql("black",colorname)) {
            background = COL4_BLACK;
        }
        else if(streql("darkblue",colorname)) {
            background = COL4_DARK_BLUE;
        }
        else if(streql("darkgreen",colorname)) {
            background = COL4_DARK_GREEN;
        }
        else if(streql("darkcyan",colorname)) {
            background = COL4_DARK_CYAN;
        }
        else if(streql("darkred",colorname)) {
            background = COL4_DARK_RED;
        }
        else if(streql("darkmagenta",colorname)) {
            background = COL4_DARK_MAGENTA;
        }
        else if(streql("darkyellow",colorname)) {
            background = COL4_DARK_YELLOW;
        }
        else if(streql("gray",colorname)) {
            background = COL4_GRAY;
        }
        else if(streql("darkgray",colorname)) {
            background = COL4_DARK_GRAY;
        }
        else if(streql("blue",colorname)) {
            background = COL4_BLUE;
        }
        else if(streql("green",colorname)) {
            background = COL4_GREEN;
        }
        else if(streql("cyan",colorname)) {
            background = COL4_CYAN;
        }
        else if(streql("red",colorname)) {
            background = COL4_RED;
        }
        else if(streql("magenta",colorname)) {
            background = COL4_MAGENTA;
        }
        else if(streql("yellow",colorname)) {
            background = COL4_YELLOW;
        }
        else if(streql("WHITE",colorname)) {
            background = COL4_MAGENTA;
        }
        return background;
    }
}