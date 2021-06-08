#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinAbout : public GUI::Window
        {
            // main methods
            public:
                WinAbout();
                WinAbout(int32_t x, int32_t y);
                ~WinAbout();
                void OnLoad() override;
                void Update() override;
                void Draw()   override;
        };
    }
}