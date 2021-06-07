#include <apps/app_term.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        const uint ATTR_SIZE = 9;

        WinTerminal::WinTerminal() : GUI::Window() { }

        WinTerminal::WinTerminal(int32_t x, int32_t y) : GUI::Window(x, y, 474, 325, "XTerm", "xterm")
        {
            // set backcolor to transparent - region will be populate by buffer and this will improve performance
            Style.Colors[0] = Graphics::Colors::Transparent;

            // set buffer appearance
            BufferFont = &Graphics::FONT_8x16_CONSOLAS;
            ForeColor = Graphics::Colors::White;
            BackColor = Graphics::Colors::Black;

            // determine buffer size
            int fw = BufferFont->GetWidth() + BufferFont->GetHorizontalSpacing();
            int fh = BufferFont->GetHeight() + BufferFont->GetVerticalSpacing();
            BufferWidth = ClientBounds.Width / fw;
            BufferHeight = ClientBounds.Height / fh;

            // create buffer
            Buffer = new uint8_t[BufferWidth * BufferHeight * ATTR_SIZE];
            Clear();

            // create input buffer
            InputBuffer = new char[512];

            // reset cursor
            CursorVisible = true;
            CursorFlash = false;
            CursorX = 0;
            CursorY = 0;

            // setup keyboard
            KernelIO::Terminal.RegisterWindow(this);
            KernelIO::Keyboard.Buffer = InputBuffer;
            KernelIO::Keyboard.BufferEnabled = true;
            KernelIO::Keyboard.TerminalBuffer = true;
        }

        WinTerminal::~WinTerminal()
        {
            // delete buffer
            delete Buffer;
        }

        void WinTerminal::OnLoad()
        {
            // load base
            GUI::Window::OnLoad();
        }

        void WinTerminal::Update()
        {
            // update base
            GUI::Window::Update();

            if (Flags.Active)
            {
                KernelIO::Terminal.RegisterWindow(this);
                KernelIO::Keyboard.Buffer = InputBuffer;
                KernelIO::Keyboard.BufferEnabled = true;
                KernelIO::Keyboard.TerminalBuffer = true;
            }
            else
            {
                if (KernelIO::Keyboard.Buffer == InputBuffer) { KernelIO::Keyboard.Buffer = nullptr; }
                if (KernelIO::Terminal.Window == this) { KernelIO::Terminal.Window = nullptr; }
            }

            // auto size if changed
            int fw = BufferFont->GetWidth() + BufferFont->GetHorizontalSpacing();
            int fh = BufferFont->GetHeight() + BufferFont->GetVerticalSpacing();
            if (old_w != Bounds.Width || old_h != Bounds.Height)
            {
                if (Buffer != nullptr) { mem_free(Buffer); }
                BufferWidth = (Bounds.Width - 2) / fw;
                BufferHeight = (Bounds.Height - 21) / fh;
                Buffer = new uint8_t[BufferWidth * BufferHeight * ATTR_SIZE];
                Clear();
                KernelIO::Shell.PrintCaret();

                old_w = Bounds.Width;
                old_h = Bounds.Height;
            }
        }

        void WinTerminal::Draw()
        {
            // draw base
            GUI::Window::Draw();

            // draw character buffer to window
            if (Flags.CanDraw) { DrawBuffer(); }
        }

        void WinTerminal::DrawBuffer()
        {
            int fw = BufferFont->GetWidth() + BufferFont->GetHorizontalSpacing();
            int fh = BufferFont->GetHeight() + BufferFont->GetVerticalSpacing();
            int diff_w = ClientBounds.Width - (fw * BufferWidth);
            int diff_h = ClientBounds.Height - (fh * BufferHeight);
            //debug_writeln_dec("DIFF W: ", diff_w);
            //debug_writeln_dec("DIFF H: ", diff_h);

            for (int32_t j = 0; j < BufferHeight; j++)
            {
                for (int32_t i = 0; i < BufferWidth; i++)
                {
                    uint32_t offset = (i + (j * BufferWidth)) * ATTR_SIZE;
                    char c = Buffer[offset];
                    Color* fg = (Color*)(Buffer + offset + 1);
                    Color* bg = (Color*)(Buffer + offset + 5);
                    Graphics::Canvas::DrawChar(ClientBounds.X + (i * fw) - 1, ClientBounds.Y + (j * fh), c, (*fg), (*bg), (*BufferFont));
                }
            }

            if (diff_w > 0) { Graphics::Canvas::DrawFilledRectangle(ClientBounds.X + ClientBounds.Width - diff_w, ClientBounds.Y, diff_w, ClientBounds.Height, BackColor); }
            if (diff_h > 0) { Graphics::Canvas::DrawFilledRectangle(ClientBounds.X, ClientBounds.Y + ClientBounds.Height - diff_h, ClientBounds.Width, diff_h, BackColor); }
        }

        void WinTerminal::Clear() { Clear(BackColor); }

        void WinTerminal::Clear(Color color)
        {
            BackColor = color;
            for (size_t i = 0; i < BufferWidth * BufferHeight * ATTR_SIZE; i += ATTR_SIZE)
            {
                Buffer[i] = 0x20;
                Color* fg = (Color*)(Buffer + i + 1);
                Color* bg = (Color*)(Buffer + i + 5);
                Graphics::SetColor(fg, 255, ForeColor.R, ForeColor.G, ForeColor.B);
                Graphics::SetColor(bg, 255, color.R, color.G, color.B);
            }
            SetCursorPos(0, 0);
        }

        void WinTerminal::NewLine()
        {
            CursorX = 0; CursorY++;
            if (CursorY >= BufferHeight) { Scroll(); CursorY--; }
        }

        void WinTerminal::Delete()
        {
            CursorX--;
            WriteChar(' ');
            CursorX--;
        }

        void WinTerminal::Scroll()
        {
            uint32_t size = (BufferWidth * BufferHeight) * ATTR_SIZE;
            mem_copy((uint8_t*)(Buffer + (BufferWidth * ATTR_SIZE)), Buffer, size - (BufferWidth * ATTR_SIZE));
            for (size_t i = size - (BufferWidth * ATTR_SIZE); i < size; i += ATTR_SIZE)
            {
                Buffer[i] = 0x20;
                Color* fg = (Color*)(Buffer + i + 1);
                Color* bg = (Color*)(Buffer + i + 5);
                Graphics::SetColor(fg, 255, ForeColor.R, ForeColor.G, ForeColor.B);
                Graphics::SetColor(bg, 255, BackColor.R, BackColor.G, BackColor.B);
            }
        }

        void WinTerminal::PutChar(int x, int y, char c, Color fg, Color bg)
        {
            if (x < 0 || x >= BufferWidth || y < 0 || y >= BufferHeight) { return; }
            int i = (x + (y * BufferWidth)) * ATTR_SIZE;
            Buffer[i] = c;
            Color* ffg = (Color*)(Buffer + i + 1);
            Color* bbg = (Color*)(Buffer + i + 5);
            Graphics::SetColor(ffg, 255, fg.R, fg.G, fg.B);
            Graphics::SetColor(bbg, 255, bg.R, bg.G, bg.B);
        }

        void WinTerminal::WriteChar(char c) { WriteChar(c, ForeColor, BackColor); }

        void WinTerminal::WriteChar(char c, Color fg) { WriteChar(c, fg, BackColor); }

        void WinTerminal::WriteChar(char c, Color fg, Color bg)
        {
            if (c == '\n') { NewLine(); }
            else if (c >= 32 && c <= 126)
            {
                int i = (CursorX + (CursorY * BufferWidth)) * ATTR_SIZE;
                Buffer[i] = c;
                Color* ffg = (Color*)(Buffer + i + 1);
                Color* bbg = (Color*)(Buffer + i + 5);
                Graphics::SetColor(ffg, 255, fg.R, fg.G, fg.B);
                Graphics::SetColor(bbg, 255, bg.R, bg.G, bg.B);
                CursorX++;
                if (CursorX >= BufferWidth) { NewLine(); }
            }
        }

        void WinTerminal::Write(char* text) { Write(text, ForeColor, BackColor); }

        void WinTerminal::Write(char* text, Color fg) { Write(text, fg, BackColor); }
        
        void WinTerminal::Write(char* text, Color fg, Color bg)
        {
            for (size_t i = 0; i < strlen(text); i++) { WriteChar(text[i], fg, bg); }
        }

        void WinTerminal::WriteLine(char* text) { WriteLine(text, ForeColor, BackColor); }

        void WinTerminal::WriteLine(char* text, Color fg) { WriteLine(text, fg, BackColor); }

        void WinTerminal::WriteLine(char* text, Color fg, Color bg)
        {
            Write(text, fg, bg);
            NewLine();
        }

        void WinTerminal::EnableCursor() { CursorVisible = true; }

        void WinTerminal::DisableCursor() { CursorVisible = false; }

        void WinTerminal::SetCursorPos(int x, int y)
        {
            if (x < 0 || x >= BufferWidth || y < 0 || y >= BufferHeight) { return; }
            CursorX = x;
            CursorY = y;
        }

        int32_t WinTerminal::GetCursorX() { return CursorX; }

        int32_t WinTerminal::GetCursorY() { return CursorY; }

        void WinTerminal::SetColors(Color fg, Color bg)
        {
            ForeColor = fg;
            BackColor = bg;
        }

        void WinTerminal::SetForeColor(Color color) { ForeColor = color; }

        void WinTerminal::SetBackColor(Color color) { BackColor = color; }
    }
}