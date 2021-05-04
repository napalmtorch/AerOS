#pragma once
#include "lib/types.h"

    namespace System
    {
        namespace KernelIO
        {
            // system message types
            typedef enum
            {
                MSG_TYPE_OK,
                MSG_TYPE_SYSTEM,
                MSG_TYPE_WARNING,
                MSG_TYPE_ERROR,
            } MSG_TYPE;

            // debugging
            void ThrowMessage(MSG_TYPE type, char* msg);
            void ThrowOK(char* msg);
            void ThrowSystem(char* msg);
            void ThrowError(char* msg);
            void ThrowWarning(char* msg);
            void ThrowPanic(char* msg);
            void Write(char* text);
            void Write(char* text, uint8_t color);
            void WriteLine(char* text);
            void WriteLine(char* text, uint8_t color);
        }
    }

extern "C"
{
    // system message types
    typedef enum
    {
        MSG_TYPE_OK,
        MSG_TYPE_SYSTEM,
        MSG_TYPE_WARNING,
        MSG_TYPE_ERROR,
    } MSG_TYPE;

    // debug flags
    extern uint8_t debug_serial_enabled;
    extern uint8_t debug_console_enabled;

    // messages
    void debug_throw_message(MSG_TYPE type, char* msg);
    void debug_throw_panic(char* msg);

    // write text
    void debug_write(char* text);
    void debug_write_ext(char* text, uint8_t color);
    void debug_writeln(char* text);
    void debug_writeln_ext(char* text, uint8_t color);
}