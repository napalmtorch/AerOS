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
        KernelIO::KernelBase Kernel;

        // serial port
        HAL::SerialPort SerialPort;

        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            // initialize interrupt service routines
            isr_init();

            // enable interrupts
            HAL::CPU::EnableInterrupts();

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);

            // test serial port
            SerialPort.SetPort(SERIAL_PORT_COM1);
            SerialPort.WriteLine("Hello penis, this is the world", 0xE);
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