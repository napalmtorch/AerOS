#include "core/kernel.hpp"
#include <graphics/canvas.hpp>
#include <graphics/colors.hpp>
#include <graphics/canvas.hpp>
#include <graphics/bitmap.hpp>

extern "C"
{
    uint32_t kernel_end_real = (uint32_t)&kernel_end;
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

        //SMBIOS
        BIOS::SMBIOS smbios;
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

        // napalm file system
        HAL::NapalmFileSystem NapalmFS;

        Threading::ThreadManager TaskManager;

        XServerHost XServer;
        GUI::WindowManager WindowMgr;
        
        void InTest()
        {
            while (true);
        }
        
        // called as first function before kernel run
        void Test()
        {
            InTest();  
        }

        void KernelBase::InitThreaded()
        {
            // this is the kernel's thread pool.
            // load here every useful thread for kernel initialization
            Shell.Initialize();

            if(Parameters.VESA)
            {
                XServer.Start();
            }

            while (true) { Run(); }
        }

        void KernelBase::Initialize()
        {
            // initialize memory manager - we need memory first to parse start parameters effectively
            MemoryManager.Initialize(true);

            // read multiboot
            Multiboot.Read();

            // initialize fonts
            Graphics::InitializeFonts();

            KernelIO::VESA.Initialize();
            KernelIO::VESA.SetMode(640, 480, 32);

            term_init();

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

            // initialize interrupt service routines
            HAL::CPU::InitializeISRs();
            
            // initialize ACPI
            ACPI.ACPIInit();
            ThrowOK("ACPI Initialised");
            
            VMM.Initialize();

            // initialize pci bus
            PciInit();
            PCIBus.Initialize();

            // initialize SMBIOS
            smbios.Initialize();

            // initialize real time clock
            RTC.Initialize();
            ThrowOK("Initialized real time clock");
            char test[32];
            strdec((uint32_t)smbios.CheckMachine(),test);
            bool FS_Disable=false;

            if(!Parameters.SMBIOS)
            {
                Terminal.Write("Detected Machine: ");
                Terminal.WriteLine(test);
                smbios.DetectMachine();
                if(smbios.CheckMachine() == smbios.Bochs)
                {
                    Terminal.WriteLine("Welcome Bochs!");
                    FS_Disable = true;
                }
                else if(smbios.CheckMachine() == smbios.Qemu)
                {
                    Terminal.WriteLine("Welcome QEMU!");
                    FS_Disable = false;
                }
                else if(smbios.CheckMachine() == smbios.VMWare)
                {
                    Terminal.WriteLine("Welcome VMWare!");
                    FS_Disable = true;
                }
                else if(smbios.CheckMachine() == smbios.Unknown)
                {
                    Terminal.WriteLine("Welcome Unknown Machine!");
                    FS_Disable = false;
                }
            }
            else if (!Parameters.DisableFS) { FS_Disable = false; } else { FS_Disable = true; }

            if (!FS_Disable)
            {
                // initialize ata controller driver
                ATA.Initialize();
                ThrowOK("Initialized ATA controller driver");
                
                // read boot record
                uint8_t* boot_sec = (uint8_t*)mem_alloc(512);
                ata_pio_read48(0, 1, boot_sec);
                
                // check for fat signatures
                if (boot_sec[38] == 0x28 || boot_sec[38] == 0x29 || boot_sec[66] == 0x28 || boot_sec[66] == 0x29)
                {
                    fat_master_fs = fs_create("");
                    if(fat_master_fs != NULL) 
                    {
                        struct directory dir;
                        fat_populate_root_dir(fat_master_fs, &dir);
                        System::KernelIO::Terminal.WriteLine("Initialized FAT32 Filesystem");
                        //term_writeln_dec("Kernel Start:", (uint32_t)&start);
                        term_writeln_dec("Kernel End:", (uint32_t)&kernel_end);
                        //term_writeln_dec("Kernel Size:", (&kernel_end - &start));
                    }
                    else
                    {
                        ThrowError("Error initializing Filesystem. Disk not FAT32?");
                    }
                }
                // check for nfs signature
                else if (boot_sec[19] == 0x66 && boot_sec[18] == 0x66 && boot_sec[17] == 0x94 && boot_sec[16] == 0x20)
                {
                    NapalmFS.Mount();
                    bool exists = NapalmFS.FileExists("/home/will/pictures/bliss.bmp");
                    WriteLineDecimal("BLISS.BMP exists?  ", (int32_t)exists);
                    ThrowOK("Initialized NAPALM file system");
                }
            }

            // initialize keyboard
            Keyboard.Initialize();
            Keyboard.BufferEnabled = true;
            Keyboard.Event_OnEnterPressed = enter_pressed;
            ThrowOK("Initialized PS/2 keyboard driver");
            
            Mouse.Initialize();
            Mouse.SetBounds(0, 0, VESA.GetWidth(), VESA.GetHeight());
            Mouse.SetPosition(VESA.GetWidth() / 2, VESA.GetHeight() / 2);
            ThrowOK("Initialized PS/2 mouse driver");

            // initialize pit
            HAL::CPU::InitializePIT(2000, nullptr);
            ThrowOK("Initialized PIT controller at 60 Hz");

            HAL::CPU::Detect();
            ThrowOK("Successfully retrieved processor information");

            // initialize task manager
            TaskManager = System::Threading::ThreadManager();

            // enable interrupts
            HAL::CPU::EnableInterrupts();
            ThrowOK("Enabled interrupts");
            
            // enable networking
            System::Network::e1000 *e1000 = new System::Network::e1000();
            ThrowOK("Network Initialized");
            e1000->PollData();
            ThrowOK("Started Network Polling Thread");

            //Test EndsWith
            if(EndsWith("hello.txt",".txt") !=0) { Terminal.WriteLine("String ended with .txt",COL4_CYAN); } 
            Threading::Thread* core_thread = new Threading::Thread("Core", "core", System::Threading::Priority::Protected, [] () { KernelIO::Kernel.InitThreaded(); });
            core_thread->Start();

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
            Parameters.SMBIOS = false;

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
                // disable SMBIOS flag
                if (streql(val, "--no_smbios")) { Parameters.SMBIOS = true; }
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
            else { term_draw(); }
            VESA.Render();
        }
        
        // triggered when a kernel panic is injected
        void KernelBase::OnPanic(char* msg)
        {
            if(fat_master_fs != NULL)
            {
                fs_destroy(fat_master_fs);
            }
            uint8_t yy = 0; 
            // messages 
            char panic_string[] = "====PANIC====";
            char expl[] ="AerOS encountered a serious problem!";
            char err[] = "Error Message: ";
            char halt[] = "AerOS has been halted to prevent damage to your Computer!";
            char disclaimer[] = "Since there is no documentation you are basically fucked now xD";
            char temp[100]{ '\0' }; //make sure we have enough space for error messages
            strcat(temp,err);
            strcat(temp,msg);       
            Terminal.Clear(COL4_DARK_BLUE);
            Terminal.SetForeColor(COL4_WHITE);
            Terminal.Write(panic_string, yy++, TEXT_ALIGN_CENTER, COL4_WHITE);
            yy += 2;
            Terminal.Write(expl, yy++, TEXT_ALIGN_CENTER);
            yy += 2;
            Terminal.Write(err, yy++, TEXT_ALIGN_CENTER);
            Terminal.Write(msg, yy++, TEXT_ALIGN_CENTER, COL4_RED);
            yy += 2;
            Terminal.Write(halt, yy++, TEXT_ALIGN_CENTER, COL4_RED);
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
            if (delta >= 1000)
            {
                RTC.Read();
                delta = 0;
            }

            if (!XServer.IsRunning()) { term_cursor_flash(); }
            else { XServer.OnInterrupt(); }
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