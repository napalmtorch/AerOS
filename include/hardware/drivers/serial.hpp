#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/ports.hpp"

extern "C"
{
    #include "lib/types.h"
    #include "hardware/ports.hpp"

    // available serial ports
    typedef enum 
    {
        SERIAL_PORT_COM1 = 0x3F8,
        SERIAL_PORT_COM2 = 0x2F8,
        SERIAL_PORT_COM3 = 0x3E8,
        SERIAL_PORT_COM4 = 0x2E8,
    } SERIAL_PORT;

    // set active serial port
    void serial_set_port(SERIAL_PORT port);

    // get active serial port
    SERIAL_PORT serial_get_port();

    // check if serial port has recieved data
    uint8_t serial_has_recieved();
    
    // check if serial port is able to send data
    uint8_t serial_can_send();

    // read character from serial port
    char serial_read();

    // write character to serial port
    void serial_write_char(char c);

    // write string of text to serial port
    void serial_write(char* text);

    // write string of text with foreground color to serial port
    void serial_write_ext(char* text, uint8_t color);

    // write line of text to serial port
    void serial_writeln(char* text);

    // write line of text with foreground color to serial port
    void serial_writeln_ext(char* text, uint8_t color);

    // set active foreground color of serial port
    void serial_set_color(uint8_t color);
}

namespace HAL
{
    class SerialPort
    {
        public:
            // set active serial port
            void SetPort(SERIAL_PORT port);

            // get active serial port
            SERIAL_PORT GetPort();

            // read character from serial port
            char Read();

            // write character to serial port
            void WriteChar(char c);

            // write string of text to serial port
            void Write(char* text);

            // write string of text with foreground color to serial port
            void Write(char* text, uint8_t color);

            // write line of text to serial port
            void WriteLine(char* text);

            // write line of text with foreground color to serial port
            void WriteLine(char* text, uint8_t color);

            // set active foreground color of serial port
            void SetColor(uint8_t color);

        private:
            // check if serial port has recieved data
            uint8_t HasRecieved();
            
            // check if serial port is able to send data
         uint8_t CanSend();
    };
}