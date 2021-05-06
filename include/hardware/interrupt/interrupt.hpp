#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/interrupt/idt.hpp"
#include "hardware/interrupt/isr.hpp"

namespace HAL
{
    namespace CPU
    {        
        // total amount of ticks
        extern uint32_t Ticks;

        // amount of delta ticks
        extern uint32_t DeltaTicks;

        // total amount of seconds
        extern uint32_t Seconds;

        // timer second counter
        extern uint32_t TimerTick;

        // amount of seconds for timer to wait
        extern uint32_t TimerMax;

        // flag indicating if timer is running
        extern bool TimerRunning;

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

        // wait a specified amount of seconds
        void Wait(uint32_t seconds);
    }
}