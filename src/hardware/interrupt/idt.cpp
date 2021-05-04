#include "hardware/interrupt/idt.hpp"

extern "C"
{
    #define IDT_ENTRIES 256
    idt_gate_t idt[IDT_ENTRIES];
    idt_register_t idt_reg;

    void idt_set_gate(int32_t n, uint32_t handler)
    {
        idt[n].low_offset = bitutil_low_16(handler);
        idt[n].sel = KERNEL_CS;
        idt[n].always0 = 0;
        idt[n].flags = 0x8E;
        idt[n].high_offset = bitutil_high_16(handler);
    }

    void idt_set()
    {
        idt_reg.base = (uint32_t)&idt;
        idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
        __asm__ __volatile("lidtl (%0)" : : "r" (&idt_reg));
    }
}