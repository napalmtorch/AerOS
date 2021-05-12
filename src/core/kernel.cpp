#include "core/kernel.hpp"
#include <gui/widget.hpp>
extern "C" {
extern uint32_t start;
extern uint32_t kernel_end;
}
static void pit_callback(registers_t regs)
{
    System::KernelIO::Kernel.OnInterrupt();
    UNUSED(regs);
}

void enter_pressed(char* input)
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

        // VMM
        HAL::VMM VMM;

        // vesa driver
        HAL::VESA VESA;

        // window server
        System::GUI::XServerHost XServer;
        // called as first function before kernel run
        void KernelBase::Initialize()
        {
            // initialize memory manager - we need memory first to parse start parameters effectively
            MemoryManager.Initialize();
            // read multiboot
            Multiboot.Read();

            // initialize terminal interface
            Terminal.Initialize();

            // prepare terminal
            Terminal.Clear(COL4_BLACK);
            Terminal.DisableCursor();
            Terminal.EnableCursor();

            // initialize fonts
            Graphics::InitializeFonts();

            // parse start parameters
            ParseStartParameters();

            // check if debugging flag is true
            if (Parameters.Debug)
            {
                // test break
                debug_bochs_break();

                // use console output by default, unless serial is specified as well
                SetDebugConsoleOutput(true);
            }
            // disable console debugging
            else { SetDebugConsoleOutput(false); }

            // check if serial flag is true
            if (Parameters.Serial)
            {
                // check if console debugging is also enabled
                if (Parameters.Debug) { SetDebugConsoleOutput(true); }
                else { SetDebugConsoleOutput(false); }          

                // enable serial debugging
                SetDebugSerialOutput(true);

                // setup serial port connection, also only in --debug mode
                SerialPort.SetPort(SERIAL_PORT_COM1);
            }

            // boot message
            Terminal.WriteLine("Starting AerOS...", COL4_GRAY);

            // vga mode
            if (Parameters.VGA)
            {
                VGA.SetMode(VGA.GetAvailableMode(4));
                ThrowOK("Initialized VGA driver");
                ThrowOK("Set VGA mode to 320x200 double buffered");
                SetDebugConsoleOutput(false);
            }
            // vesa mode
            else if (Parameters.VESA)
            {
                KernelIO::VESA.SetMode(640, 480, 32);
                HAL::CPU::DisableInterrupts();
                SetDebugConsoleOutput(false);
                ThrowOK("Initialized VESA driver");
                ThrowOK("Set VESA mode to 640x480 double buffered");
            }
            // text mode
            else
            {
                VGA.Initialize();
                VGA.SetMode(VGA.GetAvailableMode(0));
                ThrowOK("Initialized VGA driver");
                ThrowOK("Set VGA mode to 80x25");
            }

            // initialize interrupt service routines
            HAL::CPU::InitializeISRs();
            
            // initialize ACPI
            ACPI.ACPIInit();
            ThrowOK("ACPI Initialised");
            
            VMM.Initialize();

            // initialize pci bus
            PCIBus.Initialize();

            // initialize real time clock
            RTC.Initialize();
            ThrowOK("Initialized real time clock");

            if (!Parameters.DisableFS)
            {
                // initialize ata controller driver
                ATA.Initialize();
                ThrowOK("Initialized ATA controller driver");
                
                master_fs = makeFilesystem("");
                if(master_fs != NULL) 
                {
                    struct directory dir;
                    populate_root_dir(master_fs, &dir);
                    System::KernelIO::Terminal.WriteLine("Initialized FAT32 Filesystem");
                    term_writeln_dec("Kernel Start:", (uint32_t)&start);
                    term_writeln_dec("Kernel End:", (uint32_t)&kernel_end);
                    term_writeln_dec("Kernel Size:", (&kernel_end - &start));
                }
                else
                {
                    ThrowError("Error initializing Filesystem. Disk not FAT32?");
                }
            }

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


            // initialize x server
            XServer.Initialize();

            // ready shell
            Shell.Initialize();
            if (Parameters.VGA) { XServer.Start(VIDEO_DRIVER_VGA); }
            else if (Parameters.VESA) { XServer.Start(VIDEO_DRIVER_VESA); }
        }


        // parse start parameters
        void KernelBase::ParseStartParameters()
        {
            // get command line arguments
            char* input = System::KernelIO::Multiboot.GetCommandLine();

            // temporary values
            uint32_t delimiter_count = 0;
            size_t i = 0;

            // default parameters
            Parameters.Debug = false;
            Parameters.Serial = false;
            Parameters.DisableFS = false;
            Parameters.VGA = false;
            Parameters.VESA = false;

            // count delmiters
            for (i = 0; i < strlen(input); i++)
            { if (input[i] == 0x20) { delimiter_count++; } }

            // loop through delimiters
            for (i = 0; i < delimiter_count + 1; i++)
            {
                // grab string split from delimiter
                char* val = strsplit_index(input, i, ' ');
                
                // debug flag
                if (streql(val, "--debug")) { Parameters.Debug = true; }
                // serial flag
                if (streql(val, "--serial")) { Parameters.Serial = true; }
                // vga flag
                if (streql(val, "--vga")) { Parameters.VGA = true; }
                // vesa/vbe flag
                if (streql(val, "--vesa")) { Parameters.VESA = true; }
                // disable filesystem flag
                if (streql(val, "--no_fs")) { Parameters.DisableFS = true; }
            }
        }

        // kernel core code, runs in a loop
        void KernelBase::Run()
        {
            if (XServer.IsRunning())
            {
                XServer.Update();
                XServer.Draw();
            }
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
        uint32_t delta = 0;
        void KernelBase::OnInterrupt()
        {
            // increment ticks
            HAL::CPU::Ticks++;
        
            delta++;
            if (delta >= 60)
            {
                RTC.Read();
                delta = 0;
            }
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