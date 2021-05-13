#include "apps/win_tview.hpp"
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        WinTextViewer::WinTextViewer()
        {

        }

        WinTextViewer::WinTextViewer(int32_t x, int32_t y) : GUI::Window(x, y, 400, 300, "Text Viewer")
        {
            // load message of the day
            if(master_fs != nullptr)
            {
                // directory
                char* motd = "/etc/motd";
                
                // open file
                FILE *f = fopen(motd, NULL);

                if(f)
                { 
                    uint8_t c[1000];
                    int count;
                    Document = (char*)mem_alloc(1000);
                    while((count = fread(&c, 1000, 1, f)), count > 0);
                    strcpy((char*)c, Document);
                    //fclose(f);
                }
            }

            Style = GUI::CopyStyle(&GUI::WindowStyle);
            Style->Colors[0] = Graphics::Colors::White;
        }

        WinTextViewer::WinTextViewer(int32_t x, int32_t y, char* text)
        {
            // load message of the day
            if(master_fs != nullptr)
            {
                // directory
                char* motd = text;
                
                // open file
                FILE *f = fopen(motd, NULL);

                if(f)
                { 
                    uint8_t c[1000];
                    int count;
                    Document = (char*)mem_alloc(1000);
                    while((count = fread(&c, 1000, 1, f)), count > 0);
                    strcpy((char*)c, Document);
                    //fclose(f);
                }
            }

            Style = GUI::CopyStyle(&GUI::WindowStyle);
            Style->Colors[0] = Graphics::Colors::White;
        }

        void WinTextViewer::Update()
        {
            GUI::Window::Update();
        }

        void WinTextViewer::Draw()
        {
            GUI::Window::Draw();

            if (Flags->CanDraw) { KernelIO::XServer.FullCanvas.DrawString(ClientBounds->X, ClientBounds->Y, Document, Graphics::Colors::Black, Graphics::FONT_8x16_CONSOLAS); }
        }
    }
}