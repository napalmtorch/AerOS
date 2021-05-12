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

                void NewLine();
                void Scroll();

                void PutChar(uint32_t x, uint32_t y, char c, COL4 fg, COL4 bg);

                // put character at position on screen
                void PutChar(uint16_t x, uint16_t y, char c, COL4 fg, COL4 bg);

                // write character to next position
                void WriteChar(char c);
                void WriteChar(char c, COL4 fg);
                void WriteChar(char c, COL4 fg, COL4 bg);

                // write string to next position
                void Write(char* text);
                void Write(char* text, COL4 fg);
                void Write(char* text, COL4 fg, COL4 bg);

                // write line to next position
                void WriteLine(char* text);
                void WriteLine(char* text, COL4 fg);
                void WriteLine(char* text, COL4 fg, COL4 bg);

                // set cursor position
                void SetCursorPos(uint16_t x, uint16_t y);
                void SetCursorX(uint16_t x);
                void SetCursorY(uint16_t y);

                void DeleteLast();
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