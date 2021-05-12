#include <gui/widget.hpp>
#include <core/kernel.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace GUI
    {
        // check widget default events
        void CheckWidgetEvents(Widget* widget)
        {

        }

        // check if widget is null
        bool IsWidgetNull(Widget* widget)
        {
            if (widget == nullptr) { KernelIO::ThrowError("Tried to modify null widget"); return true; }
            else { return false; }
        }

        // set widget dimensions
        void SetWidgetPosition(Widget* widget, int32_t x, int32_t y)
        {
            if (IsWidgetNull(widget)) { return; }
            widget->Bounds->X = x;
            widget->Bounds->Y = y;
        }

        // set widget size
        void SetWidgetSize(Widget* widget, int32_t w, int32_t h)
        {
            if (IsWidgetNull(widget)) { return; }
            widget->Bounds->Width = w;
            widget->Bounds->Height = h;
        }

        // set widget bounds
        void SetWidgetBounds(Widget* widget, int32_t x, int32_t y, int32_t w, int32_t h)
        {
            if (IsWidgetNull(widget)) { return; }
            widget->Bounds->X = x;
            widget->Bounds->Y = y;
            widget->Bounds->Width = w;
            widget->Bounds->Height = h;
        }
        
        // set widget visible flag
        void SetWidgetVisible(Widget* widget, bool visible)
        {
            if (IsWidgetNull(widget)) { return; }
            widget->Flags->Visible = visible;
        }

        // set widget enable flag
        void SetWidgetEnabled(Widget* widget, bool enabled)
        {
            if (IsWidgetNull(widget)) { return; }
            widget->Flags->Enabled = enabled;
        }

        // set widget text
        void SetWidgetText(Widget* widget, char* text)
        {
            if (IsWidgetNull(widget)) { return; }
            if (widget->Text != nullptr) { delete widget->Text; }
            widget->Text = (char*)mem_alloc(strlen(text));
            strcpy(text, widget->Text);
        }

        // -------------------------------------------------- WIDGET BASE -------------------------------------------------- //

        Widget::Widget() { }

        Widget::Widget(int32_t x, int32_t y, int32_t w, int32_t h, WIDGET_TYPE type)
        {
            // create bounds
            Bounds = new bounds_t();
            Bounds->X = x;
            Bounds->Y = y;
            Bounds->Width = w;
            Bounds->Height = h;

            // create flags
            Type = type;
            Flags = new WidgetFlags();
            Flags->Visible = true;
            Flags->Enabled = true;
            Flags->Active = true;
            MouseFlags = new MouseEventFlags();

            // set visual style
            Style = &WindowStyle;
        }

        void Widget::Update()
        {

        }

        void Widget::Draw()
        {
            
        }

        void Widget::SetPosition(int32_t x, int32_t y) { SetWidgetPosition(this, x, y); }
        void Widget::SetPosition(point_t* pos) { SetWidgetPosition(this, pos->X, pos->Y); }
        void Widget::SetSize(int32_t w, int32_t h) { SetWidgetSize(this, w, h); }
        void Widget::SetSize(point_t* size) { SetWidgetSize(this, size->X, size->Y); }

        void Widget::SetBounds(int32_t x, int32_t y, int32_t w, int32_t h) { SetWidgetBounds(this, x, y, w, h); }
        void Widget::SetBounds(point_t* pos, point_t* size) { SetWidgetBounds(this, pos->X, pos->Y, size->X, size->Y); }
        void Widget::SetBounds(bounds_t* bounds) { SetWidgetBounds(this, bounds->X, bounds->Y, bounds->Width, bounds->Height); }

        void Widget::SetText(char* text) { SetWidgetText(this, text); }

        // -------------------------------------------------- BUTTON   ----------------------------------------------------- //

        Button::Button()
        {

        }

        Button::Button(int32_t x, int32_t y, char* text) : Widget(x, y, 92, 22, WIDGET_TYPE_BUTTON)
        {
            // set style
            Style = &ButtonStyle;

            // set text
            SetText(text);
        }

        void Button::Update()
        {
            Widget::Update();
            CheckWidgetEvents(this);
        }

        void Button::Draw()
        {
            Widget::Draw();

            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle((*Bounds), Style->Colors[0]);

            // draw border
            KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[2], Style->Colors[3], Style->Colors[4]);

            // draw text
            if (Text != nullptr)
            {
                uint32_t str_w = strlen(Text) * (Style->Font->GetWidth() + Style->Font->GetHorizontalSpacing());
                uint32_t sx = Bounds->X + (Bounds->Width / 2) - (str_w / 2);
                uint32_t sy = Bounds->Y + (Bounds->Height / 2) - ((Style->Font->GetHeight() + Style->Font->GetVerticalSpacing()) / 2);
                KernelIO::XServer.FullCanvas.DrawString(sx, sy, Text, Style->Colors[1], Graphics::FONT_8x8_SERIF);
            }
        }

        // -------------------------------------------------- TITLE BAR ---------------------------------------------------- //

        TitleBar::TitleBar()
        {

        }

        TitleBar::TitleBar(Window* window) : Widget(0, 0, 0, 0, WIDGET_TYPE_SYSTEM)
        {
            Parent = window;
        }

        void TitleBar::Update()
        {
            Widget::Update();

            Bounds->X = Parent->Bounds->X + 1;
            Bounds->Y = Parent->Bounds->Y + 1;
            Bounds->Width = Parent->Bounds->Width - 3;
            Bounds->Height = 16;
        }

        void TitleBar::Draw()
        {
            Widget::Draw();

            KernelIO::XServer.FullCanvas.DrawFilledRectangle((*Bounds), Graphics::Colors::Blue);
        }

        // -------------------------------------------------- WINDOW BASE -------------------------------------------------- //

        Window::Window() { }

        Window::Window(int32_t x, int32_t y, int32_t w, int32_t h, char* title)
        {
            // create bounds
            Bounds = new bounds_t();
            Bounds->X = x;
            Bounds->Y = y;
            Bounds->Width = w;
            Bounds->Height = h;

            // create client bounds
            ClientBounds = new bounds_t();
            ClientBounds->X = x + 1;
            ClientBounds->Y = y + 17;
            ClientBounds->Width = w - 3;
            ClientBounds->Height = h - 19;

            // create flags
            Flags = new WindowFlags();
            Flags->Visible = true;
            Flags->CanDraw = true;

            // set visual style
            Style = &WindowStyle;

            // set title
            Title = (char*)mem_alloc(strlen(title));
            strcpy(title, Title);

            // create title bar
            TBar = new TitleBar(this);
            TBar->Parent = this;
        }

        bool move_click = false;
        int32_t mx_start, my_start;
        void Window::Update()
        {
            // update title bar
            TBar->Parent = this;
            TBar->Update();

            // update client bounds
            ClientBounds->X = Bounds->X + 1;
            ClientBounds->Y = Bounds->Y + 17;
            ClientBounds->Width = Bounds->Width - 3;
            ClientBounds->Height = Bounds->Height - 19;

            // get mouse position
            int32_t mx = KernelIO::Mouse.GetX();
            int32_t my = KernelIO::Mouse.GetY();

            if (bounds_contains(TBar->Bounds, mx, my))
            {
                if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                {
                    if (!move_click)
                    {
                        mx_start = mx - Bounds->X;
                        my_start = my - Bounds->Y;
                        move_click = true;
                    }
                    Flags->Moving = true;
                }
            }

            // relocate window
            int32_t new_x = Bounds->X;
            int32_t new_y = Bounds->Y;
            if (Flags->Moving)
            {
                new_x = mx - mx_start;
                new_y = my - my_start;
                Bounds->X = new_x;
                Bounds->Y = new_y;
            }

            // mouse release
            if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released)
            {
                Flags->Moving = false;
                move_click = false;
            }

            // check if able to draw
            if (Flags->Moving || Flags->Resizing || Flags->Minimized) { Flags->CanDraw = false; } else { Flags->CanDraw = true; }
        }

        void Window::Draw()
        {
            if (!Flags->Moving)
            {
                // draw background
                KernelIO::XServer.FullCanvas.DrawFilledRectangle((*Bounds), Style->Colors[0]);

                // draw border
                KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[2], Style->Colors[3], Style->Colors[4]);

                // draw title bar
                TBar->Draw();
            }
            else
            {
                // draw border
                KernelIO::XServer.FullCanvas.DrawRectangle((*Bounds), 2, Style->Colors[4]);
            }
        }

        // -------------------------------------------------- TEST APP -------------------------------------------------- //

        TestApp::TestApp()
        {

        }

        Button* btn;
        TestApp::TestApp(int32_t x, int32_t y) : Window(x, y, 320, 240, "Testing")
        {
            btn = new Button(ClientBounds->X + 4, ClientBounds->Y + 4, "BTN");
        }

        void TestApp::Update()
        {
            Window::Update();
            
            btn->Update();
            btn->SetPosition(ClientBounds->X + 4, ClientBounds->Y + 4);
        }

        void TestApp::Draw()
        {   
            Window::Draw();

            btn->Draw();
        }
    }
}