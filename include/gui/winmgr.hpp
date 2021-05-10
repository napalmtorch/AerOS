#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <lib/list.hpp>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace GUI
    {
        // window manager
        namespace WindowManager
        {
            extern System::GUI::Window* Windows[];
            extern uint32_t WindowCount;
            extern uint32_t WindowIndex;

            void Initialize();
            void Update();
            void Draw();
            void Start(System::GUI::Window* win);
            void Close(System::GUI::Window* win);
        }
    }
}