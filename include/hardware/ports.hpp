#pragma once
#include "lib/types.h"

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
    }

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