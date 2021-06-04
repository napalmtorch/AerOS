#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>

namespace System
{
    namespace GUI
    {
        // types of border styles
        typedef enum
        {
            BORDER_STYLE_NONE,
            BORDER_STYLE_ROUNDED,
            BORDER_STYLE_FLAT,
            BORDER_STYLE_3D,
        } BORDER_STYLE;

        // structure for managing graphical visual style attributes
        struct VisualStyle
        {
            char Name[32];
            Color Colors[8];
            int32_t BorderSize;
            BORDER_STYLE BorderStyle;
            Graphics::Font* Font;
        };
    }
}