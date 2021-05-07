#include "graphics/colors.hpp"
#include "lib/string.hpp"
#include "core/kernel.hpp"

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

    COL4 GetColorFromName(char* name)
    {
        // convert name to upper case
        char upper[strlen(name)];
        strcpy(name, upper);
        strupper(upper);
        
        // black
        if (streql(upper, "BLACK")) { return COL4_BLACK; }
        if (streql(upper, "DARK BLUE") || streql(upper, "DARKBLUE") || streql(upper, "DARK_BLUE"))       { return COL4_DARK_BLUE; }
        if (streql(upper, "DARK GREEN") || streql(upper, "DARKGREEN") || streql(upper, "DARK_GREEN"))    { return COL4_DARK_GREEN; }
        if (streql(upper, "DARK CYAN") || streql(upper, "DARKCYAN") || streql(upper, "DARK_CYAN"))       { return COL4_DARK_CYAN; }
        if (streql(upper, "DARK RED") || streql(upper, "DARKRED") || streql(upper, "DARK_RED"))          { return COL4_DARK_RED; }
        if (streql(upper, "DARK MAGENTA") || streql(upper, "DARKMAGENTA") || streql(upper, "DARK_MAGENTA") || streql(upper, "PURPLE")) { return COL4_DARK_MAGENTA; }
        if (streql(upper, "DARK YELLOW") || streql(upper, "DARKYELLOW") || streql(upper, "DARK_YELLOW")) { return COL4_DARK_YELLOW; }
        if (streql(upper, "DARK GRAY") || streql(upper, "DARKGRAY") || streql(upper, "DARK_GRAY"))       { return COL4_DARK_GRAY; }
        if (streql(upper, "GRAY"))    { return COL4_GRAY; }
        if (streql(upper, "BLUE"))    { return COL4_BLUE; }
        if (streql(upper, "GREEN"))   { return COL4_GREEN; }
        if (streql(upper, "CYAN"))    { return COL4_CYAN; }
        if (streql(upper, "RED"))     { return COL4_RED; }
        if (streql(upper, "MAGENTA") || streql(upper, "PINK")) { return COL4_MAGENTA; }
        if (streql(upper, "YELLOW"))  { return COL4_YELLOW; }
        if (streql(upper, "WHITE"))   { return COL4_WHITE; }

        // return
        return COL4_BLACK;
    }
}