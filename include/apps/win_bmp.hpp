#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>

namespace System
{
    namespace Applications
    {
        class WinBitmapViewer : public GUI::Window
        {
            public:
                char* Document;
                WinBitmapViewer();
                WinBitmapViewer(int32_t x, int32_t y);
                ~WinBitmapViewer() override;
                void Update() override;
                void Draw() override;
        };
    }
}