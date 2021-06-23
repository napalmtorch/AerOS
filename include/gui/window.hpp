#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>
#include <graphics/bitmap.hpp>
#include <gui/xserver.hpp>
#include <gui/style.hpp>
#include <hardware/drivers/mouse.hpp>
#include <gui/widget.hpp>

namespace System
{
    namespace GUI
    {
        enum class WindowState
        {
            Minimized = 0,
            Normal    = 1,
            Maximized = 2,
        };

        struct WindowFlags
        {
            bool Active;
            bool Moving;
            bool Resizing;
            bool CanMaximize;
            bool CanResize;
            bool CanRefresh;
        };
        
        class WindowRenderer;

        class Window
        {
            public:
                Window();
                Window(int16_t x, int16_t y, int16_t w, int16_t h, char* title, char* id);
                virtual ~Window();

                virtual void OnLoad();
                virtual void OnClose();

                virtual void Update();
                virtual void Draw();
                virtual void Refresh();

                void SendRefresh();
                void UpdateClientBounds();

                void SetPosition(int16_t x, int16_t y);
                void SetSize(int16_t w, int16_t h);
                void SetBounds(int16_t x, int16_t y, int16_t w, int16_t h);
                void SetTitle(char* title);

                void AddWidget(void* widget);
                void* GetWidget(int32_t index);
                char *GetID();

            public:
                WindowRenderer*    Renderer;
                Graphics::Bitmap*  Buffer;
                VisualStyle        Style;
                WindowState        State;
                WindowFlags        Flags;
                HAL::MouseFlags    MouseFlags;
                bounds_t           Bounds;
                bounds_t           LastBounds;
                bounds_t           ClientBounds;
                bounds_t           TitleBarBounds;
                point_t            MaximumSize;
                point_t            MinimumSize;
                char*              Title;
                uint32_t*          Widgets;
                bool               ExitRequest;

            public:
                Button* BtnClose;
                Button* BtnMax;
                Button* BtnMin;

            protected:
                char* ID;
                int32_t WidgetCount;
        };

        class WindowRenderer
        {
            public:
                WindowRenderer();
                WindowRenderer(GUI::Window* parent);

            // drawing methods
            public:
                void Clear(Color color);
                void DrawPixel(int16_t x, int16_t y, Color color);
                void DrawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color);
                void DrawFilledRect(point_t pos, point_t size, Color color);
                void DrawFilledRect(bounds_t bounds, Color color);
                void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t thickness, Color color);
                void DrawRect(point_t pos, point_t size, int16_t thickness, Color color);
                void DrawRect(bounds_t bounds, int16_t thickness, Color color);
                void DrawRect3D(int16_t x, int16_t y, int16_t w, int16_t h, Color tl, Color b_inner, Color b_outer);          
                void DrawRect3D(bounds_t bounds, Color tl, Color b_inner, Color b_outer);      
                void DrawChar(int16_t x, int16_t y, char c, Color fg, Graphics::Font font);
                void DrawChar(point_t pos, char c, Color fg, Graphics::Font font);
                void DrawChar(int16_t x, int16_t y, char c, Color fg, Color bg, Graphics::Font font);
                void DrawChar(point_t pos, char c, Color fg, Color bg, Graphics::Font font);
                void DrawString(int16_t x, int16_t y, char* text, Color fg, Graphics::Font font);
                void DrawString(point_t pos, char* text, Color fg, Graphics::Font font);
                void DrawString(int16_t x, int16_t y, char* text, Color fg, Color bg, Graphics::Font font);
                void DrawString(point_t pos, char* text, Color fg, Color bg, Graphics::Font font);
                void DrawArray(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t* data);
                void DrawArray(int16_t x, int16_t y, int16_t w, int16_t h, Color trans_key, uint32_t* data);

            public:
                GUI::Window* Parent;
        };
    }
}