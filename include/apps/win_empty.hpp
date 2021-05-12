#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinEmpty : public GUI::Window
        {
            public:
                WinEmpty();
                WinEmpty(int32_t x, int32_t y);
                void Update() override;
                void Draw() override;
        };
    }
}