#pragma once
#include <lib/types.h>
#include <gui/widget.hpp>
#include <gui/xserver.hpp>
#include <graphics/colors.hpp>

namespace System
{
    namespace Applications
    {
        class WinTerminal : public GUI::Window
        {
            public:
                WinTerminal();
                WinTerminal(int32_t x, int32_t y);
                WinTerminal(int32_t x, int32_t y, char* args);
                void Update() override;
                void Draw() override;
                void Clear();
                void WriteChar(char c);
                void Write(char* text);
                void WriteLine(char* text);
                Color ConvertColor(uint8_t color);
            public:
                uint8_t* Buffer;
                uint32_t BufferWidth;
                uint32_t BufferHeight;
                uint32_t CursorX;
                uint32_t CursorY;
                COL4 BackColor;
                COL4 ForeColor;
        };
    }
}