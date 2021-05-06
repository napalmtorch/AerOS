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

        // file system
        HAL::FATFileSystem FAT;

        // real time clock
        HAL::RTCManager RTC;

        // ps2 keyboard controller driver
        HAL::PS2Keyboard Keyboard;

        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            // initialize terminal interface
            term_init();

            // initialize fonts
            Graphics::InitializeFonts();

            // setup vga graphics driver
            VGA.Initialize();

            // set mode to 90x60
            VGA.SetMode(VGA.GetAvailableMode(2));

            // prepare terminal
            term_clear(COL4_BLACK);
            term_cursor_disable();
            term_cursor_enable(0, 15);

            // boot message
            term_writeln_ext("Starting AerOS...", COL4_GRAY);

            // fetch multiboot header information from memory
            Multiboot.Read();
            
            // initialize interrupt service routines
            isr_init();

            // enable interrupts
            HAL::CPU::EnableInterrupts();

            // disable console output for debugger
            SetDebugConsoleOutput(false);

            // setup serial port connection
            SerialPort.SetPort(SERIAL_PORT_COM1);
            ThrowOK("Initialized serial port on COM1");

            // initialize memory manager
            MemoryManager.Initialize();
            ThrowOK("Initialize memory management system");

            // initialize pci bus
            PCIBus.Initialize();

            // initialize real time clock
            RTC.Initialize();
            ThrowOK("Initialized real time clock");

            // initialize ata controller driver
            ATA.Initialize();
            ThrowOK("Initialized ATA controller driver");

            // initialize fat file system
            FAT.Initialize();
            ThrowOK("Initialized FAT file system");

            // initialize keyboard
            Keyboard.Initialize();
            Keyboard.BufferEnabled = true;
            ThrowOK("Initialized PS/2 keyboard driver");

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
            // increment ticks
            HAL::CPU::Ticks++;

            // increment delta ticks
            HAL::CPU::DeltaTicks++;

            // second has passed
            if (HAL::CPU::DeltaTicks >= HAL::CPU::GetPITFrequency())
            {
                // increment seconds
                HAL::CPU::Seconds++;

                HAL::CPU::TimerTick++;

                // check if timer is expired
                if (HAL::CPU::TimerTick >= HAL::CPU::TimerMax) 
                {
                    HAL::CPU::TimerTick = 0; 
                    HAL::CPU::TimerMax = 0; 
                    HAL::CPU::TimerRunning = false; 
                }

                // reset delta ticks
                HAL::CPU::DeltaTicks = 0;
            }
        }

        // called when interrupt 0x80 is triggered
        void KernelBase::OnSystemCall()
        {

        }
    }
}