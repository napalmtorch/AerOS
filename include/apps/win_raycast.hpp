#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinRaycaster : public GUI::Window
        {
            public:
                WinRaycaster();
                WinRaycaster(int32_t x, int32_t y);
                ~WinRaycaster();
                void Update() override;
                void Draw() override;
                Color ConvertColor(uint8_t color);
            private:
                uint32_t BufferWidth;
                uint32_t BufferHeight;
                uint8_t* Buffer;
        };
    }
}