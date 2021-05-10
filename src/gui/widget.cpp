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
                else
                {
                    widget->MSFlags.Down = false;
                }
            }
            // mouse not hover
            else
            {
                widget->MSFlags.Hover = false;
            }
        }

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

        void Button::SetText(char* text)
        {
            if (Base.Text != nullptr) { delete Base.Text; }
            Base.Text = (char*)mem_alloc(strlen(text) + 1);
            strcpy(text, Base.Text);
            stradd(Base.Text, '\0');
        }
    }
}