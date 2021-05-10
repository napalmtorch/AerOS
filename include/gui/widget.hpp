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
                bounds_t Bounds;
                point_t ScreenPosition;
                // properties
                WIDGET_TYPE Type;
                char* Text;
                // flags
                bool Visible, Enabled, Active;
                MouseEventFlags MSFlags;
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
                void SetText(char* text);

                Widget Base;
        };

        // title bar - derived from widget base
        class TitleBar
        {
            public:
                TitleBar();
                TitleBar(Widget* parent);
                void Update();
                void Draw();

                // base
                Widget Base;
                Widget* Parent;
                Button CloseBtn;
                Button MaxBtn;
                Button MinBtn;
        };

        // window - derived from widget base
        class Window
        {
            public:
                Window();
                Window(uint32_t x, uint32_t y, uint32_t w, uint32_t h, char* title);
                void Update();
                void Draw();
                void SetTitle(char* title);
                void (*ChildUpdate)(void);
                void (*ChildDraw)(void);

                // base
                Widget Base;
                TitleBar TBar;
        };

        // textbox - derived from widget base
        class TextBox
        {
            public:
                TextBox();
                TextBox(uint32_t x, uint32_t y, char* text);
                void Update();
                void Draw();
                void SetText(char* text);

                Widget Base;
        };

        // check default widget events
        void CheckWidgetEvents(Widget* widget);

        // set widget text
        void SetWidgetText(Widget* widget, char* text);

    }
}