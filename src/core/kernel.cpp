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

        // pci controller driver
        HAL::PCIBusController PCIBus;

        // memory manager
        HAL::MemoryManager MemoryManager;

        // vga graphics driver
        HAL::VGADriver VGA;

        // ata controller driver
        HAL::ATAController ATA;

        // real time clock
        HAL::RTCManager RTC;

        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            // initialize terminal interface
            term_init();
            term_writeln_ext("Starting AerOS...", COL4_GRAY);

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
            ThrowOK("Initialized serial port on COM1");

            // initialize memory manager
            MemoryManager.Initialize();
            ThrowOK("Initialize memory management system");

            // initialize pci bus
            PCIBus.Initialize();

            // setup vga graphics driver
            VGA.Initialize();
            ThrowOK("Initialized VGA driver");

            // initialize real time clock
            RTC.Initialize();
            ThrowOK("Initialized real time clock");

            // initialize ata controller driver
            ATA.Initialize();
            ThrowOK("Initialized ATA controller driver");
            
            // print multiboot name
            SerialPort.Write("BOOT LOADER: ", COL4_YELLOW);
            SerialPort.WriteLine(Multiboot.GetName(), COL4_WHITE);

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);

            // ready
            term_writeln_ext("Ready.", COL4_GREEN);
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