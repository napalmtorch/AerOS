#include "core/debug.hpp"

extern "C"
{
    // flag indicating whether to print debug messages to serial port
    bool debug_serial_enabled = true;

    // flag indicating whether to print debug messages to console
    bool debug_console_enabled = true;

    // throw message of specified type
    void debug_throw_message(MSG_TYPE type, char* msg)
    {
        char header[12] = { '\0' };
        uint8_t fg = 0xF;

        // determine header text and foreground color
        switch (type)
        {
            case MSG_TYPE_OK:       { strcat(header, "  OK   "); fg = 0xA; break; }
            case MSG_TYPE_SYSTEM:   { strcat(header, "  SYS  "); fg = 0xB; break; }
            case MSG_TYPE_WARNING:  { strcat(header, "WARNING"); fg = 0xE; break; }
            case MSG_TYPE_ERROR:    { strcat(header, " ERROR "); fg = 0xC; break; }
        }

        // print to console if enabled
        if (debug_console_enabled)
        {
            /* TODO: not yet implemented */
        }

        // print to serial if enablex
        if (debug_serial_enabled)
        {
            serial_write("[");
            serial_write_ext(header, (COL4)fg);
            serial_write("] ");
            serial_writeln(msg);
        }
    }

    // throw kernel panic message
    void debug_throw_panic(char* msg)
    {
        /* TODO: implement proper panic screen */

        // temporary solution
        debug_throw_message(MSG_TYPE_ERROR, msg);

        // stop execution
        __asm__ __volatile__("cli");
        __asm__ __volatile__("hlt");
    }

    // write string of text
    void debug_write(char* text) { debug_write_ext(text, COL4_WHITE); }

    // write string of text with foreground color
    void debug_write_ext(char* text, COL4 color)
    {
        // write text to console if enabled
        if (debug_console_enabled)
        {
            /* TODO: not yet implemented */
        }

        // write text to serial if enabled
        if (debug_serial_enabled) { serial_write_ext(text, color); }
    }

    // write string and decimal value
    void debug_write_dec(char* text, int32_t num)
    {
        debug_write(text);
        char temp[16];
        strdec(num, temp);
        debug_write(temp);
    }

    // write string and hexadecimal value
    void debug_write_hex(char* text, uint32_t num)
    {
        debug_write(text);
        char temp[8];
        strhex32(num, temp);
        debug_write(temp);
    }

    // write line of text
    void debug_writeln(char* text) { debug_writeln_ext(text, COL4_WHITE); }

    // write line of text with foreground color
    void debug_writeln_ext(char* text, COL4 color)
    {
        // write text to console if enabled
        if (debug_console_enabled)
        {
            /* TODO: not yet implemented */
        }

        // write text to serial if enabled
        if (debug_serial_enabled) { serial_writeln_ext(text, color); }
    }

    // write line of text and decimal number
    void debug_writeln_dec(char* text, int32_t num)
    {
        debug_write(text);
        char temp[16];
        strdec(num, temp);
        debug_writeln(temp);
    }

    // write line of text and hexadecimal number
    void debug_writeln_hex(char* text, uint32_t num)
    {
        debug_write(text);
        char temp[8];
        strhex32(num, temp);
        debug_writeln(temp);
    }
}

namespace System
{
    namespace KernelIO
    {
        // throw message of specified type
        void ThrowMessage(MSG_TYPE type, char* msg) { debug_throw_message(type, msg); }

        // throw ok message
        void ThrowOK(char* msg) { debug_throw_message(MSG_TYPE_OK, msg); }

        // throw system message
        void ThrowSystem(char* msg) { debug_throw_message(MSG_TYPE_SYSTEM, msg); }

        // throw error message
        void ThrowError(char* msg) { debug_throw_message(MSG_TYPE_ERROR, msg); }

        // throw warning message
        void ThrowWarning(char* msg) { debug_throw_message(MSG_TYPE_WARNING, msg); }

        // throw kernel panic message
        void ThrowPanic(char* msg) { debug_throw_panic(msg); }

        // write string of text
        void Write(char* text) { debug_write(text); }

        // write string of text with foreground color
        void Write(char* text, COL4 color) { debug_write_ext(text, color); }

        // write string of text with decimal number
        void WriteDecimal(char* text, int32_t num) { debug_write_dec(text, num); }

        // write string of text with hexadecimal number
        void WriteHex(char* text, uint32_t num) { debug_write_hex(text, num); }

        // write line of text
        void WriteLine(char* text) { debug_writeln(text); }

        // write line of text with foreground color
        void WriteLine(char* text, COL4 color) { debug_writeln_ext(text, color); }

        // write line of text with decimal number
        void WriteLineDecimal(char* text, int32_t num) { debug_writeln_dec(text, num); }

        // write line of text with hexadecimal number
        void WriteLineHex(char* text, uint32_t num) { debug_writeln_hex(text, num); }
    }
}