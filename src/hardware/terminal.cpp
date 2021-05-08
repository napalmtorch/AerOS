#include "hardware/terminal.hpp"

extern "C"
{
    // buffer dimensions
    uint8_t buffer_width, buffer_height;

    // buffer offset
    uint8_t* buffer;

    // cursor
    bool cursor_visible;
    uint8_t cursor_x, cursor_y;

    // colors
    COL4 fore_color, back_color;

    // initialize terminal interface
    void term_init()
    {
        // initialize default buffer
        buffer_width = 80;
        buffer_height = 25;
        buffer = (uint8_t*)0xB8000;

        // initialize cursor
        cursor_visible = true;
        cursor_x = 0; cursor_y = 0;

        // initialize default colors
        fore_color = COL4_WHITE;
        back_color = COL4_BLACK;
    }

    // clear the terminal
    void term_clear(COL4 color)
    {
        // set back color
        back_color = color;

        // clear buffer
        for (size_t i = 0; i < (buffer_width * buffer_height) * 2; i += 2)
        {
            buffer[i] = 0x20;
            buffer[i + 1] = term_pack_colors(fore_color, color);
        }

        // reset cursor position
        term_set_cursor(0, 0);
    }

    // write character to terminal
    void term_write_char(char c)
    {
        // parse newline character
        if (c == '\n') { term_newline(); }
        // parse other characters
        else
        {
            // calculate offset
            uint32_t offset = 2 * (cursor_x + (cursor_y * buffer_width));
            // set data
            buffer[offset] = c;
            buffer[offset + 1] = term_pack_colors(fore_color, back_color);
            cursor_x++;
            // create new line when cursor passes screen width
            if (cursor_x >= buffer_width) { term_newline(); }
            // update cursor posotion
            term_set_cursor(cursor_x, cursor_y);
        }
    }

    // put specified character at position in terminal
    void term_put_char(uint8_t x, uint8_t y, char c, COL4 fg, COL4 bg)
    {
        // check if position is out of bounds
        if (x >= buffer_width || y >= buffer_height) { return; }
        // calculate offset
        uint32_t offset = 2 * (cursor_x + (cursor_y * buffer_width));
        // set data
        buffer[offset] = c;
        buffer[offset + 1] = term_pack_colors(fg, bg);
    }

    // write string to terminal
    void term_write(char* text)
    {
        for (size_t i = 0; i < strlen(text); i++) { term_write_char(text[i]); }
    }

    // write string to terminal with foreground color
    void term_write_ext(char* text, COL4 color)
    {
        COL4 old = fore_color;
        fore_color = color;
        term_write(text);
        fore_color = old;
    }

    // write string and decimal value to terminal
    void term_write_dec(char* text, int32_t num)
    {
        char temp[16];
        strdec(num, temp);
        term_write(text);
        term_write(temp);
    }

    // write string and hexadecimal value to terminal 
    void term_write_hex(char* text, uint32_t num)
    {
        char temp[16];
        strhex32(num, temp);
        term_write(text);
        term_write(temp);
    }

    // write line to terminal
    void term_writeln(char* text)
    {
        term_write(text);
        term_write_char('\n');
    }

    // write line to terminal with foreground color
    void term_writeln_ext(char* text, COL4 color)
    {
        COL4 old = fore_color;
        fore_color = color;
        term_writeln(text);
        fore_color = old;
    }

    // write line and decimal value to terminal
    void term_writeln_dec(char* text, int32_t num)
    {
        char temp[16];
        strdec(num, temp);
        term_write(text);
        term_writeln(temp);
    }

    // write line and hexadecimal value to terminal 
    void term_writeln_hex(char* text, uint32_t num)
    {
        char temp[16];
        strhex32(num, temp);
        term_write(text);
        term_writeln(temp);
    }

    // delete last character in terminal
    void term_delete()
    {
        // clear previous character
        cursor_x--; term_write_char(' '); cursor_x--;
        // backup cursor
        term_set_cursor(cursor_x, cursor_y);
    }

    // generate new line in terminal
    void term_newline()
    {
        // increment position
        cursor_x = 0; cursor_y++;
        // cursor position is larger than buffer height
        if (cursor_y >= buffer_height) { term_scroll(1); cursor_y--; }
        // backup cursor
        term_set_cursor(cursor_x, cursor_y);
    }

    // scroll terminal by amount of lines
    void term_scroll(uint32_t amount)
    {
        for (size_t i = 0; i < amount; i++)
        {
            // move buffer up 1 line
            mem_copy((uint8_t*)(buffer + (buffer_width * 2)), buffer, (buffer_width * buffer_height * 2) - (buffer_width * 2));
            // clear bottom line
            for (size_t j = (buffer_width * buffer_height * 2) - (buffer_width * 2); j < (buffer_width * buffer_height * 2); j += 2)
            {
                buffer[j] = 0x20;
                buffer[j + 1] = term_pack_colors(COL4_WHITE, back_color);
            }
        }
    }

    // set terminal foreground color
    void term_set_fg(COL4 color) { fore_color = color; }

    // set terminal background color
    void term_set_bg(COL4 color) { back_color = color; }

    // set terminal foreground and background color
    void term_set_colors(COL4 fg, COL4 bg)
    {
        fore_color = fg;
        back_color = bg;
    }

    // pack 2 4-bit colors into 1 byte
    uint8_t term_pack_colors(COL4 fg, COL4 bg) { return (uint8_t)((uint8_t)fg | ((uint8_t)bg << 4)); }

     // set size of terminal buffer
    void term_set_size(uint8_t w, uint8_t h)
    {
        buffer_width = w;
        buffer_height = h;
    }

    // set terminal buffer
    void term_set_buffer(uint8_t* buf)
    {
        buffer = buf;
    }

    // set terminal cursor x and y position
    void term_set_cursor(uint8_t x, uint8_t y)
    {
        cursor_x = x; cursor_y = y;
        uint32_t offset = x + (y * buffer_width);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t)(offset & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t)((offset >> 8) & 0xFF));
    }

    // enable terminal cursor and set size
    void term_cursor_enable(uint8_t start, uint8_t end)
    {
        outb(0x3D4, 0x0A);
        outb(0x3D5, (inb(0x3D5) & 0xC0) | start);
    
        outb(0x3D4, 0x0B);
        outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
    }
    
    // disable terminal cursor
    void term_cursor_disable()
    {
        outb(0x3D4, 0x0A);
	    outb(0x3D5, 0x20);
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

    // clear the terminal
    void TerminalManager::Clear() { term_clear(back_color); }
    void TerminalManager::Clear(COL4 color) { term_clear(color); }

    // new line
    void TerminalManager::NewLine() { term_newline(); }

    // scroll
    void TerminalManager::Scroll() { term_scroll(1); }
    void TerminalManager::Scroll(uint32_t amount) { term_scroll(amount); }

    // delete
    void TerminalManager::Delete() { term_delete(); }

    // delete amount
    void TerminalManager::Delete(uint32_t amount)
    {
        for (size_t i = 0; i < amount; i++) { term_delete(); }
    }

    // put character at position on screen
    void TerminalManager::PutChar(uint16_t x, uint16_t y, char c, COL4 fg, COL4 bg) { term_put_char(x, y, c, fg, bg); }

    // write character to next position
    void TerminalManager::WriteChar(char c) { term_write_char(c); }

    // write character to next position with foreground color
    void TerminalManager::WriteChar(char c, COL4 fg) { WriteChar(c, fg, back_color); }

    // write character to next position with foreground and background color
    void TerminalManager::WriteChar(char c, COL4 fg, COL4 bg)
    {
        COL4 fg_old = fore_color;
        COL4 bg_old = back_color;
        term_set_colors(fg, bg);
        term_write_char(c);
        term_set_colors(fg_old, bg_old);
    }

    // write aligned char to position
    void TerminalManager::WriteChar(char c, int y, TEXT_ALIGN align) { WriteChar(c, y, align, fore_color, back_color); }

    void TerminalManager::WriteChar(char c, int y, TEXT_ALIGN align, COL4 fg) { WriteChar(c, y, align, fg, back_color); }

    void TerminalManager::WriteChar(char c, int y, TEXT_ALIGN align, COL4 fg, COL4 bg)
    {
        uint8_t old_x = cursor_x, old_y = cursor_y;
        if (align == TEXT_ALIGN_LEFT) { cursor_x = 0; }
        else if (align == TEXT_ALIGN_CENTER) { cursor_x = (buffer_width / 2) - 4; }
        else if (align == TEXT_ALIGN_RIGHT) { cursor_x = buffer_width - 1; }
        PutChar(cursor_x, y, c, fg, bg);
        term_set_cursor(old_x, old_y);
    }

    // write string to next position
    void TerminalManager::Write(char* text) { term_write(text); }

    // write string to next position with foreground color
    void TerminalManager::Write(char* text, COL4 fg) { term_write_ext(text, fg); }

    // write string to next position with foreground and background color
    void TerminalManager::Write(char* text, COL4 fg, COL4 bg)
    {
        COL4 fg_old = fore_color;
        COL4 bg_old = back_color;
        term_set_colors(fg, bg);
        term_write(text);
        term_set_colors(fg_old, bg_old);
    }

    // write aligned string to position
    void TerminalManager::Write(char* text, int y, TEXT_ALIGN align) { Write(text, y, align, fore_color, back_color); }

    void TerminalManager::Write(char* text, int y, TEXT_ALIGN align, COL4 fg) { Write(text, y, align, fg, back_color); }

    void TerminalManager::Write(char* text, int y, TEXT_ALIGN align, COL4 fg, COL4 bg)
    {
        uint8_t old_x = cursor_x, old_y = cursor_y;
        uint32_t xx;
        if (align == TEXT_ALIGN_LEFT) { xx = 0; }
        else if (align == TEXT_ALIGN_CENTER) { xx = (buffer_width / 2) - (strlen(text) / 2); }
        else if (align == TEXT_ALIGN_RIGHT) { xx = buffer_width - 1; }
        term_set_cursor(xx, y);
        for (size_t i = 0; i < strlen(text); i++) { WriteChar(text[i], fg, bg); }
        term_set_cursor(old_x, old_y);
    }

    // write line to next position
    void TerminalManager::WriteLine(char* text) { term_writeln(text); }

    // write line to next position with foreground color
    void TerminalManager::WriteLine(char* text, COL4 fg) { term_writeln_ext(text, fg); }

    // write line to next position with foreground and background color
    void TerminalManager::WriteLine(char* text, COL4 fg, COL4 bg)
    {
        COL4 fg_old = fore_color;
        COL4 bg_old = back_color;
        term_set_colors(fg, bg);
        term_writeln(text);
        term_set_colors(fg_old, bg_old);
    }

    // set cursor position
    void TerminalManager::SetCursorPos(uint16_t x, uint16_t y) { term_set_cursor(x, y); }
    void TerminalManager::SetCursorX(uint16_t x) { term_set_cursor_x(x); }
    void TerminalManager::SetCursorY(uint16_t y) { term_set_cursor_y(y); }

    // get cursor position
    uint16_t TerminalManager::GetCursorX() { return cursor_x; }
    uint16_t TerminalManager::GetCursorY() { return cursor_y; }
    point_t TerminalManager::GetCursorPos() { return { cursor_x, cursor_y}; }

    // toggle cursor
    void TerminalManager::EnableCursor() { term_cursor_enable(0, 15); }
    void TerminalManager::EnableCursor(uint8_t start, uint8_t end) { term_cursor_enable(start, end); }
    void TerminalManager::DisableCursor() { term_cursor_disable(); }

    // set colors
    void TerminalManager::SetColors(COL4 fg, COL4 bg) { term_set_colors(fg, bg); }
    void TerminalManager::SetColors(uint8_t packed_value) { /* not yet implemented */ }
    void TerminalManager::SetForeColor(COL4 color) { term_set_fg(color); }
    void TerminalManager::SetBackColor(COL4 color) { term_set_bg(color); }

    // get colors
    COL4 TerminalManager::GetForeColor() { return fore_color; }
    COL4 TerminalManager::GetBackColor() { return back_color; }
}