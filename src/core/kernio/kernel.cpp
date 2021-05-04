#include "core/kernio/kernel.hpp"

uint32_t pos = 0;
static void pit_callback(registers_t regs)
{
    uint8_t* vidmem = (uint8_t*)0xB8000;
    vidmem[pos] = 'X';
    vidmem[pos + 1] = 0x1E;
    pos += 2;
    System::KernelIO::Kernel.OnInterrupt();
    UNUSED(regs);
}

namespace System
{
    namespace KernelIO
    {
        // kernel instance
        KernelIO::KernelBase Kernel = KernelIO::KernelBase();

        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            isr_init();
            __asm__ __volatile__("sti");
            isr_register(IRQ0, (isr_t)pit_callback);
        }

        // kernel core code, runs in a loop
        void KernelBase::Run()
        {

        }

        // triggered when a kernel panic is injected
        void KernelBase::OnPanic()
        {
            
        }

        // called when a handled interrupt call is finished
        void KernelBase::OnInterrupt()
        {

        }

        // called when interrupt 0x80 is triggered
        void KernelBase::OnSystemCall()
        {

        }
    }
}