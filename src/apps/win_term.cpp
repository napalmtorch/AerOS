#include "apps/win_term.hpp"
#include <core/kernel.hpp>

namespace System
{
    char vterm_kb_buffer[256];

    namespace Applications
    {
        bool cursor_flash;
        uint32_t time, last_time;
        uint32_t old_w, old_h;

        WinTerminal::WinTerminal()
        {

        }

        WinTerminal::WinTerminal(int32_t x, int32_t y) : GUI::Window(x, y, 404, 259, "Terminal\0")
        {
            KernelIO::Terminal.Window = this;

            // initialize buffer
            BufferWidth = (Bounds->Width - 3) / 8;
            BufferHeight = (Bounds->Height - 19) / 16;
            old_w = BufferWidth; old_h = BufferHeight;
            Buffer = new uint8_t[BufferWidth * BufferHeight * 2];

            // default colors
            BackColor = COL4_BLACK;
            ForeColor = COL4_WHITE;

            Clear();

            // load message of the day
            if(fat_master_fs != nullptr)
            {
                // directory
                char* motd = "/etc/motd";
                
                // open file
                file_t *f = fopen(motd, NULL);

                if(f)
                {
                    struct directory dir;
                    fat_populate_root_dir(fat_master_fs, &dir);
                    fat_cat_file(fat_master_fs,&dir,motd);
                }
                else { WriteLine("AerOS Terminal"); }
            }
            else { WriteLine("AerOS Terminal"); }
            KernelIO::Shell.PrintCaret();
        }

        WinTerminal::WinTerminal(int32_t x, int32_t y, char* args) : GUI::Window(x, y, 400, 259, "Terminal\0")
        {

        }

        void WinTerminal::Update()
        {
            GUI::Window::Update();

            // set terminal
            if (Flags->Active)
            {
                if (KernelIO::Terminal.Window != this) { KernelIO::Terminal.RegisterWindow(this); KernelIO::Shell.PrintCaret(); }
                cursor_x = CursorX;
                cursor_y = CursorY;
                KernelIO::Keyboard.Buffer = vterm_kb_buffer;
                KernelIO::Keyboard.BufferEnabled = true;
                KernelIO::Keyboard.Event_OnEnterPressed = enter_pressed;
                if (!KernelIO::Keyboard.TerminalBuffer) { KernelIO::Keyboard.TerminalBuffer = true; }
            }
            else
            {
                if (KernelIO::Keyboard.Buffer == vterm_kb_buffer) { KernelIO::Keyboard.Buffer = nullptr; }
                 KernelIO::Keyboard.TerminalBuffer = false;
            }

            time = KernelIO::RTC.GetSecond();
            if (time != last_time)
            {
                cursor_flash = !cursor_flash;
                last_time = time;
            }

            if (old_w != Bounds->Width || old_h != Bounds->Height)
            {
                if (Buffer != nullptr) { delete Buffer; }
                // initialize buffer
                BufferWidth = (Bounds->Width - 3) / 8;
                BufferHeight = (Bounds->Height - 19) / 16;
                Buffer = new uint8_t[BufferWidth * BufferHeight * 2];                
                KernelIO::Terminal.RegisterWindow(this); 
                Clear();
                KernelIO::Shell.PrintCaret();

                old_w = Bounds->Width;
                old_h = Bounds->Height;
            }
        }

        void WinTerminal::Draw()
        {
            GUI::Window::Draw();
            
            if (Flags->CanDraw)
            {
                Graphics::Canvas::DrawFilledRectangle((*ClientBounds), Graphics::Colors::Black);
                
                // draw buffer
                for (size_t y = 0; y < BufferHeight; y++)
                {
                    for (size_t x = 0; x < BufferWidth; x++)
                    {
                        uint32_t c = (x + (y * BufferWidth)) * 2;
                        uint8_t bg = ((Buffer[c + 1] & 0xF0) >> 4);
                        uint8_t fg = (Buffer[c + 1] & 0x0F);
                        Graphics::Canvas::DrawChar(ClientBounds->X + (x * 8), ClientBounds->Y + (y * 16), Buffer[c], ConvertColor(fg), ConvertColor(bg), Graphics::FONT_8x16);
                    }
                }

                // draw cursor
                if (cursor_flash)
                {
                    uint32_t cx = ClientBounds->X + (CursorX * 8);
                    uint32_t cy = ClientBounds->Y + (CursorY * 16);
                    Graphics::Canvas::DrawFilledRectangle(cx, cy + 13, 8, 2, ConvertColor(ForeColor));
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
            CursorX = 0;
            CursorY = 0;
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
            NewLine();
        }

        // set cursor position
        void WinTerminal::SetCursorPos(uint16_t x, uint16_t y) { CursorX = x; CursorY = y; }
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