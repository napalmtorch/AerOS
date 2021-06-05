#include <gui/widget.hpp>

namespace System
{
    namespace GUI
    {
        // constructor - blank
        Window::Window() { }

        // constructor - create window with specified properties
        Window::Window(int32_t x, int32_t y, int32_t w, int32_t h, char* title, char* name)
        {
            Title = title;
            Name = name;
            SetBounds(x, y, w, h);
            CopyStyle(&WindowStyle, &Style);
        }

        // destroy window
        Window::~Window()
        {

        }

        // update window
        void Window::Update()
        {
            ClientBounds.X = Bounds.X + 1;
            ClientBounds.Y = Bounds.Y + 20;
            ClientBounds.Width = Bounds.Width - 2;
            ClientBounds.Height = Bounds.Height - 21;
        }

        // draw window
        void Window::Draw()
        {
            // draw background
            Graphics::Canvas::DrawFilledRectangle(Bounds.X, Bounds.Y, Bounds.Width, 20, Style.Colors[3]);
            Graphics::Canvas::DrawFilledRectangle(ClientBounds, Style.Colors[0]);

            // draw border
            Graphics::Canvas::DrawRectangle(Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height, 1, Style.Colors[2]);
            //Graphics::Canvas::DrawFilledRectangle(Bounds.X, Bounds.Y + 20, Bounds.Width, 1, Style.Colors[2]);
        }

        void Window::SetPosition(int32_t x, int32_t y) { Bounds.X = x; Bounds.Y = y; }
        void Window::SetSize(int32_t w, int32_t h) { Bounds.Width = w; Bounds.Height = h; }
        void Window::SetBounds(int32_t x, int32_t y, int32_t w, int32_t h) { SetPosition(x, y); SetSize(w, h); }
    }
}