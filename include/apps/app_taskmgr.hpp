#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinTaskManager : public GUI::Window
        {
            // main methods
            public:
                WinTaskManager();
                WinTaskManager(int32_t x, int32_t y);
                ~WinTaskManager();
                void OnLoad() override;
                void Update() override;
                void Draw()   override;
        };
    }
}