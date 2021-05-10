#pragma once
#include "lib/types.h"
#include "core/debug.hpp"
// c++ header definition
#ifdef __cplusplus
    extern "C"
    {
        // in
        uint8_t inb(uint16_t port);
        uint16_t inw(uint16_t port);
        uint32_t inl(uint16_t port);

        // out
        void outb(uint16_t port, uint8_t data);
        void outw(uint16_t port, uint16_t data);
        void outl(uint16_t port, uint32_t data);
        void insw(unsigned short port, unsigned char * data, unsigned long size);
        void outsw(unsigned short port, unsigned char * data, unsigned long size);
    }

    class IOPort
    {
        private:
            // port number
            uint16_t Port;
        public:
            // constructor - default
            IOPort();

            // constructor - selected port
            IOPort(uint16_t port);

            // write 8-bit value to port
            void WriteInt8(uint8_t data);

            // write 16-bit value to port
            void WriteInt16(uint16_t data);

            // write 32-bit value to port
            void WriteInt32(uint32_t data);

            // read 8-bit value from port
            uint8_t ReadInt8();

            // read 16-bit value from port
            uint16_t ReadInt16();

            // read 32-bit value from port
            uint32_t ReadInt32();

            // get port number
            uint16_t GetPort();
    };

// c header definition
#else
     // in
    uint8_t inb(uint16_t port);
    uint16_t inw(uint16_t port);
    uint32_t inl(uint16_t port);

    // out
    void outb(uint16_t port, uint8_t data);
    void outw(uint16_t port, uint16_t data);
    void outl(uint16_t port, uint32_t data);
#endif