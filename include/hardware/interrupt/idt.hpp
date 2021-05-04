#pragma once
#include "lib/types.h"

extern "C"
{
    #define KERNEL_CS 0x08

    typedef struct 
    {
        uint16_t low_offset; 
        uint16_t sel;
        uint8_t always0;
        uint8_t flags; 
        uint16_t high_offset;
    } __attribute__((packed)) idt_gate_t;

    /* A pointer to the array of interrupt handlers.
    * Assembly instruction 'lidt' will read it */
    typedef struct 
    {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) idt_register_t;

    /* Functions implemented in idt.c */
    void idt_set_gate(int32_t n, uint32_t handler);
    void idt_set();
}