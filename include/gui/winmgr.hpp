#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <lib/list.hpp>
#include <gui/widget.hpp>

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
                void Open(Window* win);
                void Close(Window* win);
            private:
                Window* Windows[4096];
                uint32_t Index;
                uint32_t Count;
        };
    }
}