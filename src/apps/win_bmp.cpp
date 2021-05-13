#include "apps/win_bmp.hpp"
#include <core/kernel.hpp>
#include <gui/bitmap.hpp>

namespace System
{
    namespace Applications
    {
        WinBitmapViewer::WinBitmapViewer()
        {

        }

        WinBitmapViewer::WinBitmapViewer(int32_t x, int32_t y) : GUI::Window(x, y, 400, 300, "Bitmap Viewer")
        {
        bitmap_t *bitmap; 
           bitmap = bitmap_create("/test/test2.bmp");
           debug_writeln_dec("Bitmap Height:",bitmap->height);
            Style = GUI::CopyStyle(&GUI::WindowStyle);
            Style->Colors[0] = Graphics::Colors::White;
        }

        void WinBitmapViewer::Update()
        {
            GUI::Window::Update();
        }

        void WinBitmapViewer::Draw()
        {
            GUI::Window::Draw();

            if (Flags->CanDraw) { KernelIO::XServer.FullCanvas.DrawString(ClientBounds->X, ClientBounds->Y, Document, Graphics::Colors::Black, Graphics::FONT_8x16_CONSOLAS); }
        }
    }
}