#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"

extern "C"
{
    // clear the terminal
    void term_clear(COL4 color);

    // write character to terminal
    void term_write_char(char c);

    // put specified character at position in terminal
    void term_put_char(uint8_t x, uint8_t y, char c, COL4 fg, COL4 bg);

    // write string to terminal
    void term_write(char* text);

    // write line to terminal
    void term_writeln(char* text);

    // delete last character in terminal
    void term_delete();

    // set terminal foreground color
    void term_set_fg(COL4 color);

    // set terminal background color
    void term_set_bg(COL4 color);

    // set terminal cursor x and y position
    void term_set_cursor(uint8_t x, uint8_t y);

    // set terminal cursor x position
    void term_set_cursor_x(uint8_t x);

    // set terminal cursor y position
    void term_set_cursor_y(uint8_t y);

    // get terminal cursor x position
    uint8_t term_get_cursor_x();

    // get terminal cursor y position
    uint8_t term_get_cursor_y();
}