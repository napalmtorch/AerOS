#include "core/kernel.hpp"

uint32_t pos = 0;
static void pit_callback(registers_t regs)
{
    System::KernelIO::Kernel.OnInterrupt();
    UNUSED(regs);
}

static void enter_pressed(char* input)
{
    System::KernelIO::Kernel.OnEnterPressed(input);
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

        // terminal interface
        HAL::TerminalManager Terminal;

        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            debug_bochs_break();
            // initialize terminal interface
            Terminal.Initialize();

            // initialize fonts
            Graphics::InitializeFonts();

            // setup vga graphics driver
            VGA.Initialize();

            // set mode to 90x60
            VGA.SetMode(VGA.GetAvailableMode(2));

            // prepare terminal
            Terminal.Clear(COL4_BLACK);
            Terminal.DisableCursor();
            Terminal.EnableCursor();

            // boot message
            Terminal.WriteLine("Starting AerOS...", COL4_GRAY);

            // fetch multiboot header information from memory
            Multiboot.Read();
            
            // initialize interrupt service routines
            HAL::CPU::InitializeISRs();

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
            // ATA.Initialize();
            ThrowOK("Initialized ATA controller driver");

            // initialize fat file system
            //FAT.Initialize();
            ThrowOK("Initialized FAT file system");

            // initialize keyboard
            Keyboard.Initialize();
            Keyboard.BufferEnabled = true;
            Keyboard.Event_OnEnterPressed = enter_pressed;
            ThrowOK("Initialized PS/2 keyboard driver");

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);

            // ready
            Terminal.Write("shell> ", COL4_YELLOW);

            // test fat driver
            /*
            HAL::FATFile file = FAT.OpenFile("testdir");
            Write("FILE_NAME: "); WriteLine(file.Name);
            WriteLineDecimal("LENGTH: ", file.FileLength);
            WriteLineDecimal("FLAGS: ", file.Flags);
            WriteLineDecimal("DRIVE: ", file.Drive);
            WriteLineDecimal("CLUSTER: ", file.CurrentCluster);
            WriteLineDecimal("ID: ", file.ID);
            WriteLineDecimal("POSITION: ", file.Position);
            WriteLineDecimal("EOF: ", file.EOF);
            */
        }

        // kernel core code, runs in a loop
        void KernelBase::Run()
        {
            
        }

        // triggered when a kernel panic is injected
        void KernelBase::OnPanic()
        {
            
        }

        // triggered when a handled interrupt call is finished
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

        // triggered when interrupt 0x80 is triggered
        void KernelBase::OnSystemCall()
        {

        }

        // triggered when enter key is pressed
        void KernelBase::OnEnterPressed(char* input)
        {
            Terminal.Write("You typed: ");
            Terminal.WriteLine(input);
            Terminal.Write("shell> ", COL4_YELLOW);
        }
    }
}