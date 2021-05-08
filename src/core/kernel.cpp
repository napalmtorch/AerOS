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
       // HAL::FATFileSystem FAT;

        // real time clock
        HAL::RTCManager RTC;

        // ps2 keyboard controller driver
        HAL::PS2Keyboard Keyboard;

        // terminal interface
        HAL::TerminalManager Terminal;

        // shell
        System::ShellHost Shell;

        // acpi controller
        HAL::ACPI ACPI;

        VFS::FAT16 FAT16;

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
            VGA.SetMode(VGA.GetAvailableMode(0));

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

            // disable console output for debugger
            SetDebugConsoleOutput(false);

            // setup serial port connection
            SerialPort.SetPort(SERIAL_PORT_COM1);
            ThrowOK("Initialized serial port on COM1");
            //Initialise ACPI
            ACPI.ACPIInit();
            ThrowOK("ACPI Initialised");

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
        //    FAT.Initialize();
       //     FAT16.TestFat();
            FAT16.Initialise();
            ThrowOK("Initialized FAT file system");

            // initialize keyboard
            Keyboard.Initialize();
            Keyboard.BufferEnabled = true;
            Keyboard.Event_OnEnterPressed = enter_pressed;
            ThrowOK("Initialized PS/2 keyboard driver");

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);

            // enable interrupts
            HAL::CPU::EnableInterrupts();

            // ready shell
            Shell.Initialize();    

            // test fat driver
            /*          
            HAL::FATFile file = FAT.OpenFile("test.txt");
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
        void KernelBase::OnPanic(char* msg)
        {
            
            char* panic_string = "====PANIC====";
            char* expl ="A kernel panic was triggered";
            char* err = "Error Message: ";
            size_t center_loc = (80 - strlen(panic_string)) / 2;
            Terminal.Clear(COL4_DARK_BLUE);
            Terminal.SetCursorPos(center_loc, 0);
            System::KernelIO::WriteDecimal("String Length ",center_loc);
            Terminal.SetBackColor(COL4::COL4_DARK_BLUE);
            Terminal.SetForeColor(COL4::COL4_WHITE);
            Terminal.WriteLine(panic_string);
            Terminal.SetCursorPos((80 - strlen(expl)) / 2, 1);
            Terminal.WriteLine(expl);
            Terminal.NewLine();
            Terminal.NewLine();
            Terminal.SetCursorPos((80 - strlen(err)) / 2, 2);
            Terminal.Write(err);
            Terminal.NewLine();
            Terminal.SetForeColor(COL4_RED);
            Terminal.SetCursorPos((80 - strlen(msg)) / 2, 3);
            Terminal.Write(msg);
            Terminal.WriteLine(".");
            Terminal.SetForeColor(COL4_WHITE);
            Terminal.NewLine();
            Terminal.NewLine();
            Terminal.SetCursorPos((80 - strlen(panic_string)) / 2, 4);
            Terminal.WriteLine(panic_string);
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