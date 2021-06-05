#pragma once
#include "lib/types.h"
#include "hardware/interrupt/idt.hpp"
#include "hardware/interrupt/isr.hpp"

namespace HAL
{
    namespace CPU
    {
        // total amount of ticks
        extern uint32_t Ticks;

        void GetCPUInfo(uint32_t reg, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
        void Detect();
        bool IsSSE();
        bool IsMMX();

        // enable interrupts
        void EnableInterrupts();

        // disable interrupts
        void DisableInterrupts();

        // initialize interrupt service routines
        void InitializeISRs();

        // register interrupt handler
        void RegisterIRQ(uint8_t irq, isr_t handler);

        // initialize pit
        void InitializePIT(uint32_t freq, void (*callback)(registers_t));

        // get current pit frequency
        uint32_t GetPITFrequency();
    }
}