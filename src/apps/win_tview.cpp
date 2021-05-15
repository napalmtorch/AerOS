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
            Document = (char*)new uint8_t[1000];

            // load message of the day
            if(fat_master_fs != nullptr)
            {
                // directory
                char* motd = "/etc/motd";
                
                // open file
                file_t *f = fopen(motd, NULL);

                if(f)
                { 
                    uint8_t c[1000];
                    int count;
                    while((count = fread(&c, 1000, 1, f)), count > 0) 
                    {
                        for(int i = 0; i < count; i++) { stradd(Document, c[i]); }
                    }
                }
            }

            Style = GUI::CopyStyle(&GUI::WindowStyle);
            Style->Colors[0] = Graphics::Colors::White;
        }

        WinTextViewer::WinTextViewer(int32_t x, int32_t y, char* text)
        {
            Document = (char*)new uint8_t[1000];
            
            // load message of the day
            if(fat_master_fs != nullptr)
            {
              
                // open file
                file_t *f = fopen(text, NULL);

                if(f)
                { 
                    uint8_t c[1000];
                    int count;
                    while((count = fread(&c, 1000, 1, f)), count > 0) 
                    {
                        for(int i = 0; i < count; i++) { stradd(Document, c[i]); }
                    }
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

            if (Flags->CanDraw) { Graphics::Canvas::DrawString(ClientBounds->X, ClientBounds->Y, Document, Graphics::Colors::Black, Graphics::FONT_8x16_CONSOLAS); }
        }
    }
}