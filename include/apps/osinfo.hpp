#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/xserver.hpp>
#include <gui/widget.hpp>
#include <gui/winmgr.hpp>

namespace Applications
{
    namespace OSInfo
    {
            extern System::GUI::Window* Window;
            void Start();
            void Update();
            void Draw();
    };
}