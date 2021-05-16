#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>
#include <hardware/kbstream.hpp>

namespace System
{
    namespace Applications
    {
        class WinTextViewer : public GUI::Window
        {
            public:
                char* Document;
                HAL::KeyboardStream KBStream;
                WinTextViewer();
                WinTextViewer(int32_t x, int32_t y);
                WinTextViewer(int32_t x, int32_t y, char* text);
                void Update() override;
                void Draw() override;
        };
    }
}