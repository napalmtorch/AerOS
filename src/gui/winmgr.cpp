#include <gui/winmgr.hpp>

namespace System
{
    namespace GUI
    {
        // window manager
        namespace WindowManager
        {
            System::GUI::Window* Windows[512];
            uint32_t WindowCount;
            uint32_t WindowIndex;
            
            void Initialize()
            {
                
            }   

            void Update()
            {   
                for (size_t i = 0; i < WindowCount; i++)
                {
                    if (Windows[i] != nullptr)
                    {
                        Windows[i]->Update();
                    }
                }
            }

            void Draw()
            {
                for (size_t i = 0; i < WindowCount; i++)
                {
                    if (Windows[i] != nullptr)
                    {
                        if (Windows[i] != nullptr) { Windows[i]->Draw(); }
                    }
                }
            }

            void Start(System::GUI::Window* win)
            {
                if (win == nullptr) { return; }
                Windows[WindowIndex] = win;
                WindowCount++;
            }

            void Close(System::GUI::Window* win)
            {
                if (win == nullptr) { return; }
                for (size_t i = 0; i < WindowCount; i++)
                {
                    if (Windows[i] == win)
                    {
                        Windows[i] = nullptr;
                        return;
                    }
                }
                KernelIO::ThrowError("Tried to close null window");
            }
        }
    }
}