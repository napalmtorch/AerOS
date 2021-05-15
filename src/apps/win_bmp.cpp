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
            bmp = new Graphics::Bitmap("/test/parrot.bmp");
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
                if (bmp != nullptr)
                {
                    for (int32_t yy = bmp->Height - 1; yy >= 0; yy--)
                    {
                        for (int32_t xx = bmp->Width - 1; xx >= 0; xx--)
                        {
                            if (bmp->Depth == COLOR_DEPTH_24)
                            {
                                uint32_t offset = (3 * (xx + (yy * bmp->Width)));
                                uint32_t color = Graphics::RGBToPackedValue(bmp->ImageData[offset + 2], bmp->ImageData[offset + 1], bmp->ImageData[offset]);
                                Graphics::Canvas::DrawPixel(ClientBounds->X + (bmp->Width - xx), ClientBounds->Y + (bmp->Height - yy), color);
                            }
                            else if (bmp->Depth == COLOR_DEPTH_32)
                            {
                                uint32_t offset = (4 * (xx + (yy * bmp->Width)));
                                uint32_t color = Graphics::RGBToPackedValue(bmp->ImageData[offset + 2], bmp->ImageData[offset + 1], bmp->ImageData[offset]);
                                Graphics::Canvas::DrawPixel(ClientBounds->X + (bmp->Width - xx), ClientBounds->Y + (bmp->Height - yy), color);
                                //Graphics::Canvas::DrawPixel(ClientBounds->X + xx, ClientBounds->Y + yy, color);
                            }
                        }
                    }  
                }
            }
        }
    }
}