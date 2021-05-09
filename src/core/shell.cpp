#include <core/shell.hpp>
#include <core/kernel.hpp>

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
        // add commands to list
        CommandList[0] = ShellCommand("CLEAR",      "Clear the screen", "",                 Commands::CLEAR);
        CommandList[1] = ShellCommand("LSPCI",      "List detected PCI devices", "",        Commands::LSPCI);
        CommandList[2] = ShellCommand("CPUINFO",    "List processor information", "",       Commands::CPUINFO);
        CommandList[3] = ShellCommand("FG",         "Change foreground color", "",          Commands::FG);
        CommandList[4] = ShellCommand("BG",         "Change background color", "",          Commands::BG);
        CommandList[5] = ShellCommand("DUMP",       "Dump memory at location", "",          Commands::DUMP);
        CommandList[6] = ShellCommand("HELP",       "Show available commands", "",          Commands::HELP);
        CommandList[7] = ShellCommand("DISKDUMP",   "Dump disk sectors into memory", "",    Commands::DISK_DUMP);
        CommandList[8] = ShellCommand("SHUTDOWN",   "Perform a ACPI Shutdown", "",          Commands::SHUTDOWN);
        CommandList[9] = ShellCommand("POWEROFF",   "Perform a Legacy Shutdown", "",        Commands::LEGACY_SHUTDOWN);
        CommandList[10] = ShellCommand("REBOOT",    "Reboot the Computer", "",              Commands::REBOOT);
        CommandList[11] = ShellCommand("TEST",      "Call a test systemcall", "",           Commands::TEST);
        CommandList[12] = ShellCommand("PANIC",     "Throw a fake kernel panic", "",        Commands::PANIC);
        CommandList[13] = ShellCommand("FAT",       "FAT information", "",                  Commands::FAT_INFO);
        CommandList[14] = ShellCommand("FATMBR",    "FAT master boot record", "",           Commands::FAT_MBR);
        CommandList[15] = ShellCommand("FATEXT",    "FAT extended boot record", "",         Commands::FAT_EXT);
        CommandList[16] = ShellCommand("GFX",       "Test graphics mode", "",               Commands::GFX);

        // print caret to screen
        PrintCaret();
    }

    void ShellHost::PrintCaret()
    {
        KernelIO::Terminal.Write("shell> ", COL4_YELLOW);
    }

    void ShellHost::HandleInput(char* input)
    {
        ParseCommand(input);
        PrintCaret();
    }

    void ShellHost::ParseCommand(char* input)
    {
        // get command from input
        char* cmd = strsplit_index(input, 0 ,' ');
        strupper(cmd);

        // hack to use second command for clearing
        if (streql(cmd, "CLS")) { Commands::CLEAR(input); delete cmd; return; }

        // loop through commands
        for (size_t i = 0; i < 32; i++)
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
            KernelIO::DumpMemory((uint8_t*)offset, length, 12, true);
            KernelIO::SetDebugConsoleOutput(oldConsole);
            KernelIO::SetDebugSerialOutput(oldSerial);

            delete str_offset;
            delete str_len;
        }

        void HELP(char* input)
        {
            // loop through commands
            for (size_t i = 0; i < 32; i++)
            {
                if (strlen(KernelIO::Shell.GetCommand(i).Name) > 0)
                {
                    KernelIO::Terminal.Write("- ");
                    KernelIO::Terminal.Write(KernelIO::Shell.GetCommand(i).Name);
                    KernelIO::Terminal.SetCursorX(30);
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

        void PANIC(char* input)
        {
            char* err = strsub(input, 6, strlen(input));
            if (err == nullptr) { debug_throw_panic("Nothing actually crashed, I'm just a cunt."); return; }
            debug_throw_panic(err);
        }

        void FAT_MBR(char* input)
        {
            KernelIO::FAT16.PrintMBR();
        }

        void FAT_EXT(char* input)
        {
            KernelIO::FAT16.PrintEXT();
        }

        void FAT_INFO(char* input)
        {
            KernelIO::FAT16.PrintInfo();
        }

        void GFX(char* input)
        {
            KernelIO::VGA.SetMode(KernelIO::VGA.GetAvailableMode(4));
            Graphics::VGACanvas canvas;

            // loop
            while (true)
            {
                // clear the screen
                canvas.Clear(COL8_DARK_CYAN);

                canvas.DrawFilledRectangle(64, 64, 64, 64, COL8_DARK_BLUE);

                canvas.DrawFilledRectangle(KernelIO::Mouse.GetX(), KernelIO::Mouse.GetY(), 4, 4, COL8_WHITE);

                canvas.DrawChar(128, 128, '9', COL8_WHITE, Graphics::FONT_8x8);

                canvas.DrawString(0, 0, "AerOS", COL8_YELLOW, Graphics::FONT_8x8);

                // swap buffer
                canvas.Display();
            }
        }
    }
}