#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinGUIDemo : public GUI::Window
        {
            public:
                WinGUIDemo();
                WinGUIDemo(int32_t x, int32_t y);
                void Update() override;
                void Draw() override;
        };
    }
}