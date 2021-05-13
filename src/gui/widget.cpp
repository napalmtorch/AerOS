#include <gui/widget.hpp>
#include <core/kernel.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace GUI
    {
        uint8_t TITLEBAR_ICON_CLOSE[8 * 7] = 
        {
            1, 1, 0, 0, 0, 0, 1, 1,
            0, 1, 1, 0, 0, 1, 1, 0,
            0, 0, 1, 1, 1, 1, 0, 0,
            0, 0, 0, 1, 1, 0, 0, 0,
            0, 0, 1, 1, 1, 1, 0, 0,
            0, 1, 1, 0, 0, 1, 1, 0,
            1, 1, 0, 0, 0, 0, 1, 1,
        };

        uint8_t TITLEBAR_ICON_MAX[8 * 7] = 
        {
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
        };

        uint8_t TITLEBAR_ICON_MIN[8 * 7] = 
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1,
        };

        uint8_t TITLEBAR_ICON_RES[8 * 7] = 
        {
            0, 0, 1, 1, 1, 1, 1, 1,
            0, 0, 1, 1, 1, 1, 1, 1,
            0, 1, 1, 1, 1, 1, 0, 1,
            0, 1, 1, 1, 1, 1, 0, 1,
            0, 1, 0, 0, 0, 1, 0, 1,
            0, 1, 0, 0, 0, 1, 1, 1,
            0, 1, 1, 1, 1, 1, 0, 0,
        };

        VisualStyle* CopyStyle(VisualStyle* src)
        {
            VisualStyle* style = (VisualStyle*)mem_alloc(sizeof(VisualStyle));

            // name
            strcpy(src->Name, style->Name);

            // flags
            style->BorderSize = src->BorderSize;
            style->BorderStyle = src->BorderStyle;
            style->Font = src->Font;

            // colors
            for (size_t i = 0; i < 8; i++) { style->Colors[i] = src->Colors[i]; }

            return style;
        }

        // check widget default events
        bool m_down = false;
        void CheckWidgetEvents(Widget* widget)
        {
            if (IsWidgetNull(widget)) { return; }

            int32_t mx = KernelIO::Mouse.GetX();
            int32_t my = KernelIO::Mouse.GetY();

            // mouse is hovering over widget
            if (bounds_contains(widget->Bounds, mx, my))
            {
                widget->MouseFlags->Hover = true;

                // left mouse button is down
                if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed)
                {
                    widget->MouseFlags->Down = true;
                    widget->MouseFlags->Up = false;
                    m_down = false;
                }
                // left mouse button is up
                else
                {
                    widget->MouseFlags->Down = false;
                    widget->MouseFlags->Clicked = false;
                    if (!m_down) { if (widget->OnClick != nullptr) { widget->OnClick(); } m_down = true; }
                }
            }
            // mouse is NOT hovering over widget
            else
            {
                widget->MouseFlags->Hover = false;   
                widget->MouseFlags->Down = false;
                widget->MouseFlags->Clicked = false;
                m_down = false;
            }
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
            widget->Text = (char*)mem_alloc(strlen(text) + 1);
            for (size_t i = 0; i < strlen(text); i++) { widget->Text[i] = text[i]; }
            widget->Text[strlen(text)] = '\0';
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

            // draw 3d border
            if (Style->BorderStyle == BORDER_STYLE_3D)
            { 
                if (MouseFlags->Down) { KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[4], Style->Colors[2], Style->Colors[2]); }
                else { KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[2], Style->Colors[3], Style->Colors[4]); }
            }
            // draw fixed border
            else if (Style->BorderStyle == BORDER_STYLE_FIXED)
            { KernelIO::XServer.FullCanvas.DrawRectangle((*Bounds), 1, Style->Colors[4]); }

            // draw text
            if (Text != nullptr && strlen(Text) > 0 && !streql(Text, "\0"))
            {
                uint32_t str_w = strlen(Text) * (Style->Font->GetWidth() + Style->Font->GetHorizontalSpacing());
                uint32_t sx = Bounds->X + (Bounds->Width / 2) - (str_w / 2);
                uint32_t sy = Bounds->Y + (Bounds->Height / 2) - ((Style->Font->GetHeight() + Style->Font->GetVerticalSpacing()) / 2);
                if (MouseFlags->Down) { sx += 2; sy += 2; }
                KernelIO::XServer.FullCanvas.DrawString(sx, sy, Text, Style->Colors[1], Graphics::FONT_8x8_SERIF);
            }
        }

        // -------------------------------------------------- TITLE BAR ---------------------------------------------------- //

        TitleBar::TitleBar()
        {

        }

        TitleBar::TitleBar(Window* window) : Widget(0, 0, 0, 0, WIDGET_TYPE_SYSTEM)
        {
            // set parent
            Parent = window;

            // create buttons
            BtnClose = new Button(Bounds->X + Bounds->Width - 16,       Bounds->Y + 2, "");
            BtnMax   = new Button(BtnClose->Bounds->X - 16, Bounds->Y + 2, "");
            BtnMin   = new Button(BtnMax->Bounds->X - 16,   Bounds->Y + 2, "");
            BtnClose->SetSize(14, 12);
            BtnMax->SetSize(14, 12);
            BtnMin->SetSize(14, 12);
        }

        void TitleBar::Update()
        {
            Widget::Update();

            // update bounds
            Bounds->X = Parent->Bounds->X + 1;
            Bounds->Y = Parent->Bounds->Y + 1;
            Bounds->Width = Parent->Bounds->Width - 3;
            Bounds->Height = 16;

            // update buttons
            BtnClose->SetPosition(Bounds->X + Bounds->Width - 16, Bounds->Y + 2);
            BtnMax->SetPosition(BtnClose->Bounds->X - 16, Bounds->Y + 2);
            BtnMin->SetPosition(BtnMax->Bounds->X - 16, Bounds->Y + 2);
            BtnClose->Update();
            BtnMax->Update();
            BtnMin->Update();
        }

        void TitleBar::Draw()
        {
            Widget::Draw();

            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle((*Bounds), Parent->Style->Colors[5]);

            // draw text
            if (Parent->Title != nullptr)
            {
                KernelIO::XServer.FullCanvas.DrawString(Bounds->X + 3, Bounds->Y + 4, Parent->Title, Parent->Style->Colors[6], Graphics::FONT_8x8);
            }

            // draw buttons
            BtnClose->Draw();
            BtnMax->Draw();
            BtnMin->Draw();

            // draw button icons
            KernelIO::XServer.FullCanvas.DrawFlatArray(BtnClose->Bounds->X + 3, BtnClose->Bounds->Y + 2, 8, 7, TITLEBAR_ICON_CLOSE, BtnClose->Style->Colors[1]);
            if (!Parent->Flags->Maximized) { KernelIO::XServer.FullCanvas.DrawFlatArray(BtnMax->Bounds->X + 3,   BtnMax->Bounds->Y + 2,   8, 7, TITLEBAR_ICON_MAX, BtnMax->Style->Colors[1]); }
            else { KernelIO::XServer.FullCanvas.DrawFlatArray(BtnMax->Bounds->X + 3,   BtnMax->Bounds->Y + 2,   8, 7, TITLEBAR_ICON_RES, BtnMax->Style->Colors[1]); }
            KernelIO::XServer.FullCanvas.DrawFlatArray(BtnMin->Bounds->X + 3,   BtnMin->Bounds->Y + 2,   8, 7, TITLEBAR_ICON_MIN, BtnMin->Style->Colors[1]);
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

            // old bounds
            OldBounds = new bounds_t();

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

            // close button clicked
            if (TBar->BtnClose->MouseFlags->Down && !TBar->BtnClose->MouseFlags->Clicked)
            {
                KernelIO::XServer.WindowMgr.Close(this);
                return;
                TBar->BtnClose->MouseFlags->Clicked = true;
            }

            // maximize button clicked
            if (TBar->BtnMax->MouseFlags->Down && !TBar->BtnMax->MouseFlags->Clicked)
            {
                Flags->Maximized = !Flags->Maximized;

                if (Flags->Maximized)
                {
                    OldBounds->X = Bounds->X;
                    OldBounds->Y = Bounds->Y;
                    OldBounds->Width = Bounds->Width;
                    OldBounds->Height = Bounds->Height;
                    Bounds->Width = KernelIO::VESA.GetWidth();
                    Bounds->Height = KernelIO::VESA.GetHeight() - 24;
                    Bounds->X = 0;
                    Bounds->Y = 0;
                }
                else
                {
                    Bounds->X = OldBounds->X;
                    Bounds->Y = OldBounds->Y;
                    Bounds->Width = OldBounds->Width;
                    Bounds->Height = OldBounds->Height;
                }

                TBar->BtnMax->MouseFlags->Clicked = true;
            }

            // maximize button clicked

            // get mouse position
            int32_t mx = KernelIO::Mouse.GetX();
            int32_t my = KernelIO::Mouse.GetY();

            // check for movement
            if (KernelIO::XServer.WindowMgr.ActiveWindow == this)
            {
                if (bounds_contains(TBar->Bounds, mx, my) && !TBar->BtnClose->MouseFlags->Down && !TBar->BtnMax->MouseFlags->Down && !TBar->BtnMin->MouseFlags->Down)
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
            }

            // check if able to draw
            if (Flags->Moving || Flags->Resizing || Flags->Minimized) { Flags->CanDraw = false; } else { Flags->CanDraw = true; }
        }

        void Window::Draw()
        {
            if (!Flags->Moving)
            {
                // draw background
                KernelIO::XServer.FullCanvas.DrawFilledRectangle((*ClientBounds), Style->Colors[0]);

                // draw border
                KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[2], Style->Colors[3], Style->Colors[4]);

                // draw title bar
                TBar->Draw();
            }
            else
            {
                // draw border
                KernelIO::XServer.FullCanvas.DrawRectangle((*Bounds), 3, Style->Colors[4]);
                KernelIO::XServer.FullCanvas.DrawRectangle((*Bounds), 1, Style->Colors[2]);
            }
        }
    }
}