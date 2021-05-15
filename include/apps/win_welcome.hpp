#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinWelcome : public GUI::Window
        {
            public:
                char* Document;
                WinWelcome();
                WinWelcome(int32_t x, int32_t y);
                void Update() override;
                void Draw() override;
        };
    }
}