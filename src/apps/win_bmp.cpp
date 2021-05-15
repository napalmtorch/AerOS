#include "apps/win_bmp.hpp"
#include <core/kernel.hpp>
#include <graphics/bitmap.hpp>

namespace System
{
    namespace Applications
    {
        Graphics::Bitmap* bmp;

        WinBitmapViewer::WinBitmapViewer()
        {

        }

        WinBitmapViewer::WinBitmapViewer(int32_t x, int32_t y) : GUI::Window(x, y, 400, 300, "Bitmap Viewer")
        {
            bmp = new Graphics::Bitmap("/test/smiley.bmp");
        }

        void WinBitmapViewer::Update()
        {
            GUI::Window::Update();
        }

        void WinBitmapViewer::Draw()
        {
            GUI::Window::Draw();

            if (Flags->CanDraw) 
            { 
                Graphics::Canvas::DrawBitmap(ClientBounds->X, ClientBounds->Y, 2, bmp);
            }
        }
    }
}