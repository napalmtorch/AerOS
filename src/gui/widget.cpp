#include <gui/widget.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace GUI
    {
        void Widget::Create()
        {
            Bounds->X = 0;
            Bounds->Y = 0;
            Bounds->Width = 64;
            Bounds->Height = 64;
            ScreenPosition->X = 0;
            ScreenPosition->Y = 0;

            Type = WIDGET_TYPE_CONTAINER;
            Text = nullptr;

            Visible = true;
            Enabled = true;
            Active = true;
            
            MSFlags = new MouseEventFlags();

            Style = &ButtonStyle;
        }

        void Widget::Update()
        {
            // get mouse position
            uint32_t mx = KernelIO::Mouse.GetX(), my = KernelIO::Mouse.GetY();

            // mouse is inside object bounds
            if (mx >= Bounds->X && my >= Bounds->Y && mx < Bounds->X + Bounds->Width && my < Bounds->Y + Bounds->Height)
            {
                if (!MSFlags->Hover) { if (OnMouseEnter != nullptr) { OnMouseEnter(); } }
                MSFlags->Hover = true;
                if (KernelIO::Mouse.IsLeftPressed() == HAL::ButtonState::Pressed) 
                {
                    MSFlags->Down = true; 
                    if (OnMouseDown != nullptr) { OnMouseDown(); }
                    if (!MSFlags->Clicked)
                    {
                        if (OnClick != nullptr) { OnClick(); }
                        MSFlags->Clicked = true;
                    }
                }
                else { MSFlags->Down = false; MSFlags->Clicked = false; }
            }
            else
            {
                if (MSFlags->Hover) { if (OnMouseLeave != nullptr) { OnMouseLeave(); } }
                MSFlags->Hover = false;
                MSFlags->Down = false;
                MSFlags->Clicked = false;
                MSFlags->Up = true;
            }
        }

        void Widget::Draw()
        {
            
        }

        bounds_t* Widget::GetBounds() { return Bounds; }
        point_t* Widget::GetScreenPosition() { return ScreenPosition; }
        WIDGET_TYPE Widget::GetType() { return Type; }
        char* Widget::GetText() { return Text->ToCharArray(); }
        bool Widget::IsVisible() { return Visible; }
        bool Widget::IsEnabled() { return Enabled; }
        bool Widget::IsActive() { return Active; }
        MouseEventFlags* Widget::GetMouseFlags() { return MSFlags; }
        VisualStyle* Widget::GetStyle() { return Style; }

        void Button::Create()
        {
            Widget::Create();
        }

        void Button::Create(uint32_t x, uint32_t y, char* text)
        {
            Widget::Create();
            Bounds->X = x;
            Bounds->Y = y;
            Bounds->Width = 92;
            Bounds->Height = 22;
            Text = new String(text);
        }

        void Button::Update()
        {
            // update base
            Widget::Update();
        }

        void Button::Draw()
        {
            // draw base
            Widget::Draw();

            // draw background
            KernelIO::XServer.FullCanvas.DrawFilledRectangle(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[0]);

            // draw border
            if (MSFlags->Down)
            { KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[4], Style->Colors[3], Style->Colors[3]); }
            else { KernelIO::XServer.FullCanvas.DrawRectangle3D(Bounds->X, Bounds->Y, Bounds->Width, Bounds->Height, Style->Colors[2], Style->Colors[3], Style->Colors[4]); }

            // draw text
            if (Text != nullptr)
            {
                uint32_t txt_w = Text->GetLength() * 8;
                uint32_t dx = Bounds->X + (Bounds->Width / 2) - (txt_w / 2);
                uint32_t dy = Bounds->Y + (Bounds->Height / 2) - 4;
                if (MSFlags->Down) { dx += 1; dy += 1; }
                KernelIO::XServer.FullCanvas.DrawString(dx, dy, Text->ToCharArray(), Style->Colors[1], Graphics::FONT_8x8);
            }
        }
    }
}