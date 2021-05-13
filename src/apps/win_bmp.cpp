#include "apps/win_bmp.hpp"
#include <core/kernel.hpp>
#include <gui/bitmap.hpp>

namespace System
{
    namespace Applications
    {
        uint32_t* img;

        WinBitmapViewer::WinBitmapViewer()
        {

        }

        WinBitmapViewer::WinBitmapViewer(int32_t x, int32_t y) : GUI::Window(x, y, 400, 300, "Bitmap Viewer")
        {
            /*
             // load message of the day
            if(master_fs != nullptr)
            {
                // directory
                char* fname = "/lol.pic";
                
                // open file
                FILE *f = fopen(fname, NULL);

                if(f)
                { 
                    struct directory dir;
                    populate_root_dir(master_fs, &dir);
                    uint32_t size = FileSize(master_fs, &dir, "lol.pic");
                    debug_writeln_dec("SIZE: ", size);
                    img = new uint[size];

                    int count;
                    fread(img, size, 1, (FILE*)f);
                }
            }
            */

            bitmap_t *bitmap; 
            bitmap = bitmap_create("/test/test2.bmp");
            debug_writeln_dec("Bitmap Height:",bitmap->height);
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
                for (size_t yy = 0; yy < 4; yy++)
                {
                    for (size_t xx = 0; xx < 4; xx++)
                    {
                        KernelIO::XServer.FullCanvas.DrawPixel(ClientBounds->X + xx, ClientBounds->Y + yy, ((uint32_t*)img)[(xx + (yy * 4))]);
                    }
                }
                
            }
        }
    }
}