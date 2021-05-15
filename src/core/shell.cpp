#include <core/shell.hpp>
#include <core/kernel.hpp>
#include <apps/win_term.hpp>

namespace System
{
    ShellCommand::ShellCommand() : Name("\0"), Help("\0"), Usage("\0"), Execute(nullptr) { }

    ShellCommand::ShellCommand(char* name, char help[], char usage[], void (*execute)(char*))
    {
        // check if name is too long
        if (strlen(name) > 16) { KernelIO::Write("Command name too long: ", COL4_RED); KernelIO::WriteLine(name); return; }
        // check if help is too long
        if (strlen(help) > 64) { KernelIO::Write("Command help too long: ", COL4_RED); KernelIO::WriteLine(name); return; }
        // check if usage is too long
        if (strlen(usage) > 32) { KernelIO::Write("Command usage too long: ", COL4_RED); KernelIO::WriteLine(name); return; }

        strcpy(name, Name);
        strcpy(help, Help);
        strcpy(usage, Usage);
        Execute = execute;
    }

    ShellCommand::~ShellCommand() { }

    ShellHost::ShellHost() { }

    ShellHost::~ShellHost() { }

    // initialize shell
    void ShellHost::Initialize()
    {
        RegisterCommand("CLEAR",      "Clear the screen", "",                 Commands::CLEAR);
        RegisterCommand("LSPCI",      "List detected PCI devices", "",        Commands::LSPCI);
        RegisterCommand("CPUINFO",    "List processor information", "",       Commands::CPUINFO);
        RegisterCommand("FG",         "Change foreground color", "",          Commands::FG);
        RegisterCommand("BG",         "Change background color", "",          Commands::BG);
        RegisterCommand("DUMP",       "Dump memory at location", "",          Commands::DUMP);
        RegisterCommand("HELP",       "Show available commands", "",          Commands::HELP);
        RegisterCommand("DISKDUMP",   "Dump disk sectors into memory", "",    Commands::DISK_DUMP);
        RegisterCommand("SHUTDOWN",   "Perform a ACPI shutdown", "",          Commands::SHUTDOWN);
        RegisterCommand("POWEROFF",   "Perform a legacy shutdown", "",        Commands::LEGACY_SHUTDOWN);
        RegisterCommand("REBOOT",     "Reboot the system", "",                Commands::REBOOT);
        RegisterCommand("TEST",       "Call a test systemcall", "",           Commands::TEST);
        RegisterCommand("PANIC",      "Throw a fake kernel panic", "",        Commands::PANIC);
        RegisterCommand("GFX",        "Test graphics mode", "",               Commands::GFX);
        RegisterCommand("LIST_TEST",  "Test vector list", "",                 Commands::LIST_TEST);
        RegisterCommand("LS",         "List directory contents", "",          Commands::LS);
        RegisterCommand("CAT",        "Display file contents", "",            Commands::CAT);
        RegisterCommand("MKDIR",      "Create a new directory", "",           Commands::MKDIR);

        // print caret to screen
        PrintCaret();
    }

    void ShellHost::PrintCaret()
    {
        KernelIO::Terminal.Write("shell> ", COL4_YELLOW);
    }

    void ShellHost::RegisterCommand(char* name, char* help, char* usage, void(*execute)(char*))
    {
        if (CommandIndex >= SHELL_MAX_COMMANDS) { KernelIO::ThrowError("Maximum amount of shell commands reached"); return; }
        strcpy(name, CommandList[CommandIndex].Name);
        strcpy(help, CommandList[CommandIndex].Help);
        strcpy(usage, CommandList[CommandIndex].Usage);
        CommandList[CommandIndex].Execute = execute;
        CommandIndex++;
    }

    void ShellHost::HandleInput(char* input)
    {
        ParseCommand(input);
        
        char* cmd = strsplit_index(input, 0 ,' ');
        strupper(cmd);
        if (!streql(cmd, "GFX")) { PrintCaret(); }
        delete cmd;
    }

    void ShellHost::ParseCommand(char* input)
    {
        // get command from input
        char* cmd = strsplit_index(input, 0 ,' ');
        strupper(cmd);

        // hack to use second command for clearing
        if (streql(cmd, "CLS")) { Commands::CLEAR(input); delete cmd; return; }

        // loop through commands
        for (size_t i = 0; i < SHELL_MAX_COMMANDS; i++)
        {
            // found match
            if (streql(cmd, CommandList[i].Name) == true)
            {
                // execute command and return
                CommandList[i].Execute(input);
                delete cmd;
                return;
            }
        }

        if (cmd != nullptr) { delete cmd; }

        // don't print if no text was typed
        if (strlen(input) > 0) { KernelIO::Terminal.WriteLine("Invalid command or file", COL4_RED); }
    }

    ShellCommand ShellHost::GetCommand(uint32_t index) { return CommandList[index]; }

    char* ShellHost::GetCurrentPath() { return CurrentPath.ToCharArray(); }


    namespace Commands
    {
        void CLEAR(char* input)
        {
            KernelIO::Terminal.Clear();
        }

        void LSPCI(char* input)
        {
            KernelIO::PCIBus.List();
        }

        void CPUINFO(char* input) { HAL::CPU::Detect(); }

        void FG(char* input)
        {
            char* color = strsplit_index(input, 1,' ');

            // invalid color
            if (color == nullptr) 
            { 
                KernelIO::Terminal.WriteLine("No color selected"); 
                KernelIO::Terminal.Write("Usage: ", COL4_CYAN); 
                KernelIO::Terminal.WriteLine("fg [color]"); 
                //delete color;
                return;
            }

            // set color
            KernelIO::Terminal.SetForeColor(Graphics::GetColorFromName(color));

            delete color;
        }

        void BG(char* input)
        {
            char* color = strsplit_index(input, 1, ' ');

            // invalid color
            if (color == nullptr) 
            { 
                KernelIO::Terminal.WriteLine("No color selected"); 
                KernelIO::Terminal.Write("Usage: ", COL4_CYAN); 
                KernelIO::Terminal.WriteLine("bg [color]"); 
                //delete color;
                return;
            }

            // set color
            KernelIO::Terminal.SetBackColor(Graphics::GetColorFromName(color));

            delete color;
        }

        void DUMP(char* input)
        {
            // get data from input
            char* str_offset = strsplit_index(input, 1, ' ');
            char* str_len    = strsplit_index(input, 2, ' ');

            uint32_t offset = stod(str_offset);
            uint32_t length = stod(str_len);

            KernelIO::Terminal.WriteLine("Showing memory at ");
            char temp[16];
            strhex32(offset, temp);
            KernelIO::Terminal.WriteLine(temp, COL4_GRAY);

            bool oldConsole = debug_console_enabled;
            bool oldSerial = debug_serial_enabled;
            KernelIO::SetDebugConsoleOutput(true);
            KernelIO::SetDebugSerialOutput(false);
            KernelIO::DumpMemoryTerminal((uint8_t*)offset, length, 12, true);
            KernelIO::SetDebugConsoleOutput(oldConsole);
            KernelIO::SetDebugSerialOutput(oldSerial);

            delete str_offset;
            delete str_len;
        }

        void HELP(char* input)
        {
            // loop through commands
            for (size_t i = 0; i < SHELL_MAX_COMMANDS; i++)
            {
                if (strlen(KernelIO::Shell.GetCommand(i).Name) > 0)
                {
                    KernelIO::Terminal.Write("- ");
                    KernelIO::Terminal.Write(KernelIO::Shell.GetCommand(i).Name);
                    if (!KernelIO::XServer.IsRunning()) { KernelIO::Terminal.SetCursorX(16); }
                    else { ((Applications::WinTerminal*)KernelIO::Terminal.Window)->SetCursorX(16); }
                    KernelIO::Terminal.WriteLine(KernelIO::Shell.GetCommand(i).Help, COL4_GRAY);
                }
            }
        }

        void DISK_DUMP(char* input)
        {
            char* str_sector = strsplit_index(input, 1, ' ');
            char* str_bytes  = strsplit_index(input, 2, ' ');
            uint8_t* buffer = new uint8_t[512];

            uint32_t sector = stod(str_sector);
            uint32_t bytes =  stod(str_bytes);
            if (bytes > 512) { bytes = 512; }

            uint32_t offset = (uint32_t)((sector * 512) + buffer);

            KernelIO::ATA.ReadSectors(buffer, sector, 1);

            KernelIO::DumpMemory((uint8_t*)offset, bytes, 12, true);
        }

        void SHUTDOWN(char* input)
        {
            System::KernelIO::ACPI.Shutdown();
        }

        void LEGACY_SHUTDOWN(char* input)
        {
            System::KernelIO::ACPI.LegacyShutdown();
        }

        void REBOOT(char* input)
        {
            System::KernelIO::ACPI.Reboot();
        }
        
        void TEST(char* input)
        {
            // call syscall
            asm volatile("int $0x80");
        }

        void MKDIR(char* input)
        {
            struct directory dir;
            fat_populate_root_dir(fat_master_fs, &dir);
            char* dirname = strsplit_index(input, 1, ' ');
            if(dirname == nullptr) { System::KernelIO::Terminal.WriteLine("No filename specified"); }
            else { fat_create_dir(fat_master_fs,&dir,dirname); } 
        }

        void LS(char* input)
        {
            struct directory dir;
            fat_populate_root_dir(fat_master_fs, &dir);
            char* listdir = strsplit_index(input, 1, ' ');
            if(listdir == nullptr)
            {
                //show root
                KernelIO::Terminal.WriteLine("Showing Root");
                fat_print_directory(fat_master_fs,&dir);
            }
            else
            {
                KernelIO::Terminal.Write("Showing: ");
                KernelIO::Terminal.WriteLine(listdir);
                fat_dir_by_name(fat_master_fs,&dir,listdir);
            }
        }

        void CAT(char* input)
        {
            struct directory dir;
            fat_populate_root_dir(fat_master_fs, &dir);
            char* file = strsplit_index(input, 1, ' ');
            fat_cat_file(fat_master_fs,&dir,file);
        }
        
        void PANIC(char* input)
        {
            char* err = strsub(input, 6, strlen(input));
            if (err == nullptr) { debug_throw_panic("Nothing actually crashed, I'm just a cunt."); return; }
            debug_throw_panic(err);
        }

        void GFX(char* input)
        {
            if (KernelIO::Terminal.Window != nullptr || KernelIO::XServer.Running) { KernelIO::Terminal.WriteLine("Graphics mode already running", COL4_RED); return; }

            KernelIO::VESA.SetMode(640, 480, 32);
            KernelIO::SetDebugConsoleOutput(false);
            KernelIO::ThrowOK("Initialized VESA driver");
            KernelIO::ThrowOK("Set VESA mode to 640x480 double buffered");

            
            // initialize keyboard
            KernelIO::Keyboard.BufferEnabled = false;
            KernelIO::Keyboard.Event_OnEnterPressed = nullptr;

            // initialize mouse
            outb(0x64, 0xA8);
            outb(0x64, 0x20);
            uint8_t status = inb(0x60) | 2;
            outb(0x64, 0x60);
            outb(0x60, status);
            outb(0x64, 0xD4);
            outb(0x60, 0xF4);
            inb(0x60);
            
            // enable interrupts
            asm volatile("cli");
            HAL::CPU::EnableInterrupts();

            KernelIO::XServer.Initialize();
            KernelIO::XServer.Start();
            
        }

        void LIST_TEST(char*input)
        {
            /*
            System::List<char*> test = System::List<char*>();
            test.Add("Penis");
            test.Add("World");
            test.Insert(1, "Hello");
            KernelIO::Terminal.WriteLine((*test.Get(0)));
            KernelIO::Terminal.WriteLine((*test.Get(1)));
            KernelIO::Terminal.WriteLine((*test.Get(2)));
            test.~List();
            */
        }
    }
}