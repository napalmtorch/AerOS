#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <graphics/bitmap.hpp>
#include <gui/xserver.hpp>
#include <gui/style.hpp>
#include <hardware/drivers/mouse.hpp>

namespace System
{
    namespace GUI
    {
        enum class WidgetType
        {
            Container,
            Button,
            CheckBox,
            TextBox,
        };

        struct WidgetFlags
        {
            bool Active;
            bool Enabled;
            bool Visible;
        };

        struct MSFlags
        {
            bool Hover;
            bool Down;
            bool Up;
            bool Clicked;
            bool Unclicked;
            bool Leave;
        };
        
        class Widget
        {
            public:
                Widget();
                Widget(int16_t x, int16_t y, int16_t w, int16_t h, WidgetType type);
                Widget(int16_t x, int16_t y, int16_t w, int16_t h, WidgetType type, void* parent);
                virtual ~Widget();
                virtual void Update();
                virtual void Draw();
                virtual void OnClick();
                virtual void OnMouseDown();
                virtual void OnMouseUp();
                virtual void OnMouseHover();
                virtual void OnMouseLeave();

            public:
                void (*Click)(void* win);
                void (*MouseDown)(void* win);
                void (*MouseUp)(void* win);
                void (*MouseHover)(void* win);
                void (*MouseLeave)(void* win);

            public:
                void SetPosition(int16_t x, int16_t y);
                void SetSize(int16_t w, int16_t h);
                void SetBounds(int16_t x, int16_t y, int16_t w, int16_t h);
                void SetText(char* text);

            public:
                void*            Parent;
                VisualStyle        Style;
                WidgetType         Type;
                WidgetFlags        Flags;
                MSFlags            MouseFlags;
                bounds_t           Bounds;
                char* Text;

        };

        class Button : public Widget
        {
            public:
                Button();
                Button(int16_t x, int16_t y, char* text);
                Button(int16_t x, int16_t y, char* text, void* parent);
                ~Button();
                void Update() override;
                void Draw() override;
                void OnClick() override;
                void OnMouseDown() override;
                void OnMouseUp() override;
                void OnMouseHover() override;
                void OnMouseLeave() override;
        };
    }
}