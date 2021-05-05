#include "hardware/drivers/serial.hpp"

extern "C"
{
    // active serial port
    uint16_t serial_port;

    // set active serial port
    void serial_set_port(SERIAL_PORT port)
    {
        serial_port = (uint16_t)port;

        outb(serial_port + 1, 0x00);    // Disable all interrupts
        outb(serial_port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        outb(serial_port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        outb(serial_port + 1, 0x00);    // (hi byte)
        outb(serial_port + 3, 0x03);    // 8 bits, no parity, one stop bit
        outb(serial_port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        outb(serial_port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        outb(serial_port + 4, 0x1E);    // Set in loopback mode, test the serial chip
        outb(serial_port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
        
        // Check if serial is faulty (i.e: not same byte as sent)
        if(inb(serial_port + 0) != 0xAE) { /* unable to initialize */ return; }
        
        // If serial is not faulty set it in normal operation mode
        outb(serial_port + 4, 0x0F);
    }

    // get active serial port
    SERIAL_PORT serial_get_port() { return (SERIAL_PORT)serial_port; }

    // check if serial port has recieved data
    uint8_t serial_has_recieved() { return inb(serial_port + 5) & 1; }
    
    // check if serial port is able to send data
    uint8_t serial_can_send() { return inb(serial_port + 5) & 0x20; }

    // read character from serial port
    char serial_read()
    {
        if (serial_port != SERIAL_PORT_COM1 && serial_port != SERIAL_PORT_COM2 && 
            serial_port != SERIAL_PORT_COM3 && serial_port != SERIAL_PORT_COM4) { return 0; }
        while (serial_has_recieved() == 0);
        return inb(serial_port);
    }

    // write character to serial port
    void serial_write_char(char c)
    {
        if (serial_port != SERIAL_PORT_COM1 && serial_port != SERIAL_PORT_COM2 && 
            serial_port != SERIAL_PORT_COM3 && serial_port != SERIAL_PORT_COM4) { return; }
        while (serial_can_send() == 0);
        outb(serial_port, c);
    }

    // write string of text to serial port
    void serial_write(char* text)
    {
        for (size_t i = 0; i < strlen(text); i++) { serial_write_char(text[i]); }
    }

    // write string of text with foreground color to serial port
    void serial_write_ext(char* text, uint8_t color)
    {
        serial_set_color(color);
        serial_write(text);
        serial_set_color(0xFF);
    }

    // write line of text to serial port
    void serial_writeln(char* text)
    {
        serial_write(text);
        serial_write_char('\n');
    }

    // write line of text with foreground color to serial port
    void serial_writeln_ext(char* text, uint8_t color)
    {
        serial_set_color(color);
        serial_writeln(text);
        serial_set_color(0xFF);
    }

    // set active foreground color of serial port
    void serial_set_color(uint8_t color)
    {
        switch (color)
        {
            case 0x00: { serial_write("\u001b[30m"); break; }
            case 0x01: { serial_write("\u001b[34m"); break; }
            case 0x02: { serial_write("\u001b[32m"); break; }
            case 0x03: { serial_write("\u001b[36m"); break; }
            case 0x04: { serial_write("\u001b[31m"); break; }
            case 0x05: { serial_write("\u001b[35m"); break; }
            case 0x06: { serial_write("\u001b[33m"); break; }
            case 0x07: { serial_write("\u001b[37m"); break; }
            case 0x08: { serial_write("\u001b[37m"); break; }
            case 0x09: { serial_write("\u001b[34m"); break; }
            case 0x0A: { serial_write("\u001b[32m"); break; }
            case 0x0B: { serial_write("\u001b[36m"); break; }
            case 0x0C: { serial_write("\u001b[31m"); break; }
            case 0x0D: { serial_write("\u001b[35m"); break; }
            case 0x0E: { serial_write("\u001b[33m"); break; }
            case 0x0F: { serial_write("\u001b[37m"); break; }
            case 0xFF: { serial_write("\u001b[0m"); break; }
            default: { serial_write("\u001b[0m"); break; }
        }
    }
}

namespace HAL
{
    // set active serial port
    void SerialPort::SetPort(SERIAL_PORT port) { serial_set_port(port); }

    // get active serial port
    SERIAL_PORT SerialPort::GetPort() { return serial_get_port(); }

    // read character from serial port
    char SerialPort::Read() { return serial_read(); }

    // write character to serial port
    void SerialPort::WriteChar(char c) { serial_write_char(c); }

    // write string of text to serial port
    void SerialPort::Write(char* text) { serial_write(text); }

    // write string of text with foreground color to serial port
    void SerialPort::Write(char* text, uint8_t color) { serial_write_ext(text, color); }

    // write line of text to serial port
    void SerialPort::WriteLine(char* text) { serial_writeln(text); }

    // write line of text with foreground color to serial port
    void SerialPort::WriteLine(char* text, uint8_t color) { serial_writeln_ext(text, color); }

    // set active foreground color of serial port
    void SerialPort::SetColor(uint8_t color) { serial_set_color(color); }

    // check if serial port has recieved data
    uint8_t SerialPort::HasRecieved() { return serial_has_recieved(); }

    // check if serial port is able to send data
    uint8_t SerialPort::CanSend() { return serial_can_send(); }
}