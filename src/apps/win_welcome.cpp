#include "apps/win_welcome.hpp"
#include <core/kernel.hpp>
#include <hardware/config_parser.hpp>
namespace System
{
    namespace Applications
    {

        WinWelcome::WinWelcome()
        {
              
        }

        WinWelcome::WinWelcome(int32_t x, int32_t y) : GUI::Window(x, y, 400, 300, "Welcome")
        { 
            Document = (char*)new uint8_t[1000];
            Config::ConfigParser* config = new Config::ConfigParser("/etc/global.cfg");
            debug_writeln_ext(config->GetConfigValue("wallpaper"),COL4_RED);
            debug_writeln_ext(config->GetConfigValue("mouse"),COL4_GREEN);
            // load message of the day
            if(fat_master_fs != nullptr)
            {
                if(fat_file_exists("/users/aeros/documents/welcome.txt"))
                {
                // directory
                char* file = "/users/aeros/documents/welcome.txt";
            
                // open file
                file_t *f = fopen(file, NULL);

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
                else 
                {
                    Document = "Welcome to AerOS";
                }
            }

            Style = GUI::CopyStyle(&GUI::WindowStyle);
            Style->Colors[0] = Graphics::Colors::White;        
        }

        void WinWelcome::Update()
        {
            GUI::Window::Update();
        }

        void WinWelcome::Draw()
        {
            GUI::Window::Draw();

            if (Flags->CanDraw) { Graphics::Canvas::DrawString(ClientBounds->X, ClientBounds->Y, Document, Graphics::Colors::Black, Graphics::FONT_8x16_CONSOLAS); }
        }

    }
}