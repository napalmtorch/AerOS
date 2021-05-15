#include "apps/win_welcome.hpp"
#include <core/kernel.hpp>
namespace System
{
    namespace Applications
    {
        void OpenMe(char* input)
        {
        KernelIO::XServer.WindowMgr.Open(new System::Applications::WinWelcome(120, 120));
        }

        WinWelcome::WinWelcome()
        {

        }

        WinWelcome::WinWelcome(int32_t x, int32_t y) : GUI::Window(x, y, 320, 200, "Welcome to AerOS")
        {
        System::KernelIO::Shell.RegisterCommand("WELCOME","Display the Welcome Screen","",OpenMe);   
        Document = (char*)new uint8_t[1000];

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
        }

    }
}