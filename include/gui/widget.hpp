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
        class Widget
        {
            public:
                virtual void Create() = 0;
                // update
                virtual void Update() = 0;
                // draw
                virtual void Draw() = 0;
                // click event
                void (*OnClick)(void);
                // mouse events
                void (*OnMouseEnter)(void);
                void (*OnMouseLeave)(void);
                void (*OnMouseDown)(void);
                void (*OnMouseUp)(void);

                bounds_t* GetBounds();
                point_t* GetScreenPosition();
                WIDGET_TYPE GetType();
                char* GetText();
                bool IsVisible();
                bool IsEnabled();
                bool IsActive();
                MouseEventFlags* GetMouseFlags();
                VisualStyle* GetStyle();
            protected:
                // dimensions
                bounds_t* Bounds;
                point_t* ScreenPosition;
                // properties
                WIDGET_TYPE Type;
                String* Text;
                // flags
                bool Visible, Enabled, Active;
                MouseEventFlags* MSFlags;
                // style
                VisualStyle* Style;
        };

        // button - derived from widget base
        class Button : public Widget
        {
            public:
                // create button
                void Create();
                void Create(uint32_t x, uint32_t y, char* text);
                // update override
                void Update();
                // draw override
                void Draw();
        };
    }
}