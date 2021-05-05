#include "core/debug.hpp"

extern "C"
{
    // flag indicating whether to print debug messages to serial port
    uint8_t debug_serial_enabled = true;

    // flag indicating whether to print debug messages to console
    uint8_t debug_console_enabled = true;

    // throw message of specified type
    void debug_throw_message(MSG_TYPE type, char* msg)
    {

    }

    // throw kernel panic message
    void debug_throw_panic(char* msg)
    {
        // stop execution
        __asm__ __volatile__("cli");
        __asm__ __volatile__("hlt");
    }

    // write string of text
    void debug_write(char* text) { debug_write_ext(text, 0xF); }

    // write string of text with foreground color
    void debug_write_ext(char* text, uint8_t color)
    {

    }

    // write line of text
    void debug_writeln(char* text) { debug_writeln_ext(text, 0xF); }

    // writ eline of text with foreground color
    void debug_writeln_ext(char* text, uint8_t color)
    {

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
        void Write(char* text) { }

        // write string of text with foreground color
        void Write(char* text, uint8_t color) { }

        // write line of text
        void WriteLine(char* text) { }

        // write line of text with foreground color
        void WriteLine(char* text, uint8_t color) { }
    }
}