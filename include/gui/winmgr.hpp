#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/window.hpp>

namespace System
{
    namespace GUI
    {
        class WindowManager
        {
            public:
                void Initialize();
                void Update();
                void Draw();
                Window* Start(Window* win);
                bool Close(Window* win);
                Window* SetActiveWindow(Window* win);
                Window* GetWindow(int32_t index);
                int32_t GetWindowIndex(GUI::Window* window);
                Window* IsWindowRunning(GUI::Window* window);
                
            private:
                Window** Windows;
                Window*  ActiveWindow;
                int32_t  WindowCount;
                int32_t  ActiveIndex;
        };
    }
}