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
        HAL::FATFileSystem FAT;

        // real time clock
        HAL::RTCManager RTC;

        // ps2 keyboard controller driver
        HAL::PS2Keyboard Keyboard;

        // terminal interface
        HAL::TerminalManager Terminal;


        HAL::ACPI Acpi;

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

            // enable interrupts
            HAL::CPU::EnableInterrupts();

            // disable console output for debugger
            SetDebugConsoleOutput(false);

            // setup serial port connection
            SerialPort.SetPort(SERIAL_PORT_COM1);
            ThrowOK("Initialized serial port on COM1");
            //Initialise ACPI
            Acpi.ACPIInit();
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
            FAT.Initialize();
            ThrowOK("Initialized FAT file system");

            // initialize keyboard
            Keyboard.Initialize();
            Keyboard.BufferEnabled = true;
            Keyboard.Event_OnEnterPressed = enter_pressed;
            ThrowOK("Initialized PS/2 keyboard driver");

            // initialize pit
            HAL::CPU::InitializePIT(60, pit_callback);

            // string test
            String testStr = "Hello world";
            Terminal.WriteLine(testStr.ToCharArray());
            testStr = "POOP";
            Terminal.WriteLine(testStr.ToCharArray());

            // ready
            Terminal.Write("shell> ", COL4_YELLOW);

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
        void KernelBase::OnPanic()
        {
            
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

        }
        // triggered when enter key is pressed
        void KernelBase::OnEnterPressed(char* input)
        {
            HandleCommand(input);
            Terminal.Write("shell> ",COL4_YELLOW);
        }

        void KernelBase::HandleCommand(char* input)
        {
            // split input into arguments
            char* str = strsplit_index(input,0,' ');

            // blank input
            if (strlen(input) == 0) { return; }
            // list pci devices
            if (streql("lspci", input))
            {
                bool old = debug_console_enabled;
                SetDebugConsoleOutput(true);
                PCIBus.List(); 
                SetDebugConsoleOutput(old);
            }
            // clear the screen
            else if (streql("clear", input) || streql("cls", input)) { Terminal.Clear(COL4_BLACK); Terminal.SetCursorPos(0,0); }

            //cpuinfo
             else if (streql("shutdown", input)) { Acpi.Shutdown(); return; }
            else if (streql("cpuinfo", input)) { HAL::CPU::Detect(); return; }
            // set colors
            else if (streql ("color", str))
            {          
                char* background = strsplit_index(input,2,' ');
                char* foreground = strsplit_index(input,1,' ');

                if (background != nullptr && foreground != nullptr)
                {
                    if(streql(str, foreground) || streql(str, background))
                    { Terminal.WriteLine("All Parameters are required"); Terminal.WriteLine("Usage: color foreground background"); Terminal.WriteLine("Example: color white black"); return; }

                    System::KernelIO::WriteLine(foreground);
                    System::KernelIO::WriteLine(background);
                    Terminal.Clear(Graphics::GetColorFromName(background));
                    Terminal.SetForeColor(Graphics::GetColorFromName(foreground));
                    Terminal.WriteLine("Color Set!");

                    delete background;
                    delete foreground;
                    delete str;
                }
                return;
            }
            // debug
            else if (streql("debug", input))
            {
                char* s = new char[5];
                char* j = new char[5];
                delete s;
                delete j;
                char* q = new char[10];
            }
            // invalid command
            else { Terminal.WriteLine("Invalid command or file", COL4_RED); }

            // finish
            delete str;
            return;
        }
    }
}