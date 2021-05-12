#include "apps/win_term.hpp"
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        bool cursor_flash;
        uint32_t time, last_time;

        WinTerminal::WinTerminal()
        {

        }

        WinTerminal::WinTerminal(int32_t x, int32_t y) : GUI::Window(x, y, 404, 259, "Terminal")
        {
            KernelIO::Terminal.Window = this;

            // initialize buffer
            BufferWidth = (Bounds->Width - 3) / 8;
            BufferHeight = (Bounds->Height - 19) / 8;
            Buffer = new uint8_t[BufferWidth * BufferHeight * 2];

            BackColor = COL4_BLACK;
            ForeColor = COL4_WHITE;

            Clear();
            WriteLine("Hello penis");
        }

        WinTerminal::WinTerminal(int32_t x, int32_t y, char* args) : GUI::Window(x, y, 400, 259, "Terminal")
        {

        }

        void WinTerminal::Update()
        {
            GUI::Window::Update();

            // set terminal
            if (Flags->Active)
            {
                KernelIO::Terminal.Window = this;
            }

            time = KernelIO::RTC.GetSecond();
            if (time != last_time)
            {
                cursor_flash = !cursor_flash;
                char temp[2];
                strdec(time, temp);
                KernelIO::Terminal.WriteLine(temp);
                last_time = time;
            }
        }

        void WinTerminal::Draw()
        {
            GUI::Window::Draw();
            
            if (Flags->CanDraw)
            {
                // draw buffer
                for (size_t y = 0; y < BufferHeight; y++)
                {
                    for (size_t x = 0; x < BufferWidth; x++)
                    {
                        uint32_t c = (x + (y * BufferWidth)) * 2;
                        uint8_t bg = ((Buffer[c + 1] & 0xF0) >> 4);
                        uint8_t fg = (Buffer[c + 1] & 0x0F);
                        KernelIO::XServer.FullCanvas.DrawChar(ClientBounds->X + (x * 8), ClientBounds->Y + (y * 8), Buffer[c], ConvertColor(fg), ConvertColor(bg), Graphics::FONT_8x8);
                    }
                }

                // draw cursor
                if (cursor_flash)
                {
                    uint32_t cx = ClientBounds->X + (CursorX * 8);
                    uint32_t cy = ClientBounds->Y + (CursorY * 8);
                    KernelIO::XServer.FullCanvas.DrawFilledRectangle(cx, cy + 6, 8, 2, ConvertColor(ForeColor));
                }
            }
        }

        void WinTerminal::Clear()
        {
            // fill buffer
            for (size_t i = 0; i < BufferWidth * BufferHeight * 2; i += 2)
            {
                Buffer[i] = 0x20; 
                Buffer[i + 1] = term_pack_colors(ForeColor, BackColor);
            }
        }

        void WinTerminal::NewLine()
        {
            CursorX = 0; CursorY++;
            if (CursorY >= BufferHeight) { Scroll(); CursorY--; }
        }

        void WinTerminal::Scroll()
        {
            // move buffer up 1 line
            mem_copy((uint8_t*)(Buffer + (BufferWidth * 2)), Buffer, (BufferWidth * BufferHeight * 2) - (BufferWidth * 2));
            // clear bottom line
            for (size_t j = (BufferWidth * BufferHeight * 2) - (BufferWidth * 2); j < (BufferWidth * BufferHeight * 2); j += 2)
            {
                Buffer[j] = 0x20;
                Buffer[j + 1] = term_pack_colors(COL4_WHITE, BackColor);
            }
        }

        // put character at position on screen
        void WinTerminal::PutChar(uint16_t x, uint16_t y, char c, COL4 fg, COL4 bg)
        {
            if (x >= BufferWidth || y >= BufferHeight) { return; }
            uint32_t offset = (x + (y * BufferWidth)) * 2;
            Buffer[offset] = c;
            Buffer[offset + 1] = term_pack_colors(fg, bg);
        }

        // write character to next position
        void WinTerminal::WriteChar(char c) { WriteChar(c, ForeColor, BackColor); }
        void WinTerminal::WriteChar(char c, COL4 fg) { WriteChar(c, fg, BackColor); }
        void WinTerminal::WriteChar(char c, COL4 fg, COL4 bg)
        {
            uint32_t offset = (CursorX + (CursorY * BufferWidth)) * 2;
            if (c == '\n') { NewLine(); }
            else
            {
                Buffer[offset] = c;
                Buffer[offset + 1] = term_pack_colors(fg, bg);
                CursorX++;
                if (CursorX >= BufferWidth) { NewLine(); }
            }
        }


        // write string to next position     
        void WinTerminal::Write(char* text) { Write(text, ForeColor, BackColor); }
        void WinTerminal::Write(char* text, COL4 fg) { Write(text, fg, BackColor); }
        void WinTerminal::Write(char* text, COL4 fg, COL4 bg)
        {
            for (size_t i = 0; i < strlen(text); i++)
            {
                WriteChar(text[i], fg, bg);
            }
        }

        // write line to next position
        void WinTerminal::WriteLine(char* text) { WriteLine(text, ForeColor, BackColor); }
        void WinTerminal::WriteLine(char* text, COL4 fg) { WriteLine(text, fg, BackColor); }
        void WinTerminal::WriteLine(char* text, COL4 fg, COL4 bg)
        {
            Write(text, fg, bg);
            WriteChar('\n');
        }

        // set cursor position
        void WinTerminal::SetCursorPos(uint16_t x, uint16_t y) { CursorX = x; CursorY = y;}
        void WinTerminal::SetCursorX(uint16_t x) { CursorX = x; }
        void WinTerminal::SetCursorY(uint16_t y) { CursorY = y; }

        void WinTerminal::DeleteLast()
        {
            CursorX--;
            uint32_t offset = (CursorX + (CursorY * BufferWidth)) * 2;
            Buffer[offset] = 0x20;
            Buffer[offset + 1] = term_pack_colors(ForeColor, BackColor);
        }

        Color WinTerminal::ConvertColor(uint8_t color)
        {
            switch (color)
            {
                case 0x00: { return Graphics::Colors::Black; break; }
                case 0x01: { return Graphics::Colors::DarkBlue; break; }
                case 0x02: { return Graphics::Colors::DarkGreen; break; }
                case 0x03: { return Graphics::Colors::DarkCyan; break; }
                case 0x04: { return Graphics::Colors::DarkRed; break; }
                case 0x05: { return Graphics::Colors::DarkMagenta; break; }
                case 0x06: { return Graphics::Colors::DarkOrange; break; }
                case 0x07: { return Graphics::Colors::DarkGray; break; }
                case 0x08: { return Graphics::Colors::DarkSlateBlue; break; }
                case 0x09: { return Graphics::Colors::Blue; break; }
                case 0x0A: { return Graphics::Colors::Green; break; }
                case 0x0B: { return Graphics::Colors::Cyan; break; }
                case 0x0C: { return Graphics::Colors::Red; break; }
                case 0x0D: { return Graphics::Colors::Magenta; break; }
                case 0x0E: { return Graphics::Colors::Yellow; break; }
                case 0x0F: { return Graphics::Colors::White; break; }
                default: return Graphics::Colors::Black;
            }

            return Graphics::Colors::Black;
        }
    }
}