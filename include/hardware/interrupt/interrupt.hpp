#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/interrupt/idt.hpp"
#include "hardware/interrupt/isr.hpp"

namespace HAL
{
    namespace CPU
    {        
        // enable interrupts
        void EnableInterrupts();

        // disable interrupts
        void DisableInterrupts();

        // register interrupt handler
        void RegisterIRQ(uint8_t irq, isr_t handler);

        // initialize pit
        void InitializePIT(uint32_t freq, void (*callback)(registers_t));
    }
}