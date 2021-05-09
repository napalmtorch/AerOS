#include "core/kernel.hpp"

static void pit_callback(registers_t regs)
{
    System::KernelIO::Kernel.OnInterrupt();
    UNUSED(regs);
}

static void enter_pressed(char* input)
{
    System::KernelIO::Kernel.OnEnterPressed(input);
}

extern "C" int endKernel;

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
        VFS::FAT16 FAT16;

        // real time clock
        HAL::RTCManager RTC;

        // ps2 keyboard controller driver
        HAL::PS2Keyboard Keyboard;
        
        // ps2 mouse controller driver
        HAL::PS2Mouse Mouse;

        // terminal interface
        HAL::TerminalManager Terminal;

        // shell
        System::ShellHost Shell;

        // acpi controller
        HAL::ACPI ACPI;

        // called as first function before kernel run
        void KernelBase::Initialize()
        {

            // fetch multiboot header information from memory
            // we need this VERY early on since it contains the boot parameters,
            // NICO I AM LOOKING AT YOU, DONT FUCK WITH IT xD - Signed Kev         
            Multiboot.Read();
            if(StringContains(System::KernelIO::Multiboot.GetCommandLine(),"--debug"))
            {
                debug_bochs_break();
                // We only enable console output for the debugger when the kernel was booted with --debug
                SetDebugConsoleOutput(true);
                if(StringContains(System::KernelIO::Multiboot.GetCommandLine(),"--serial"))
                {
                    SetDebugSerialOutput(true);
                }
                else
                {
                    SetDebugSerialOutput(false);  
                }
                // setup serial port connection, also only in --debug mode
                SerialPort.SetPort(SERIAL_PORT_COM1);
                ThrowOK("Initialized serial port on COM1");
            }
            else
            {
                SetDebugConsoleOutput(false);   
            }

            // initialize terminal interface
            Terminal.Initialize();

            // prepare terminal
            Terminal.Clear(COL4_BLACK);
            Terminal.DisableCursor();
            Terminal.EnableCursor();

            // initialize fonts
            Graphics::InitializeFonts();

            // setup vga graphics driver
            VGA.Initialize();

            // boot message
            Terminal.WriteLine("Starting AerOS...", COL4_GRAY);

            VGA.SetMode(VGA.GetAvailableMode(0));
            ThrowOK("Initialized VGA driver");
            ThrowOK("Set VGA mode to 80x25");

            // initialize interrupt service routines
            HAL::CPU::InitializeISRs();
            
            // initialize ACPI
            ACPI.ACPIInit();
            ThrowOK("ACPI Initialised");

            // initialize memory manager
            MemoryManager.Initialize();
            ThrowOK("Initialized memory management system");

            // initialize pci bus
            PCIBus.Initialize();

            // initialize real time clock
            RTC.Initialize();
            ThrowOK("Initialized real time clock");

            // initialize ata controller driver
            ATA.Initialize();
            ThrowOK("Initialized ATA controller driver");

            // initialize fat file system
            //FAT16.Initialize();
            //ThrowOK("Initialized FAT file system");       

            // initialize keyboard
            Keyboard.Initialize();
            Keyboard.BufferEnabled = true;
            Keyboard.Event_OnEnterPressed = enter_pressed;
            ThrowOK("Initialized PS/2 keyboard driver");

            Mouse.Initialize();

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);

            // enable interrupts
            asm volatile("cli");
            HAL::CPU::EnableInterrupts();

            // ready shell
            Shell.Initialize();
            if(StringContains(System::KernelIO::Multiboot.GetCommandLine(),"--vga"))
            {
                Shell.ParseCommand("gfx");
            }
        }

        // kernel core code, runs in a loop
        void KernelBase::Run()
        {
            
        }
        
        // triggered when a kernel panic is injected
        void KernelBase::OnPanic(char* msg)
        {
            // messages 
            char panic_string[] = "====PANIC====";
            char expl[] ="A kernel panic was triggered";
            char err[] = "Error Message: ";
            
            uint8_t yy = 0; 
            Terminal.Clear(COL4_DARK_BLUE);
            Terminal.SetForeColor(COL4_WHITE);
            Terminal.Write(panic_string, yy++, TEXT_ALIGN_CENTER, COL4_WHITE);
            yy += 2;
            Terminal.Write(expl, yy++, TEXT_ALIGN_CENTER);
            yy += 2;
            Terminal.Write(err, yy++, TEXT_ALIGN_CENTER);
            Terminal.Write(msg, yy++, TEXT_ALIGN_CENTER, COL4_RED);
            yy += 2;
            Terminal.NewLine();
            Terminal.NewLine();
            Terminal.Write(panic_string, yy++, TEXT_ALIGN_CENTER);
            Terminal.DisableCursor();
        }

        // triggered when a handled interrupt call is finished
        void KernelBase::OnInterrupt()
        {
            // increment ticks
            HAL::CPU::Ticks++;
        }

        // triggered when interrupt 0x80 is triggered
        void KernelBase::OnSystemCall()
        {
            Terminal.WriteLine("This is a motherfucking syscall OK?!!");
        }

        // triggered when enter key is pressed
        void KernelBase::OnEnterPressed(char* input)
        {
            Shell.HandleInput(input);
        }
    }
}