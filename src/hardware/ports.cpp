#include "hardware/ports.hpp"

// port byte in
uint8_t inb(uint16_t port)
{
    uint8_t result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

// port byte out
uint16_t inw(uint16_t port)
{
    uint16_t result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

// port long int
uint32_t inl(uint16_t port)
{
    uint32_t rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
    return rv;
}

// port byte out
void outb(uint16_t port, uint8_t data) { __asm__ __volatile__("out %%al, %%dx" : : "a" (data), "d" (port)); }

// port word out
void outw(uint16_t port, uint16_t data) { __asm__ __volatile__("out %%ax, %%dx" : : "a" (data), "d" (port)); }

// port long out
void outl(uint16_t port, uint32_t data) { __asm__ __volatile__("outl %%eax, %%dx" : : "dN" (port), "a" (data)); }