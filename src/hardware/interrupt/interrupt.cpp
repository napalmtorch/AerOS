#include "hardware/interrupt/interrupt.hpp"
#include "core/kernel.hpp"

namespace HAL
{
    namespace CPU
    {
        // total amount of ticks
        uint32_t Ticks;

        // amount of delta ticks
        uint32_t DeltaTicks;

        // total amount of seconds
        uint32_t Seconds;

        // timer second counter
        uint32_t TimerTick;

        // amount of seconds for timer to wait
        uint32_t TimerMax;

        // flag indicating if timer is running
        bool TimerRunning;

        uint32_t pit_freq = 0;

        // enable interrupts
        void EnableInterrupts() { __asm__ __volatile__("sti"); }

        // disable interrupts
        void DisableInterrupts() { __asm__ __volatile__("cli"); }

        // initialize interrupt service routines
        void InitializeISRs() { isr_init(); }

        // register interrupt handler
        void RegisterIRQ(uint8_t irq, isr_t handler) { isr_register(irq, handler); }

        // initialize pit
        void InitializePIT(uint32_t freq, void (*callback)(registers_t))
        {
            // set frequency value
            pit_freq = freq;
            
            // register interrupt
            RegisterIRQ(IRQ0, (isr_t)callback);

            // send frequency to pit
            uint32_t divisor = 1193180 / freq;
            uint8_t low = (uint8_t)(divisor & 0xFF);
            uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
            outb(0x43, 0x36);
            outb(0x40, low);
            outb(0x40, high);
        }

        // get current pit frequency
        uint32_t GetPITFrequency() { return pit_freq; }

        // wait a specified amount of seconds
        void Wait(uint32_t seconds)
        {
            TimerTick = 0;
            TimerMax = seconds;
            TimerRunning = true;
            uint8_t time = 0;

            wait:
            if (TimerRunning) { time = System::KernelIO::RTC.GetSecond(); goto wait; }
        }
    }
}