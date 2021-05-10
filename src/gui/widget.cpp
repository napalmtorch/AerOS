#include <gui/widget.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace GUI
    {
        // update widget default events
        void CheckWidgetEvents(Widget* widget)
        {
            uint32_t mx = KernelIO::Mouse.GetX();
            uint32_t my = KernelIO::Mouse.GetY();

            // mouse hover
            if (mx >= widget->Bounds.X && my >= widget->Bounds.Y && mx < widget->Bounds.X + widget->Bounds.Width && my < widget->Bounds.Y + widget->Bounds.Height)
            {
                widget->MSFlags.Hover = true;

                // mouse click
                if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                {
                    widget->MSFlags.Down = true;
                }
                // mouse not clicked
                else
                {
                    widget->MSFlags.Down = false;
                }
            }
            // mouse not hover
            else
            {
                widget->MSFlags.Hover = false;
                widget->MSFlags.Down = false;
                widget->MSFlags.Clicked = false;
                widget->MSFlags.Up = true;
            }
        }

        // set widget text
        void SetWidgetText(Widget* widget, char* text)
        {
            if (widget->Text != nullptr) { delete widget->Text; }
            widget->Text = (char*)mem_alloc(strlen(text) + 1);
            strcpy(text, widget->Text);
            stradd(widget->Text, '\0');
        }

        // -------------------------------------------------- WINDOW -------------------------------------------------- //

        Window::Window()
        {

        }

        Window::Window(uint32_t x, uint32_t y, uint32_t w, uint32_t h, char* title)
        {
            // default bounds
            Base.Bounds.X = x;
            Base.Bounds.Y = y;
            Base.Bounds.Width = w;
            Base.Bounds.Height = h;

            // default screen position
            Base.ScreenPosition.X = 0;
            Base.ScreenPosition.Y = 0;

            // flags
            Base.Type = WIDGET_TYPE_WINDOW;
            Base.Enabled = true;
            Base.Visible = true;
            Base.Active = true;
            Base.MSFlags = { false, false, false, false };
            
            // style
            Base.Style = &GUI::WindowStyle;

            // text
            Base.Text = nullptr;
            SetTitle(title);

            // title bar
            TBar = TitleBar(&Base);
        }

        void Window::Update()
        {
            TBar.Parent = &Base;
            TBar.Update();
        }

        void Window::Draw()
        {
            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(Base.Bounds, Base.Style->Colors[0]);

            // draw border
            KernelIO::XServer.FullCanvas.DrawRectangle3D(Base.Bounds.X, Base.Bounds.Y, Base.Bounds.Width, Base.Bounds.Height, Base.Style->Colors[2], Base.Style->Colors[3], Base.Style->Colors[4]);

            TBar.Draw();
        }

        void Window::SetTitle(char* title) { SetWidgetText(&Base, title); }

        // -------------------------------------------------- TITLE BAR ----------------------------------------------- //

        TitleBar::TitleBar()
        {
            
        }
        
        TitleBar::TitleBar(Widget* parent)
        {
            Parent = (Widget*)parent;

            // close button
            CloseBtn = Button(Parent->Bounds.X + Parent->Bounds.Width - 18, Parent->Bounds.Y + 3, "");
            CloseBtn.Base.Bounds.Width = 14;
            CloseBtn.Base.Bounds.Height = 12;

            // maximize button
            MaxBtn = Button(CloseBtn.Base.Bounds.X - 16, CloseBtn.Base.Bounds.Y, "");
            MaxBtn.Base.Bounds.Width = 14;
            MaxBtn.Base.Bounds.Height = 12;

            // minimize button
            MinBtn = Button(MaxBtn.Base.Bounds.X - 16, CloseBtn.Base.Bounds.Y, "");
            MinBtn.Base.Bounds.Width = 14;
            MinBtn.Base.Bounds.Height = 12;
        }

        void TitleBar::Update()
        {
            CloseBtn.Update();
            MaxBtn.Update();
            MinBtn.Update();
        }

        void TitleBar::Draw()
        {
            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(Parent->Bounds.X + 1, Parent->Bounds.Y + 1, Parent->Bounds.Width - 3, 16, Parent->Style->Colors[5]);

            // draw window title
            if (Parent->Text != nullptr)
            {
                KernelIO::XServer.FullCanvas.DrawString(Parent->Bounds.X + 4, Parent->Bounds.Y + 5, Parent->Text, Parent->Style->Colors[6], Graphics::FONT_8x8);
            }

            CloseBtn.Draw();
            MaxBtn.Draw();
            MinBtn.Draw();
            
            // draw close and maximize icon
            for (size_t yy = 0; yy < 7; yy++)
            {
                for (size_t xx = 0; xx < 8; xx++)
                {
                    // close icon
                    if (GUI::TitleBarIcon_Exit[xx + (yy * 8)] == 1)
                    { KernelIO::XServer.FullCanvas.DrawPixel(CloseBtn.Base.Bounds.X + xx + 3, CloseBtn.Base.Bounds.Y + yy + 2, Parent->Style->Colors[1]); }

                    // max icon
                    if (GUI::TitleBarIcon_Max[xx + (yy * 8)] == 1)
                    { KernelIO::XServer.FullCanvas.DrawPixel(MaxBtn.Base.Bounds.X + xx + 3, MaxBtn.Base.Bounds.Y + yy + 2, Parent->Style->Colors[1]); }
                }
            }

            // draw minimize icon
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(MinBtn.Base.Bounds.X + 3, MinBtn.Base.Bounds.Y + 7, MinBtn.Base.Bounds.Width - 7, 2, Parent->Style->Colors[1]);
        }

        // -------------------------------------------------- BUTTON -------------------------------------------------- // 

        // button constructor - default
        Button::Button()
        {

        }

        // button constructor - specified
        Button::Button(uint32_t x, uint32_t y, char* text)
        {
            // default bounds
            Base.Bounds.X = x;
            Base.Bounds.Y = y;
            Base.Bounds.Width = 92;
            Base.Bounds.Height = 22;

            // default screen position
            Base.ScreenPosition.X = 0;
            Base.ScreenPosition.Y = 0;

            // flags
            Base.Type = WIDGET_TYPE_BUTTON;
            Base.Enabled = true;
            Base.Visible = true;
            Base.Active = true;
            Base.MSFlags = { false, false, false, false };
            
            // style
            Base.Style = &GUI::ButtonStyle;

            // text
            Base.Text = nullptr;
            SetText(text);
        }

        // button - on update
        void Button::Update()
        {
            // check events
            CheckWidgetEvents(&Base);
        }

        // button - on draw
        char temp[8];
        void Button::Draw()
        {
            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(Base.Bounds, Base.Style->Colors[0]);
        
            // draw border
            if (Base.MSFlags.Down) { KernelIO::XServer.FullCanvas.DrawRectangle3D(Base.Bounds.X, Base.Bounds.Y, Base.Bounds.Width, Base.Bounds.Height, Base.Style->Colors[4], Base.Style->Colors[2], Base.Style->Colors[2]); }
            else { KernelIO::XServer.FullCanvas.DrawRectangle3D(Base.Bounds.X, Base.Bounds.Y, Base.Bounds.Width, Base.Bounds.Height, Base.Style->Colors[2], Base.Style->Colors[3], Base.Style->Colors[4]); }

            // draw text
            if (Base.Text != nullptr)
            {
                uint32_t txt_w = strlen(Base.Text) * (Base.Style->Font->GetWidth() + Base.Style->Font->GetHorizontalSpacing());
                uint32_t txt_h = Base.Style->Font->GetHeight() + Base.Style->Font->GetVerticalSpacing();
                uint32_t dx = Base.Bounds.X + (Base.Bounds.Width / 2) - (txt_w / 2);
                uint32_t dy = Base.Bounds.Y + (Base.Bounds.Height / 2) - (txt_h / 2);
                if (Base.MSFlags.Down) { dx += 2; dy += 2; }
                KernelIO::XServer.FullCanvas.DrawString(dx, dy, Base.Text, Base.Style->Colors[1], (*Base.Style->Font));
            }
        }

        // set text
        void Button::SetText(char* text) { SetWidgetText(&Base, text); }
    }
}