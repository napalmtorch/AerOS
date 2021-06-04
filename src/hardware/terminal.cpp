#include "hardware/terminal.hpp"
#include <core/kernel.hpp>

extern "C"
{
    // buffer dimensions
    uint8_t buffer_width, buffer_height;

    // buffer offset
    uint8_t* buffer = nullptr;

    // cursor
    bool cursor_visible;
    bool cursor_flash;
    int32_t cursor_flash_tick;
    uint8_t cursor_x, cursor_y;

    // appearance
    Color fore_color, back_color;
    Graphics::Font* font;

    // initialize terminal interface
    void term_init()
    {
        term_cursor_enable(0, 0);
        term_set_cursor(0, 0);

        // initialize default colors
        fore_color = Graphics::Colors::White;
        back_color = Graphics::Colors::Black;
        font = &Graphics::FONT_8x16_CONSOLAS;

        term_set_size_auto();
    }

    void term_draw()
    {
        int32_t fw = font->GetWidth() + font->GetHorizontalSpacing();
        int32_t fh = font->GetHeight() + font->GetVerticalSpacing();

         // loop through buffer
        for (int32_t j = 0; j < buffer_height; j++)
        {
            for (int32_t i = 0; i < buffer_width; i++)
            {
                // get character properties
                uint32_t offset = (i + (j * buffer_width)) * 9;
                char c = buffer[offset];
                Color* fg = (Color*)(buffer + offset + 1);
                Color* bg = (Color*)(buffer + offset + 5);
                // draw character
                Graphics::Canvas::DrawChar(i * fw, j * fh, c, (*fg), (*bg), (*font));
            }
        }

        if (cursor_visible && cursor_flash)
        {
            Graphics::Canvas::DrawFilledRectangle(cursor_x * fw, (cursor_y * fh) + (fh - 2), fw, 2, fore_color);
        }
    }

    void term_cursor_flash()
    {
        cursor_flash_tick++;
        if (cursor_flash_tick >= 15) { cursor_flash = !cursor_flash; cursor_flash_tick = 0; }
    }

    // clear the terminal
    void term_clear(Color color)
    {
        for (uint32_t i = 0; i < buffer_width * buffer_height * 9; i += 9)
        {
            buffer[i] = 0x20;
            Color* fg = (Color*)(buffer + i + 1);
            Color* bg = (Color*)(buffer + i + 5);
            fg->R = fore_color.R; fg->G = fore_color.G; fg->B = fore_color.B; fg->A = 255;
            bg->R = back_color.R; bg->G = back_color.G; bg->B = back_color.B; bg->A = 255;
        }

        System::KernelIO::VESA.Clear(Graphics::RGBToPackedValue(back_color.R, back_color.G, back_color.B));
        term_set_cursor(0, 0);
    }

    // write character to terminal
    void term_write_char(char c)
    {
        if (c == '\n') { term_newline(); }
        else if (c >= 32 && c <= 126)
        {
            term_put_char(cursor_x, cursor_y, c, fore_color, back_color);
            cursor_x++;
            if (cursor_x >= buffer_width) { term_newline(); }
        }
    }

    // put specified character at position in terminal
    void term_put_char(uint8_t x, uint8_t y, char c, Color fg, Color bg)
    {
        if (x < 0 || y < 0 || x >= buffer_width || y >= buffer_height) { return; }
        uint32_t offset = (x + (y * buffer_width)) * 9;
        buffer[offset] = c;
        Color* ffg = (Color*)(buffer + offset + 1);
        Color* bbg = (Color*)(buffer + offset + 5);
        ffg->R = fg.R; ffg->G = fg.G; ffg->B = fg.B; ffg->A = 255;
        bbg->R = bg.R; bbg->G = bg.G; bbg->B = bg.B; bbg->A = 255;
    }

    // write string to terminal
    void term_write(char* text) { term_write_ext(text, fore_color); }

    // write string to terminal with foreground color
    void term_write_ext(char* text, Color color)
    {
        Color fg_old = fore_color;
        fore_color = color;
        for (size_t i = 0; i < strlen(text); i++) { term_write_char(text[i]); }
        fore_color = fg_old;
    }

    // write string and decimal value to terminal
    void term_write_dec(char* text, int32_t num)
    {
        term_write(text);
        char temp[32];
        strdec(num, temp);
        term_write(temp);
    }

    // write string and hexadecimal value to terminal 
    void term_write_hex(char* text, uint32_t num)
    {
        term_write(text);
        char temp[32];
        strhex32(num, temp);
        term_write(temp);
    }

    // write line to terminal
    void term_writeln(char* text)
    {
       term_write(text);
       term_newline();
    }

    // write line to terminal with foreground color
    void term_writeln_ext(char* text, Color color)
    {
        term_write_ext(text, color);
        term_newline();
    }

    // write line and decimal value to terminal
    void term_writeln_dec(char* text, int32_t num)
    {
        term_write_dec(text, num);
        term_newline();
    }

    // write line and hexadecimal value to terminal 
    void term_writeln_hex(char* text, uint32_t num)
    {
        term_write_hex(text, num);
        term_newline();
    }

    // delete last character in terminal
    void term_delete()
    {
        cursor_x--;
        term_write_char(0x20);
        cursor_x--;
    }

    // generate new line in terminal
    void term_newline()
    {
        cursor_x = 0; cursor_y++;
        if (cursor_y >= buffer_height)
        {
            term_scroll(1);
            cursor_y--;
        }
    }

    // scroll terminal by amount of lines
    void term_scroll(uint32_t amount)
    {
        uint32_t size = buffer_width * buffer_height * 9;
        for (size_t i = 0; i < amount; i++)
        {
            mem_copy((uint8_t*)(buffer + (buffer_width * 9)), buffer, size - (buffer_width * 9));
            for (size_t i = size - (buffer_width * 9); i < size; i += 9)
            {
                buffer[i] = 0x20;
                Color* ffg = (Color*)(buffer + i + 1);
                Color* bbg = (Color*)(buffer + i + 5);
                ffg->R = fore_color.R; ffg->G = fore_color.G; ffg->B = fore_color.B; ffg->A = 255;
                bbg->R = back_color.R; bbg->G = back_color.G; bbg->B = back_color.B; bbg->A = 255;
            }
        }
    }

    // set terminal foreground color
    void term_set_fg(Color color) { fore_color = color; }

    // set terminal background color
    void term_set_bg(Color color) { back_color = color; }

    // set terminal foreground and background color
    void term_set_colors(Color fg, Color bg)
    {
        term_set_fg(fg);
        term_set_bg(bg);
    }

    // pack 2 4-bit colors into 1 byte
    uint8_t term_pack_colors(COL4 fg, COL4 bg) { return (uint8_t)((uint8_t)fg | ((uint8_t)bg << 4)); }

     // set size of terminal buffer
    void term_set_size(uint8_t w, uint8_t h)
    {
        if (w == 0 || h == 0) { return; }
        if (buffer != nullptr) { mem_free(buffer); }
        buffer_width = w;
        buffer_height = h;

        buffer = new uint8_t[buffer_width * buffer_height * 9];

        term_clear(Graphics::Colors::Black);

        debug_write("SET BUFFER SIZE TO ");
        debug_write_dec("", buffer_width);
        debug_write("x");
        debug_writeln_dec("", buffer_height);
    }
    
    void term_set_size_auto()
    {
        int32_t fw = font->GetWidth() + font->GetHorizontalSpacing();
        int32_t fh = font->GetHeight() + font->GetVerticalSpacing();

        uint32_t w = System::KernelIO::VESA.GetWidth() / fw;
        uint32_t h = System::KernelIO::VESA.GetHeight() / fh;
        term_set_size(w, h);
    }

    // set terminal buffer
    void term_set_buffer(uint8_t* buf)
    {
        
    }

    // set terminal cursor x and y position
    void term_set_cursor(uint8_t x, uint8_t y)
    {
        if (x < 0 || y < 0 || x >= buffer_width || y >= buffer_height) { return; }
        cursor_x = x;
        cursor_y = y;
    }

    // enable terminal cursor and set size
    void term_cursor_enable(uint8_t start, uint8_t end)
    {
        cursor_visible = true;
    }
    
    // disable terminal cursor
    void term_cursor_disable()
    {
        cursor_visible = false;
    }

    // set terminal cursor x position
    void term_set_cursor_x(uint8_t x) { term_set_cursor(x, cursor_y); }

    // set terminal cursor y position
    void term_set_cursor_y(uint8_t y) { term_set_cursor(cursor_x, y); }

    // get terminal cursor x position
    uint8_t term_get_cursor_x() { return cursor_x; }

    // get terminal cursor y position
    uint8_t term_get_cursor_y() { return cursor_y; }
}

namespace HAL
{
    // initialize terminal interface
    void TerminalManager::Initialize() { term_init(); }

    // register gui window
    void TerminalManager::RegisterWindow(void* win) { Window = win; }

    // clear the terminal
    void TerminalManager::Clear() { term_clear(back_color); }
    void TerminalManager::Clear(COL4 color) { term_clear(Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(color))); }

    // new line
    void TerminalManager::NewLine() { term_newline(); }

    // scroll
    void TerminalManager::Scroll() { term_scroll(1); }
    void TerminalManager::Scroll(uint32_t amount) { term_scroll(amount); }

    // delete
    void TerminalManager::Delete() { term_delete(); }
    void TerminalManager::Delete(uint32_t amount) { for (size_t i = 0; i < amount; i++) { term_delete(); } }

    // put character at position on screen
    void PutChar(uint16_t x, uint16_t y, char c, COL4 fg, COL4 bg) { term_put_char(x, y, c, Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(fg)), Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(bg))); }

    // write character to next position
    void TerminalManager::WriteChar(char c) { term_write_char(c); }
    void TerminalManager::WriteChar(char c, COL4 fg)
    {
        Color fg_old = fore_color;
        fore_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(fg));
        term_write_char(c);
        fore_color = fg_old;
    }
    void TerminalManager::WriteChar(char c, COL4 fg, COL4 bg)
    {
        Color fg_old = fore_color;
        Color bg_old = back_color;
        fore_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(fg));
        back_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(bg));
        term_write_char(c);
        fore_color = fg_old;
        back_color = bg_old;
    }
    void TerminalManager::WriteChar(char c, Color fg)
    {
         Color fg_old = fore_color;
        fore_color = fg; 
        term_write_char(c);
        fore_color = fg_old;
    }
    void TerminalManager::WriteChar(char c, Color fg, Color bg)
    {
          Color fg_old = fore_color;
        Color bg_old = back_color;
        fore_color = fg;
        back_color = bg;
        term_write_char(c);
        fore_color = fg_old;
        back_color = bg_old;
    }

    // write aligned char to position
    void TerminalManager::WriteChar(char c, int y, TEXT_ALIGN align) { }
    void TerminalManager::WriteChar(char c, int y, TEXT_ALIGN align, COL4 fg) { }
    void TerminalManager::WriteChar(char c, int y, TEXT_ALIGN align, COL4 fg, COL4 bg) { }

    // write string to next position
    void TerminalManager::Write(char* text) { term_write(text); }

    void TerminalManager::Write(char* text, COL4 fg)
    {
        for (size_t i = 0; i < strlen(text); i++) { WriteChar(text[i], fg); }
    }

    void TerminalManager::Write(char* text, COL4 fg, COL4 bg)
    {
        for (size_t i = 0; i < strlen(text); i++) { WriteChar(text[i], fg, bg); }
    }

    void TerminalManager::Write(char* text, Color fg) { term_write_ext(text, fg); }
    void TerminalManager::Write(char* text, Color fg, Color bg)
    {
        for (size_t i = 0; i < strlen(text); i++) { WriteChar(text[i], fg, bg); }
    }

    // write aligned string to position
    void TerminalManager::Write(char* text, int y, TEXT_ALIGN align) { }
    void TerminalManager::Write(char* text, int y, TEXT_ALIGN align, COL4 fg) { }
    void TerminalManager::Write(char* text, int y, TEXT_ALIGN align, COL4 fg, COL4 bg) { }

    // write line to next position
    void TerminalManager::WriteLine(char* text) { term_writeln(text); }
    void TerminalManager::WriteLine(char* text,int num) { term_writeln_dec(text, num); }
    void TerminalManager::WriteLine(char* text,int num, COL4 fg)
    {
        Write(text, fg);
        char temp[32];
        strdec(num, temp);
        Write(temp, fg);
        term_newline();
    }

    void TerminalManager::WriteLine(char* text,int num, COL4 fg, COL4 bg)
    {
        Write(text, fg, bg);
        char temp[32];
        strdec(num, temp);
        Write(temp, fg, bg);
        term_newline();
    }
    void TerminalManager::WriteLine(char* text, COL4 fg) { Write(text, fg); term_newline(); }
    void TerminalManager::WriteLine(char* text, COL4 fg, COL4 bg) { Write(text, fg, bg); term_newline(); }
    void TerminalManager::WriteLine(char* text, Color fg) { Write(text, fg); term_newline(); }
    void TerminalManager::WriteLine(char* text, Color fg, Color bg) { Write(text, fg, bg); term_newline(); }
    void TerminalManager::WriteLine(char* text, int num, Color fg)
    {
        Write(text, fg);
        char temp[32];
        strdec(num, temp);
        Write(temp, fg);
        term_newline();
    }

    void TerminalManager::WriteLine(char* text, int num, Color fg, Color bg)
    {
        Write(text, fg, bg);
        char temp[32];
        strdec(num, temp);
        Write(temp, fg, bg);
        term_newline();
    }

    // set cursor position
    void TerminalManager::SetCursorPos(uint16_t x, uint16_t y) { term_set_cursor(x, y); }
    void TerminalManager::SetCursorX(uint16_t x) { term_set_cursor_x(x); }
    void TerminalManager::SetCursorY(uint16_t y) { term_set_cursor_y(y); }

    // toggle cursor
    void TerminalManager::EnableCursor() { cursor_visible = true; }
    void TerminalManager::EnableCursor(uint8_t start, uint8_t end) { cursor_visible = true; }
    void TerminalManager::DisableCursor() { cursor_visible = false; }

    // get cursor position
    uint16_t TerminalManager::GetCursorX() { return cursor_x; }
    uint16_t TerminalManager::GetCursorY() { return cursor_y; }
    point_t TerminalManager::GetCursorPos() { return { cursor_x, cursor_y }; }

    // set colors
    void TerminalManager::SetColors(COL4 fg, COL4 bg)
    {
        fore_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(fg));
        back_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(bg));
    }
    void TerminalManager::SetColors(Color fg, Color bg) { fore_color = fg; back_color = bg; }
    void TerminalManager::SetColors(uint8_t packed_value) { }
    void TerminalManager::SetForeColor(COL4 color) { fore_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(color)); }
    void TerminalManager::SetBackColor(COL4 color) { back_color = Graphics::PackedValueToRGB(Graphics::VGAPaletteToRGB(color)); }
    void TerminalManager::SetForeColor(Color color) { fore_color = color; }
    void TerminalManager::SetBackColor(Color color) { back_color = color; }

    // get colors
    COL4 TerminalManager::GetForeColor() { return (COL4)Graphics::RGBToVGAPallete(fore_color); }
    COL4 TerminalManager::GetBackColor() { return (COL4)Graphics::RGBToVGAPallete(back_color); } 
}