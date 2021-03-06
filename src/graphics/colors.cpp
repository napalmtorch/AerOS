#include "graphics/colors.hpp"
#include "lib/string.hpp"
#include "core/kernel.hpp"

namespace Graphics
{
    bool IsColorEqual(Color a, Color b)
    {
        return a.R == b.R && a.G == b.G && a.B == b.B;
    }

    // brute force method for converting vga palette color to rgb
    uint32_t VGAPaletteToRGB(uint8_t color)
    {
        COL4 c = (COL4)color;
        switch (c)
        {
            case COL4_BLACK:        { return RGBToPackedValue(0x00, 0x00, 0x00); }
            case COL4_DARK_BLUE:    { return RGBToPackedValue(0x00, 0x00, 0x7F); }
            case COL4_DARK_GREEN:   { return RGBToPackedValue(0x00, 0x7F, 0x00); }
            case COL4_DARK_CYAN:    { return RGBToPackedValue(0x00, 0x7F, 0x7F); }
            case COL4_DARK_RED:     { return RGBToPackedValue(0x7F, 0x00, 0x00); }
            case COL4_DARK_MAGENTA: { return RGBToPackedValue(0x7F, 0x00, 0x7F); }
            case COL4_DARK_YELLOW:  { return RGBToPackedValue(0x7F, 0x7F, 0x00); }
            case COL4_GRAY:         { return RGBToPackedValue(0xAF, 0xAF, 0xAF); }
            case COL4_DARK_GRAY:    { return RGBToPackedValue(0x7F, 0x7F, 0x7F); }
            case COL4_BLUE:         { return RGBToPackedValue(0x00, 0x00, 0xFF); }
            case COL4_GREEN:        { return RGBToPackedValue(0x00, 0xFF, 0x00); }
            case COL4_CYAN:         { return RGBToPackedValue(0x00, 0xFF, 0xFF); }
            case COL4_RED:          { return RGBToPackedValue(0xFF, 0x00, 0x00); }
            case COL4_MAGENTA:      { return RGBToPackedValue(0xFF, 0x00, 0xFF); }
            case COL4_YELLOW:       { return RGBToPackedValue(0xFF, 0xFF, 0x00); }
            case COL4_WHITE:        { return RGBToPackedValue(0xFF, 0xFF, 0xFF); }
            default: { return RGBToPackedValue(0x00, 0x00, 0x00); }
        }
    }

    // brute force method for converting rgb to palette color
    uint8_t RGBToVGAPallete(uint8_t r, uint8_t g, uint8_t b)
    {
        return RGBToVGAPallete({ 255, r, g, b });
    }

    uint8_t RGBToVGAPallete(Color color)
    {
        if (IsColorEqual(color, Colors::Black)) { return 0; }
        else if (IsColorEqual(color, Colors::DarkBlue)) { return 1; }
        else if (IsColorEqual(color, Colors::DarkGreen)) { return 2; }
        else if (IsColorEqual(color, Colors::DarkCyan)) { return 3; }
        else if (IsColorEqual(color, Colors::DarkRed)) { return 4; }
        else if (IsColorEqual(color, Colors::DarkMagenta)) { return 5; }
        else if (IsColorEqual(color, Colors::DarkOrange)) { return 6; }
        else if (IsColorEqual(color, { 0xAF, 0xAF, 0xAF})) { return 7; }
        else if (IsColorEqual(color, Colors::DarkGray)) { return 8; }
        else if (IsColorEqual(color, Colors::Blue)) { return 9; }
        else if (IsColorEqual(color, Colors::Green)) { return 10; }
        else if (IsColorEqual(color, Colors::Cyan)) { return 11; }
        else if (IsColorEqual(color, Colors::Red)) { return 12; }
        else if (IsColorEqual(color, Colors::Magenta)) { return 13; }
        else if (IsColorEqual(color, Colors::Yellow)) { return 14; }
        else if (IsColorEqual(color, Colors::White)) { return 15; }
    }

    // convert rgb value to packed color
    uint32_t RGBToPackedValue(uint8_t r, uint8_t g, uint8_t b)
    {
        if (r == 0 && g == 0 && b == 0) { return 0xFF000000; }
        return (uint32_t)((0xFF << 24) | (r << 16) | (g << 8) | b);
    }
    
    Color PackedValueToRGB(uint32_t packed)
    {
        return { (uint8_t)((packed & 0xFF000000) >> 24), (uint8_t)((packed & 0x00FF0000) >> 16), (uint8_t)((packed & 0x0000FF00) >> 8), (uint8_t)(packed & 0x000000FF) };
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

    namespace Colors
    {
        const Color AliceBlue                   = { 0xFF, 0xF0, 0xF8, 0xFF };
        const Color AntiqueWhite                = { 0xFF, 0xFA, 0xEB, 0xD7 };
        const Color Aqua                        = { 0xFF, 0x00, 0xFF, 0xFF };
        const Color Aquamarine                  = { 0xFF, 0x7F, 0xFF, 0xD4 };
        const Color Azure                       = { 0xFF, 0xF0, 0xFF, 0xFF };
        const Color Beige                       = { 0xFF, 0xF5, 0xF5, 0xDC };
        const Color Bisque                      = { 0xFF, 0xFF, 0xE4, 0xC4 };
        const Color Black                       = { 0xFF, 0x00, 0x00, 0x00 };
        const Color BlanchedAlmond              = { 0xFF, 0xFF, 0xEB, 0xCD };
        const Color Blue                        = { 0xFF, 0x00, 0x00, 0xFF };
        const Color BlueViolet                  = { 0xFF, 0x8A, 0x2B, 0xE2 };
        const Color Brown                       = { 0xFF, 0xA5, 0x2A, 0x2A };
        const Color BurlyWood                   = { 0xFF, 0xDe, 0xB8, 0x87 };
        const Color CadetBlue                   = { 0xFF, 0x5F, 0x9E, 0xA0 };
        const Color Chartreuse                  = { 0xFF, 0x7F, 0xFF, 0x00 };
        const Color Chocolate                   = { 0xFF, 0xD2, 0x69, 0x1E };
        const Color Coral                       = { 0xFF, 0xFF, 0x7F, 0x50 };
        const Color CornflowerBlue              = { 0xFF, 0x64, 0x95, 0xED };
        const Color Cornsilk                    = { 0xFF, 0xFF, 0xF8, 0xDC };
        const Color Crimson                     = { 0xFF, 0xDC, 0x14, 0x3C };
        const Color Cyan                        = { 0xFF, 0x00, 0xFF, 0xFF };
        const Color DarkBlue                    = { 0xFF, 0x00, 0x00, 0x8B };
        const Color DarkCyan                    = { 0xFF, 0x00, 0x8B, 0x8B };
        const Color DarkGoldenrod               = { 0xFF, 0xB8, 0x86, 0x0B };
        const Color DarkGray                    = { 0xFF, 0xA9, 0xA9, 0xA9 };
        const Color DarkGreen                   = { 0xFF, 0x00, 0x64, 0x00 };
        const Color DarkKhaki                   = { 0xFF, 0xBD, 0xB7, 0x6B };
        const Color DarkMagenta                 = { 0xFF, 0x8B, 0x00, 0x8B };
        const Color DarkOliveGreen              = { 0xFF, 0x55, 0x6B, 0x2F };
        const Color DarkOrange                  = { 0xFF, 0xFF, 0x8C, 0x00 };
        const Color DarkOrchid                  = { 0xFF, 0x99, 0x32, 0xCC };
        const Color DarkRed                     = { 0xFF, 0x8B, 0x00, 0x00 };
        const Color DarkSalmon                  = { 0xFF, 0xE9, 0x96, 0x7A };
        const Color DarkSeaGreen                = { 0xFF, 0x8F, 0xBC, 0x8F };
        const Color DarkSlateBlue               = { 0xFF, 0x48, 0x3D, 0x8B };
        const Color DarkSlateGray               = { 0xFF, 0x2F, 0x4F, 0x4F };

        const Color White                       = { 0xFF, 0xFF, 0xFF, 0xFF };
        const Color Red                         = { 0xFF, 0xFF, 0x00, 0x00 };
        const Color Green                       = { 0xFF, 0x00, 0xFF, 0x00 };
        const Color Yellow                      = { 0xFF, 0xFF, 0xFF, 0x00 };
        const Color Magenta                     = { 0xFF, 0xFF, 0x00, 0xFF };

        const Color Transparent                 = { 0x00, 0x00, 0x00, 0x00 };
    }
}