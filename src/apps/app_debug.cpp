#include <apps/app_debug.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        WinDebug::WinDebug() : GUI::Window()
        {

        }

        WinDebug::WinDebug(int32_t x, int32_t y) : GUI::Window(x, y, 320, 240, "Debug Info", "debuginfo")
        {

        }

        WinDebug::~WinDebug()
        {

        }

        void WinDebug::OnLoad()
        {
            GUI::Window::OnLoad();
        }

        void WinDebug::Update()
        {
            GUI::Window::Update();
        }

        void WinDebug::Draw()
        {
            GUI::Window::Draw();

            if (Flags.CanDraw)
            {
                char temp[64];
                char num[16];
                temp[0] = '\0';
                num[0] = '\0';
                int32_t dx = ClientBounds.X + 4, dy = ClientBounds.Y + 4;

                // draw framerate
                strcat(temp, "FPS: ");
                strdec(KernelIO::XServer.GetFPS(), num);
                strcat(temp, num);
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

                // draw thread count
                strcat(temp, "Threads: ");
                strdec(KernelIO::TaskManager.GetThreadCount(), num);
                strcat(temp, num);
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

                // draw window count
                strcat(temp, "Windows: ");
                strdec(KernelIO::WindowMgr.GetWindowCount(), num);
                strcat(temp, num);
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

                dy += 12;

                // draw used memory
                strcat(temp, "Memory Used:  ");
                strdec(mem_get_used() / 1024 / 1024, num);
                strcat(temp, num);
                strcat(temp, " MB");
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

                // draw used memory in bytes
                strcat(temp, "              ");
                strdec(mem_get_used(), num);
                strcat(temp, num);
                strcat(temp, " bytes");
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

                // draw total memory
                strcat(temp, "Memory Total: ");
                strdec(mem_get_total_usable() / 1024 / 1024, num);
                strcat(temp, num);
                strcat(temp, " MB");
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

                // draw total memory in bytes
                strcat(temp, "              ");
                strdec(mem_get_total_usable(), num);
                strcat(temp, num);
                strcat(temp, " bytes");
                Graphics::Canvas::DrawString(dx, dy, temp, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                dy += 12; temp[0] = '\0';

            }
        }
    }
}