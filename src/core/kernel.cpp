#include "core/kernel.hpp"

uint32_t pos = 0;
static void pit_callback(registers_t regs)
{
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

        // multiboot header
        HAL::MultibootHeader Multiboot;

        // real time clock
        HAL::RTCManager RTC;

        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            // fetch multiboot header information from memory
            Multiboot.Read();

            // initialize fonts
            Graphics::InitializeFonts();

            // initialize interrupt service routines
            isr_init();

            // enable interrupts
            HAL::CPU::EnableInterrupts();

            // setup serial port connection
            SerialPort.SetPort(SERIAL_PORT_COM1);

            // initialize real time clock
            RTC.Initialize();

            // test exception
            ThrowError("You made a mistake you idiot");
            ThrowWarning("You are not very smart");
            ThrowOK("Its not uncommon these days xD");
            ThrowSystem("Commiting suicide...");
            
            // print multiboot name
            SerialPort.WriteLine(Multiboot.GetName(), 0xE);

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);
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