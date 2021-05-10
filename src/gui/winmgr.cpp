#include <gui/winmgr.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace GUI
    {
        // window manager
        namespace WindowManager
        {
            Window* Windows[512];
            uint32_t WindowCount = 0;
            uint32_t WindowIndex = 0;

            
            void Initialize()
            {
                
            }   

            void Update()
            {   
                if (WindowCount > 0)
                {
                    for (size_t i = 0; i < WindowCount; i++)
                    {
                        if (Windows[i] != nullptr)
                        {
                            Windows[i]->Update();
                        }
                    }
                }
            }

            void Draw()
            {
                if (WindowCount > 0)
                {
                    for (size_t i = 0; i < WindowCount; i++)
                    {
                        if (Windows[i] != nullptr)
                        {
                            Windows[i]->Draw();
                        }
                    }
                }

            }

            void Start(System::GUI::Window* win)
            {
                if (win == nullptr) { KernelIO::ThrowError("Tried to start null window"); return; }
                Windows[WindowIndex] = win;
                WindowIndex++;
                WindowCount++;
            }

            void Close(System::GUI::Window* win)
            {
                if (win == nullptr) { KernelIO::ThrowError("Tried to close null window"); return; }
                for (size_t i = 0; i < WindowCount; i++)
                {
                    if (Windows[i] == win && win != nullptr)
                    {
                        delete Windows[i];
                        Windows[i] = nullptr;
                        break;
                    }
                }
            }
        }
    }
}