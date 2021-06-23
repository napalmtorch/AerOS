#include <gui/window.hpp>
#include <core/kernel.hpp>
#include <gui/widget.hpp>

namespace System
{
    namespace GUI
    {
        Window::Window()
        {

        }

        void CloseClicked(void* win);

        Window::Window(int16_t x, int16_t y, int16_t w, int16_t h, char* title, char* id)
        {
            SetBounds(x, y, w, h);
            SetTitle(title);

            this->Flags.CanMaximize = true;
            this->Flags.CanResize = true;
            this->Flags.Active = true;
            this->Flags.CanRefresh = true;
            this->State = WindowState::Normal;
            this->ID = id;

            CopyStyle((VisualStyle*)&WindowStyle, &Style);

            Buffer = new Graphics::Bitmap(w, h, COLOR_DEPTH_32);

            Renderer = new WindowRenderer(this);      
            UpdateClientBounds();

            // initialize widget list
            Widgets = new uint32_t[4096];
            WidgetCount = 0;

            // close button
            BtnClose = new Button(0, 0, "", this);
            BtnClose->SetSize(14, 13);
            BtnClose->Click = CloseClicked;

            // max button
            BtnMax = new Button(0, 0, "", this);
            BtnMax->SetSize(14, 13);

            // min button
            BtnMin = new Button(0, 0, "", this);
            BtnMin->SetSize(14, 13);

            Refresh();
        }

        Window::~Window()
        {

        }

        void Window::OnLoad()
        {
            Flags.Active = true;
            Update();
            Draw();
            SendRefresh();
        }

        void Window::OnClose()
        {

        }

        bool move_click;
        int32_t mx_start, my_start;
        void Window::Update()
        {
            if (Flags.Active)
            {
                UpdateClientBounds();

                // check for movement
                if (State != WindowState::Minimized && State != WindowState::Maximized)
                {
                    // get mouse position
                    int32_t mx = KernelIO::Mouse.GetX();
                    int32_t my = KernelIO::Mouse.GetY();

                    // check for moveent
                    if (rect_contains(Bounds.X + TitleBarBounds.X, Bounds.Y + TitleBarBounds.Y, TitleBarBounds.Width, TitleBarBounds.Height, mx, my))
                    {
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed && !BtnClose->MouseFlags.Hover && !BtnMax->MouseFlags.Hover && !BtnMin->MouseFlags.Hover)
                        {
                            if (!move_click)
                            {
                                mx_start = mx - Bounds.X;
                                my_start = my - Bounds.Y;
                                move_click = true;
                            }
                            Flags.Moving = true;
                        }
                    }

                    // relocate window
                    if (Flags.Moving)
                    {
                        Bounds.X = mx - mx_start;
                        Bounds.Y = my - my_start;
                        if (Bounds.Y < 0) { Bounds.Y = 0; }
                    }

                    // mouse release
                    if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released)
                    {
                        Flags.Moving = false;
                        Flags.Resizing = false;
                        move_click = false;
                    }
                }

                // update widgets
                for (size_t i = 0; i < 4096; i++)
                {
                    if (Widgets[i] != 0)
                    {
                        GUI::Widget* widget = (GUI::Widget*)Widgets[i];
                        widget->Update();
                    }
                }

                BtnClose->SetPosition(TitleBarBounds.X + TitleBarBounds.Width - 16, TitleBarBounds.Y + 2);
                BtnClose->Update();

                BtnMax->SetPosition(BtnClose->Bounds.X - 16, BtnClose->Bounds.Y);
                BtnMax->Update();

                BtnMin->SetPosition(BtnMax->Bounds.X - 16, BtnClose->Bounds.Y);
                BtnMin->Update();
            }
            
            // refresh on move or resize
            if (LastBounds.X != Bounds.X || LastBounds.Y != Bounds.Y || LastBounds.Width != Bounds.Width || LastBounds.Height != Bounds.Height)
            {
                SendRefresh();
                LastBounds.X = Bounds.X;
                LastBounds.Y = Bounds.Y;
                LastBounds.Width = Bounds.Width;
                LastBounds.Height = Bounds.Height;
            }
        }
        
        void Window::Draw()
        {
            if (Flags.Active)
            {
                // draw client bounds
                Renderer->DrawFilledRect(ClientBounds, Style.Colors[0]);

                // draw border
                Renderer->DrawRect3D(0, 0, Bounds.Width, Bounds.Height, Style.Colors[2], Style.Colors[3], Style.Colors[4]);

                // draw title bar bounds
                Renderer->DrawFilledRect(TitleBarBounds, Style.Colors[5]);

                // draw title bar text
                if (Title != nullptr)
                {
                    if (strlen(Title) > 0)
                    {
                        Renderer->DrawString(TitleBarBounds.X + 4, TitleBarBounds.Y + 5, Title, Style.Colors[6], (*Style.Font));
                    }
                }

                // draw widgets
                for (size_t i = 0; i < 4096; i++)
                {
                    if (Widgets[i] != 0)
                    {
                        GUI::Widget* widget = (GUI::Widget*)Widgets[i];
                        widget->Draw();
                    }
                }

                BtnClose->Draw();
                BtnMax->Draw();
                BtnMin->Draw();
            }
        }

        void CloseClicked(void* win)
        {
            ((GUI::Window*)win)->ExitRequest = true;
        }

        void Window::Refresh()
        {
            if (Flags.CanRefresh)
            {
                Window::Draw();
                Flags.CanRefresh = false;
            }

            // copy buffer to screen
            if (!Flags.Moving)
            {
                Graphics::Canvas::DrawBitmapFast(Bounds.X, Bounds.Y, Buffer);
            }
            else 
            { 
                Graphics::Canvas::DrawRectangle(Bounds, 2, Graphics::Colors::White); 
                Graphics::Canvas::DrawRectangle(Bounds, 1, Graphics::Colors::Black);
            }
        }

        void Window::SendRefresh()
        {
            Flags.CanRefresh = true;
            Refresh();
        }

        void Window::UpdateClientBounds()
        {
            TitleBarBounds.X = Style.BorderSize;
            TitleBarBounds.Y = Style.BorderSize;
            TitleBarBounds.Width = Bounds.Width - ((Style.BorderSize * 2) + 1);
            TitleBarBounds.Height = 16;

            ClientBounds.X = Style.BorderSize;
            ClientBounds.Y = Style.BorderSize + (TitleBarBounds.Height + 1);
            ClientBounds.Width = Bounds.Width - (Style.BorderSize * 2);
            ClientBounds.Height = Bounds.Height - ((TitleBarBounds.Height + 1) + (Style.BorderSize * 2));
        }

        void Window::SetPosition(int16_t x, int16_t y)
        {
            Bounds.X = x;
            Bounds.Y = y;
        }

        void Window::SetSize(int16_t w, int16_t h)
        {
            Bounds.Width = w;
            Bounds.Height = h;   
        }

        void Window::SetBounds(int16_t x, int16_t y, int16_t w, int16_t h)
        {
            Bounds.X = x;
            Bounds.Y = y;
            Bounds.Width = w;
            Bounds.Height = h;   
        }

        void Window::SetTitle(char* title)
        {
            if (Title != nullptr) { delete Title; }
            Title = new char[strlen(title) + 1];
            strcpy(title, Title);
        }
        
        void Window::AddWidget(void* widget)
        {
            if (WidgetCount >= 4096) { return; }
            Widgets[WidgetCount] = (uint32_t)widget;
            WidgetCount++;
            SendRefresh();
        }

        void* Window::GetWidget(int32_t index)
        {
            if (index >= WidgetCount) { return nullptr; }
            return (void*)Widgets[index];
        }

        char* Window::GetID() { return ID; }

        WindowRenderer::WindowRenderer() { Parent = nullptr; }

        WindowRenderer::WindowRenderer(GUI::Window* parent) { Parent = parent; }

        void WindowRenderer::Clear(Color color)
        {
            Parent->Buffer->DrawFilledRect(0, 0, Parent->Buffer->Width, Parent->Buffer->Height, color);
        }

        void WindowRenderer::DrawPixel(int16_t x, int16_t y, Color color)
        {
            if (Parent == nullptr) { return; }
            Parent->Buffer->SetPixel(x, y, color);
        }
        
        void WindowRenderer::DrawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color)
        {
            if (Parent == nullptr) { return; }
            Parent->Buffer->DrawFilledRect(x, y, w, h, color);
        }

        void WindowRenderer::DrawFilledRect(point_t pos, point_t size, Color color) { DrawFilledRect(pos.X, pos.Y, size.X, size.Y, color); }

        void WindowRenderer::DrawFilledRect(bounds_t bounds, Color color) { DrawFilledRect(bounds.X, bounds.Y, bounds.Width, bounds.Height, color); }

        void WindowRenderer::DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t thickness, Color color)
        {
            DrawFilledRect(x, y, w, thickness, color);
            DrawFilledRect(x, y + h - thickness, w, thickness, color);
            DrawFilledRect(x, y + thickness, thickness, h - (thickness * 2), color);
            DrawFilledRect(x + w - thickness, y + thickness, thickness, h - (thickness * 2), color);
        }
        
        void WindowRenderer::DrawRect(point_t pos, point_t size, int16_t thickness, Color color) { DrawRect(pos.X, pos.Y, size.X, size.Y, thickness, color); }

        void WindowRenderer::DrawRect(bounds_t bounds, int16_t thickness, Color color) { DrawRect(bounds.X, bounds.Y, bounds.Width, bounds.Height, thickness, color); }

        void WindowRenderer::DrawRect3D(int16_t x, int16_t y, int16_t w, int16_t h, Color tl, Color b_inner, Color b_outer)
        {
            DrawFilledRect(x, y, w, 1, tl);
            DrawFilledRect(x, y, 1, h, tl);
            DrawFilledRect(x + 1, y + h - 2, w - 2, 1, b_inner);
            DrawFilledRect(x + w - 2, y + 1, 1, h - 2, b_inner);
            DrawFilledRect(x, y + h - 1, w, 1, b_outer);
            DrawFilledRect(x + w - 1, y, 1, h, b_outer);
        }

        void WindowRenderer::DrawRect3D(bounds_t bounds, Color tl, Color b_inner, Color b_outer) { DrawRect3D(bounds.X, bounds.Y, bounds.Width, bounds.Height, tl, b_inner, b_outer); }

        void WindowRenderer::DrawChar(int16_t x, int16_t y, char c, Color fg, Graphics::Font font)
        {
            if (c == 0) { return; }
            uint32_t p = font.GetHeight() * c;
            for (size_t cy = 0; cy < font.GetHeight(); cy++)
            {
                for (size_t cx = 0; cx < font.GetWidth(); cx++)
                {
                    if (Graphics::ConvertByteToBitAddress(font.GetData()[p + cy], cx + 1))
                    { DrawPixel(x + (font.GetWidth() - cx), y + cy, fg); }
                }
            }
        }

        void WindowRenderer::DrawChar(point_t pos, char c, Color fg, Graphics::Font font) { DrawChar(pos.X, pos.Y, c, fg, font); }

        void WindowRenderer::DrawChar(int16_t x, int16_t y, char c, Color fg, Color bg, Graphics::Font font)
        {
            uint32_t p = font.GetHeight() * c;
            for (size_t cy = 0; cy < font.GetHeight(); cy++)
            {
                for (size_t cx = 0; cx < font.GetWidth(); cx++)
                {
                    if (Graphics::ConvertByteToBitAddress(font.GetData()[p + cy], cx + 1))
                    { DrawPixel(x + (font.GetWidth() - cx), y + cy, fg); }
                    else { DrawPixel(x + (font.GetWidth() - cx), y + cy, bg); }
                }
            }
        }

        void WindowRenderer::DrawChar(point_t pos, char c, Color fg, Color bg, Graphics::Font font) { DrawChar(pos.X, pos.Y, c, fg, bg, font); }

        void WindowRenderer::DrawString(int16_t x, int16_t y, char* text, Color fg, Graphics::Font font)
        {
            if (text == nullptr) { return; }
            if (strlen(text) == 0) { return; }
            int32_t dx = x, dy = y;
            for (int32_t i = 0; i < strlen(text); i++)
            {
                if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
                else if (text[i] >= 32 && text[i] <= 126)
                {
                    DrawChar(dx, dy, text[i], fg, font);
                    dx += font.GetWidth() + font.GetHorizontalSpacing();
                }
            }
        }

        void WindowRenderer::DrawString(point_t pos, char* text, Color fg, Graphics::Font font) { DrawString(pos.X, pos.Y, text, fg, font); }

        void WindowRenderer::DrawString(int16_t x, int16_t y, char* text, Color fg, Color bg, Graphics::Font font)
        {
            if (text == nullptr) { return; }
            if (strlen(text) == 0) { return; }
            int32_t dx = x, dy = y;
            for (int32_t i = 0; i < strlen(text); i++)
            {
                if (text[i] == '\n') { dx = x; dy += font.GetHeight() + font.GetVerticalSpacing(); }
                else if (text[i] >= 32 && text[i] <= 126)
                {
                    DrawChar(dx, dy, text[i], fg, bg, font);
                    dx += font.GetWidth() + font.GetHorizontalSpacing();
                }
            }
        }
        
        void WindowRenderer::DrawString(point_t pos, char* text, Color fg, Color bg, Graphics::Font font) { DrawString(pos.X, pos.Y, text, fg, bg, font); }

        void WindowRenderer::DrawArray(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t* data)
        {
            for (int32_t yy = 0; yy < h; yy++)
            {
                for (int32_t xx = 0; xx < w; xx++)
                {
                    DrawPixel(x + xx, y + yy, Graphics::PackedValueToRGB(data[xx + (yy * w)]));
                }
            }
        }

        void WindowRenderer::DrawArray(int16_t x, int16_t y, int16_t w, int16_t h, Color trans_key, uint32_t* data)
        {
            uint32_t offset = 0;
            for (int32_t yy = 0; yy < h; yy++)
            {
                for (int32_t xx = 0; xx < w; xx++)
                {
                    offset = xx + (yy * w);
                    if (!Graphics::IsColorEqual(Graphics::PackedValueToRGB(data[offset]), trans_key)) 
                    { DrawPixel(x + xx, y + yy, Graphics::PackedValueToRGB(data[offset])); }
                }
            }
        }
    }
}