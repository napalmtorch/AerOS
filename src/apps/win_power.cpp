#include <lib/types.h>
#include <hardware/acpi.hpp>
#include "apps/win_power.hpp"
#include <graphics/canvas.hpp>
#include <graphics/colors.hpp>
#include <graphics/font.hpp>
#include <gui/widget.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        WinPower::WinPower()
        {

        }

        WinPower::WinPower(int32_t x, int32_t y) : GUI::Window(x, y, 320, 200, "Shutdown Menu")
        {
          GUI::Button button;
          GUI::VisualStyle buttonStyle;
          Graphics::Canvas::DrawString(20,20,"Select your Option",Graphics::Colors::Black,Graphics::FONT_8x16_CONSOLAS); 
          Graphics::Canvas::DrawString(button.Bounds->X + 14, button.Bounds->Y + 5, "AerOS", buttonStyle.Colors[1], Graphics::FONT_8x8_PHEONIX); 
          
        }

        void WinPower::Update()
        {
            GUI::Window::Update();
        }

        void WinPower::Draw()
        {
            GUI::Window::Draw();
        }
    }
}