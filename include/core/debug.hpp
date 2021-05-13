#pragma once
#include "lib/types.h"
#include "graphics/colors.hpp"
#include "hardware/terminal.hpp"

extern "C"
{
    #include "lib/types.h"
    #include "lib/string.hpp"
    #include "hardware/drivers/serial.hpp"
    
    // system message types
    typedef enum
    {
        MSG_TYPE_OK,
        MSG_TYPE_SYSTEM,
        MSG_TYPE_WARNING,
        MSG_TYPE_ERROR,
    } MSG_TYPE;

    // debug flags
    extern bool debug_serial_enabled;
    extern bool debug_console_enabled;

    //bochs breakpoint
    void debug_bochs_break();

    // messages
    void debug_throw_message(MSG_TYPE type, char* msg);
    void debug_throw_panic(char* msg);

    // write text
    void debug_write(char* text);
    void debug_write_ext(char* text, COL4 color);
    void debug_write_dec(char* text, int32_t num);
    void debug_write_hex(char* text, uint32_t num);
    void debug_writeln(char* text);
    void debug_writeln_ext(char* text, COL4 color);
    void debug_writeln_dec(char* text, int32_t num);
    void debug_writeln_hex(char* text, uint32_t num);
}

namespace System
{
    namespace KernelIO
    {
        // debugging
        void ThrowMessage(MSG_TYPE type, char* msg);
        void ThrowOK(char* msg);
        void ThrowSystem(char* msg);
        void ThrowError(char* msg);
        void ThrowWarning(char* msg);
        void ThrowPanic(char* msg);
        void Write(char* text);
        void Write(char* text, COL4 color);
        void WriteDecimal(char* text, int32_t num);
        void WriteHex(char* text, uint32_t num);
        void WriteLine(char* text);
        void WriteLine(char* text, COL4 color);
        void WriteLineDecimal(char* text, int32_t num);
        void WriteLineHex(char* text, uint32_t num);
        void DumpMemory(uint8_t* src, uint32_t len, uint8_t bytes_per_line, bool ascii);
        void DumpMemoryTerminal(uint8_t* src, uint32_t len, uint8_t bytes_per_line, bool ascii);
        void SetDebugConsoleOutput(bool value);
        void SetDebugSerialOutput(bool value);
    }
}