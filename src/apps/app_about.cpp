#include <apps/app_about.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        WinAbout::WinAbout() : GUI::Window()
        {

        }

        WinAbout::WinAbout(int32_t x, int32_t y) : GUI::Window(x, y, 480, 240, "About", "about")
        {

        }

        WinAbout::~WinAbout()
        {

        }

        void WinAbout::OnLoad()
        {
            GUI::Window::OnLoad();
        }

        void WinAbout::Update()
        {
            GUI::Window::Update();
        }

        void WinAbout::Draw()
        {
            GUI::Window::Draw();

            if (Flags.CanDraw)
            {
                int32_t dx = ClientBounds.X + 4, dy = ClientBounds.Y + 4;
                Graphics::Canvas::DrawString(dx, dy, "About AerOS", Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12;
                dy += 12;
                char about[64]{'\0'};
                strcat(about,"Version: 0.1");
                Graphics::Canvas::DrawString(dx, dy, about, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 24;
                Graphics::Canvas::DrawString(dx, dy, "Developed by", Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 16;
                Graphics::Canvas::DrawString(dx, dy, "Will Hall", Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12;
                Graphics::Canvas::DrawString(dx, dy, "Kevin Meerts", Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12;
                Graphics::Canvas::DrawString(dx, dy, "Nicola Leone Ciardi", Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 24;
                Graphics::Canvas::DrawString(dx, dy, "https://github.com/napalmtorch/AerOS", Graphics::Colors::Cyan, Graphics::FONT_8x8_SERIF);
                
            }
        }
    }
}