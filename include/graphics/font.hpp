#pragma once
#include "lib/types.h"

namespace Graphics
{
    struct Font
    {
        protected:
            uint8_t  Width;
            uint8_t  Height;
            uint8_t  HorizontalSpacing;
            uint8_t  VerticalSpacing;
            uint8_t* Data;

        public:
            Font(uint8_t w, uint8_t h, uint8_t sh, uint8_t sv, uint8_t* data) : Width(w), Height(h), HorizontalSpacing(sh), VerticalSpacing(sv), Data(data) { }
            uint8_t GetWidth() { return Width; }
            uint8_t GetHeight() { return Height; }
            uint8_t GetHorizontalSpacing() { return HorizontalSpacing; }
            uint8_t GetVerticalSpacing() { return VerticalSpacing; }
            uint8_t* GetData() { return Data; }
    };

    // pre-defined fonts
    extern Font FONT_3x5;
    extern Font FONT_8x8;
    extern Font FONT_8x16;

    // initialize fonts
    void InitializeFonts();

    uint8_t ConvertByteToBitAddress(uint8_t to_convert, int32_t to_return);
}