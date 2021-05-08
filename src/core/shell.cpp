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
        CommandList[7] = ShellCommand("FAT",        "Show FAT information", "",             Commands::FAT);
        CommandList[8] = ShellCommand("FATMBR",     "Show FAT master boot record", "",      Commands::FAT_MBR);
        CommandList[9] = ShellCommand("FATEXT",     "Show FAT extended boot record", "",    Commands::FAT_EXT);
        CommandList[10] = ShellCommand("DISKDUMP",  "Dump disk sectors into memory", "",  Commands::FAT_ROOT);

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
        if (streql(cmd, "CLS")) { Commands::CLEAR(input); return; }

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
            UNUSED(input);
        }

        void LSPCI(char* input)
        {
            bool old = debug_console_enabled;
            KernelIO::SetDebugConsoleOutput(true);
            KernelIO::PCIBus.List(); 
            KernelIO::SetDebugConsoleOutput(old);
        }

        void CPUINFO(char* input) { HAL::CPU::Detect(); }

        void FG(char* input)
        {
            char* color = strsplit_index(input, 1,' ');

            // invalid color
            if (strlen(color) == 0) 
            { 
                KernelIO::WriteLine("No color selected"); 
                KernelIO::Write("Usage: ", COL4_CYAN); 
                KernelIO::WriteLine("fg [color]"); 
                delete color;
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
            if (strlen(color) == 0) 
            { 
                KernelIO::WriteLine("No color selected"); 
                KernelIO::Write("Usage: ", COL4_CYAN); 
                KernelIO::WriteLine("bg [color]"); 
                delete color;
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

        void FAT(char* input)
        {
            KernelIO::FAT.PrintFATInformation();   
        }

        void FAT_MBR(char* input)
        {
            KernelIO::FAT.PrintBIOSParameterBlock();
        }

        void FAT_EXT(char* input)
        {
            KernelIO::FAT.PrintExtendedBootRecord();
        }

        void FAT_ROOT(char* input)
        {
            KernelIO::FAT.LoadRootDirectory();

            // get data from input
            char* str_offset = strsplit_index(input, 1, ' ');
            char* str_len    = strsplit_index(input, 2, ' ');

            uint32_t str_offset_dec = stod(str_offset);
            uint32_t offset = (uint32_t)KernelIO::FAT.FATTable + (str_offset_dec * 512);
            char offset_new[16];
            strdec(offset, offset_new);

            char cmd[64];
            strcat(cmd, "dump ");
            strcat(cmd, offset_new);
            strcat(cmd, " ");
            strcat(cmd, str_len);

            KernelIO::Shell.ParseCommand(cmd);

            delete str_offset;
            delete str_len;            

        }
    }
}