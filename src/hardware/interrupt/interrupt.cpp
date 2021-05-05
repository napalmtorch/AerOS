#include "hardware/interrupt/interrupt.hpp"

namespace HAL
{
    namespace CPU
    {
        // enable interrupts
        void EnableInterrupts() { __asm__ __volatile__("sti"); }

        // disable interrupts
        void DisableInterrupts() { __asm__ __volatile__("cli"); }

        // register interrupt handler
        void RegisterIRQ(uint8_t irq, isr_t handler) { isr_register(irq, handler); }

        // initialize pit
        void InitializePIT(uint32_t freq, void (*callback)(registers_t))
        {
            RegisterIRQ(IRQ0, (isr_t)callback);
            uint32_t divisor = 1193180 / freq;
            uint8_t low = (uint8_t)(divisor & 0xFF);
            uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
            outb(0x43, 0x36);
            outb(0x40, low);
            outb(0x40, high);
        }
    }
}