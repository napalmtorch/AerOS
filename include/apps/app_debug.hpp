#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinDebug : public GUI::Window
        {
            // main methods
            public:
                WinDebug();
                WinDebug(int32_t x, int32_t y);
                ~WinDebug();
                void OnLoad() override;
                void Update() override;
                void Draw()   override;
        };
    }
}