#include <gui/winmgr.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace GUI
    {

        void WindowManager::Initialize()
        {
            TestApp* win = new TestApp(128, 128);
            Open(win);
        }

        void WindowManager::Update()
        {
            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] != nullptr)
                {
                    Windows[i]->Update();
                }
            }
        }

        void WindowManager::Draw()
        {
            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] != nullptr)
                {
                    Windows[i]->Draw();
                }
            }
        }

        void WindowManager::Open(Window* win)
        {
            if (win == nullptr) { KernelIO::ThrowError("Cannot open null window"); return; }
            Windows[Index] = win;
            Index++;
            Count++;
        }

        void WindowManager::Close(Window* win)
        {
            for (size_t i = 0; i < Index + 1; i++)
            {
                if (Windows[i] == win)
                {
                    delete Windows[i];
                    Windows[i] = nullptr;
                    Count--;
                    return;
                }
            }
        }
    }
}