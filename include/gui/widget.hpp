#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <graphics/colors.hpp>
#include <graphics/font.hpp>
#include <gui/xserver.hpp>

typedef enum
{
    WIDGET_TYPE_BUTTON,
    WIDGET_TYPE_TEXTBOX,
    WIDGET_TYPE_CHECKBOX,
    WIDGET_TYPE_LISTBOX,
    WIDGET_TYPE_PROGRESSBAR,
    WIDGET_TYPE_NUMERICUPDOWN,
    WIDGET_TYPE_CONTAINER,
    WIDGET_TYPE_WINDOW,
    WIDGET_TYPE_SYSTEM,
} WIDGET_TYPE;

namespace System
{
    namespace GUI
    {
        // widget base class
        struct Widget
        {
            public:
                // click event
                void (*OnClick)(void);
                // mouse events
                void (*OnMouseEnter)(void);
                void (*OnMouseLeave)(void);
                void (*OnMouseDown)(void);
                void (*OnMouseUp)(void);
                void (*OnActivate)(void);
                // dimensions
                bounds_t* Bounds;
                point_t* ScreenPosition;
                // properties
                WIDGET_TYPE Type;
                char* Text;
                // flags
                bool Visible, Enabled, Active;
                MouseEventFlags* MSFlags;
                // style
                VisualStyle* Style;
        };

        // button - derived from widget base
        class Button
        {
            public:
                Button();
                Button(uint32_t x, uint32_t y, char* text);
                void Update();
                void Draw();

                Widget Base;
        };

        // textbox - derived from widget base
        class TextBox
        {
            public:
                TextBox();
                TextBox(uint32_t x, uint32_t y, char* text);
                void Update();
                void Draw();

                Widget Base;
        };
    }
}