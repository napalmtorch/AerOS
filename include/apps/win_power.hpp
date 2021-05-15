#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinPower : public GUI::Window
        {
            public:
                WinPower();
                WinPower(int32_t x, int32_t y);
                void Update() override;
                void Draw() override;
        };
    }
}