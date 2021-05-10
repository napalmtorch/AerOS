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
            extern System::List<Window*> Windows;

            void Initialize();
            void Update();
            void Draw();
            void Start(Window* win);
            void Close(Window* win);
        }
    }
}