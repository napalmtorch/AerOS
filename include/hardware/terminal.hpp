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