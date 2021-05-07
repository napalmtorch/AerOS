#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/memory.hpp"

extern "C"
{
    // initialize terminal interface
    void term_init();

    // clear the terminal
    void term_clear(COL4 color);

    // write character to terminal
    void term_write_char(char c);

    // put specified character at position in terminal
    void term_put_char(uint8_t x, uint8_t y, char c, COL4 fg, COL4 bg);

    // write string to terminal
    void term_write(char* text);

    // write string to terminal with foreground color
    void term_write_ext(char* text, COL4 color);

    // write string and decimal value to terminal
    void term_write_dec(char* text, int32_t num);

    // write string and hexadecimal value to terminal 
    void term_write_hex(char* text, uint32_t num);

    // write line to terminal
    void term_writeln(char* text);

    // write string to terminal with foreground color
    void term_writeln_ext(char* text, COL4 color);

    // write string and decimal value to terminal
    void term_writeln_dec(char* text, int32_t num);

    // write string and hexadecimal value to terminal 
    void term_writeln_hex(char* text, uint32_t num);

    // delete last character in terminal
    void term_delete();

    // generate new line in terminal
    void term_newline();

    // scroll terminal by amount of lines
    void term_scroll(uint32_t amount);

    // set terminal foreground color
    void term_set_fg(COL4 color);

    // set terminal background color
    void term_set_bg(COL4 color);

    // set terminal foreground and background color
    void term_set_colors(COL4 fg, COL4 bg);

    // pack 2 4-bit colors into 1 byte
    uint8_t term_pack_colors(COL4 fg, COL4 bg);

    // set size of terminal buffer
    void term_set_size(uint8_t w, uint8_t h);

    // set terminal buffer
    void term_set_buffer(uint8_t* buf);

    // set terminal cursor x and y position
    void term_set_cursor(uint8_t x, uint8_t y);

    // enable terminal cursor and set size
    void term_cursor_enable(uint8_t start, uint8_t end);
    
    // disable terminal cursor
    void term_cursor_disable();

    // set terminal cursor x position
    void term_set_cursor_x(uint8_t x);

    // set terminal cursor y position
    void term_set_cursor_y(uint8_t y);

    // get terminal cursor x position
    uint8_t term_get_cursor_x();

    // get terminal cursor y position
    uint8_t term_get_cursor_y();
}

namespace HAL
{
    class TerminalManager
    {
        public:
            // initialize terminal interface
            void Initialize();

            // clear the terminal
            void Clear();
            void Clear(COL4 color);

            // new line
            void NewLine();

            // scroll
            void Scroll();
            void Scroll(uint32_t amount);

            // delete
            void Delete();
            void Delete(uint32_t amount);

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

            // toggle cursor
            void EnableCursor();
            void EnableCursor(uint8_t start, uint8_t end);
            void DisableCursor();

            // get cursor position
            uint16_t GetCursorX();
            uint16_t GetCursorY();
            point_t GetCursorPos();

            // set colors
            void SetColors(COL4 fg, COL4 bg);
            void SetColors(uint8_t packed_value);
            void SetForeColor(COL4 color);
            void SetBackColor(COL4 color);

            // get colors
            COL4 GetForeColor();
            COL4 GetBackColor(); 
            uint8_t cursor_x, cursor_y;
    };
}