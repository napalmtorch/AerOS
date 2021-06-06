#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <gui/widget.hpp>
#include <gui/winmgr.hpp>

namespace System
{
    namespace Applications
    {
        class WinTerminal : public GUI::Window
        {
            // main methods
            public:
                WinTerminal();
                WinTerminal(int32_t x, int32_t y);
                ~WinTerminal();
                void OnLoad() override;
                void Update() override;
                void Draw()   override;

            // private methods
            private:
                void DrawBuffer();

            // terminal methods
            public:
                void Clear();
                void Clear(Color color);
                void NewLine();
                void Delete();
                void Scroll();
                void PutChar(int x, int y, char c, Color fg, Color bg);
                void WriteChar(char c);
                void WriteChar(char c, Color fg);
                void WriteChar(char c, Color fg, Color bg);
                void Write(char* text);
                void Write(char* text, Color fg);
                void Write(char* text, Color fg, Color bg);
                void WriteLine(char* text);
                void WriteLine(char* text, Color fg);
                void WriteLine(char* text, Color fg, Color bg);
                void EnableCursor();
                void DisableCursor();
                void SetCursorPos(int x, int y);
                int32_t GetCursorX();
                int32_t GetCursorY();
                void SetColors(Color fg, Color bg);
                void SetForeColor(Color color);
                void SetBackColor(Color color);

            // properties
            private:
                // buffer
                uint8_t* Buffer;
                int32_t  BufferWidth;
                int32_t  BufferHeight;
                char*    InputBuffer;

                // cursor
                int32_t CursorX;
                int32_t CursorY;
                bool CursorVisible;
                bool CursorFlash;

                // appearance
                Graphics::Font* BufferFont;
                Color BackColor;
                Color ForeColor;
        };
    }
}