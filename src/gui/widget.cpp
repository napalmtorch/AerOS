#include <gui/widget.hpp>
#include <core/kernel.hpp>
#include <gui/window.hpp>

namespace System
{
    GUI::Window* GetParent(void* parent) { return ((GUI::Window*)parent); }

    namespace GUI
    {
        Widget::Widget()
        {

        }

        Widget::Widget(int16_t x, int16_t y, int16_t w, int16_t h, WidgetType type)
        {
            SetBounds(x, y, w, h);
            CopyStyle(&WindowStyle, &Style);

            Flags.Active = true;
            Flags.Enabled = true;
            Flags.Visible = true;
            Parent = nullptr;
        }

        Widget::Widget(int16_t x, int16_t y, int16_t w, int16_t h, WidgetType type, void* parent)
        {
            SetBounds(x, y, w, h);
            CopyStyle(&WindowStyle, &Style);

            Flags.Active = true;
            Flags.Enabled = true;
            Flags.Visible = true;
            Parent = parent;
        }

        Widget::~Widget() { }

        void Widget::Update()
        {
            if (Flags.Active)
            {
                // get mouse pos
                int32_t mx = KernelIO::Mouse.GetX();
                int32_t my = KernelIO::Mouse.GetY();

                // standalone
                if (Parent == nullptr)
                {
                    if (bounds_contains(&Bounds, mx, my))
                    {
                        MouseFlags.Hover = true;
                        OnMouseHover();
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed && !MouseFlags.Clicked) 
                        { 
                            //OnMouseDown();
                            MouseFlags.Down = true; 
                            MouseFlags.Clicked = true;
                            MouseFlags.Unclicked = false;
                        }
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released && !MouseFlags.Unclicked) 
                        {
                            MouseFlags.Down = false; 
                            if (MouseFlags.Clicked)
                            {
                                //OnClick();
                                //OnMouseUp();
                                MouseFlags.Unclicked = true;
                                MouseFlags.Clicked = false; 
                            }
                        }
                    }
                    else { MouseFlags.Hover = false; MouseFlags.Down = false; MouseFlags.Clicked = false; MouseFlags.Up = true; MouseFlags.Unclicked = false; }
                }
                // child of window
                else
                {
                    if (rect_contains(GetParent(Parent)->Bounds.X + Bounds.X, GetParent(Parent)->Bounds.Y + Bounds.Y, Bounds.Width, Bounds.Height, mx, my))
                    {
                        MouseFlags.Hover = true;
                        OnMouseHover();
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed && !MouseFlags.Clicked) 
                        { 
                            OnMouseDown();
                            MouseFlags.Clicked = true;
                            MouseFlags.Unclicked = false;
                        }
                        if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Released && !MouseFlags.Unclicked) 
                        {                      
                            if (MouseFlags.Clicked)
                            {
                                OnClick();
                                OnMouseUp();
                                MouseFlags.Unclicked = true;
                                MouseFlags.Clicked = false; 
                            }
                        }
                    }
                    else { MouseFlags.Hover = false; MouseFlags.Down = false; MouseFlags.Clicked = false; MouseFlags.Up = true; MouseFlags.Unclicked = false; }
                }
            }
        }

        void Widget::Draw()
        {
            
        }

        void Widget::OnClick()
        {
            if (Click != nullptr) { Click(Parent); }
            GetParent(Parent)->SendRefresh();
        }

        void Widget::OnMouseDown()
        {
            MouseFlags.Down = true;
            if (MouseDown != nullptr) { MouseDown(Parent); }
            GetParent(Parent)->SendRefresh();
        }

        void Widget::OnMouseUp()
        {
            MouseFlags.Down = false;
            if (MouseUp != nullptr) { MouseUp(Parent); }
            GetParent(Parent)->SendRefresh();
        }

        void Widget::OnMouseHover()
        {

        }

        void Widget::OnMouseLeave()
        {

        }

        void Widget::SetPosition(int16_t x, int16_t y) { Bounds.X = x; Bounds.Y = y; }
        void Widget::SetSize(int16_t w, int16_t h) { Bounds.Width = w; Bounds.Height = h; }
        void Widget::SetBounds(int16_t x, int16_t y, int16_t w, int16_t h) { SetPosition(x, y); SetSize(w, h); }
        void Widget::SetText(char* text)
        {
            if (Text != nullptr) { mem_free(Text); }
            Text = new char[strlen(text) + 1];
            strcpy(text, Text);
        }

        Button::Button() : Widget() { }

        Button::Button(int16_t x, int16_t y, char* text) : Widget(x, y, 92, 22, WidgetType::Button)
        {
            SetText(text);
        }

        Button::Button(int16_t x, int16_t y, char* text, void* parent) : Widget(x, y, 92, 22, WidgetType::Button, parent)
        {
            SetText(text);
        }

        Button::~Button()
        {

        }

        void Button::Update()
        {
            Widget::Update();
        }

        void Button::Draw()
        {
            Widget::Draw();

            if (Parent == nullptr)
            {
                // draw background
                Graphics::Canvas::DrawFilledRectangle(Bounds, Style.Colors[0]);
            }
            else
            {
                // draw background
                GetParent(Parent)->Renderer->DrawFilledRect(Bounds, Style.Colors[0]);

                // draw border
                if (MouseFlags.Down) { GetParent(Parent)->Renderer->DrawRect3D(Bounds, Style.Colors[4], Style.Colors[3], Style.Colors[2]); }
                else { GetParent(Parent)->Renderer->DrawRect3D(Bounds, Style.Colors[2], Style.Colors[3], Style.Colors[4]); }

                // draw text
                if (Text != nullptr)
                {
                    if (strlen(Text) > 0)
                    {
                        int tw = strlen(Text) * (Style.Font->GetWidth() + Style.Font->GetHorizontalSpacing());
                        int th = Style.Font->GetHeight() + Style.Font->GetVerticalSpacing();
                        int dx = Bounds.X + (Bounds.Width / 2) - (tw / 2);
                        int dy = Bounds.Y + (Bounds.Height / 2) - (th / 2);
                        GetParent(Parent)->Renderer->DrawString(dx, dy, Text, Style.Colors[1], (*Style.Font));
                    }
                }
            }
        }

        void Button::OnClick()
        {
            Widget::OnClick();
        }

        void Button::OnMouseDown()
        {
            Widget::OnMouseDown();
        }

        void Button::OnMouseUp()
        {
            Widget::OnMouseUp();
        }

        void Button::OnMouseHover()
        {
            Widget::OnMouseHover();
        }

        void Button::OnMouseLeave()
        {
            Widget::OnMouseLeave();
        }
    }
}