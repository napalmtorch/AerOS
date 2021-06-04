#include <core/shell.hpp>
#include <core/kernel.hpp>
#include <hardware/sudoers.hpp>
#include <hardware/fat.hpp>
#include <hardware/hostname.hpp>
#include <hardware/drivers/rtc.hpp>
#include <lib/string.hpp>

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
        RegisterCommand("clear",      "Clear the screen", "",                 Commands::CLEAR);
        RegisterCommand("lspci",      "List detected PCI devices", "",        Commands::LSPCI);
        RegisterCommand("cpuinfo",    "List processor information", "",       Commands::CPUINFO);
        RegisterCommand("fg",         "Change foreground color", "",          Commands::FG);
        RegisterCommand("bg",         "Change background color", "",          Commands::BG);
        RegisterCommand("dump",       "Dump memory at location", "",          Commands::DUMP);
        RegisterCommand("help",       "Show available commands", "",          Commands::HELP);
        RegisterCommand("diskdump",   "Dump disk sectors into memory", "",    Commands::DISK_DUMP);
        RegisterCommand("shutdown",   "Perform a ACPI shutdown", "",          Commands::SHUTDOWN);
        RegisterCommand("poweroff",   "Perform a legacy shutdown", "",        Commands::LEGACY_SHUTDOWN);
        RegisterCommand("reboot",     "Reboot the system", "",                Commands::REBOOT);
        RegisterCommand("syscall",    "Call a test systemcall", "",           Commands::TEST);
        RegisterCommand("panic",      "Throw a fake kernel panic", "",        Commands::PANIC);
        RegisterCommand("gfx",        "Test graphics mode", "",               Commands::GFX);
        RegisterCommand("testlist",   "Test vector list", "",                 Commands::LIST_TEST);
        RegisterCommand("cd",         "Set current directory", "",            Commands::CD);
        RegisterCommand("ls",         "List directory contents", "",          Commands::LS);
        RegisterCommand("cat",        "Display file contents", "",            Commands::CAT);
        RegisterCommand("mkdir",      "Create a new directory", "",           Commands::MKDIR);
        RegisterCommand("textview",   "Open file in Text Viewer", "",         Commands::TEXTVIEW);
        RegisterCommand("run",        "Run a windowed application", "",       Commands::RUN);
        RegisterCommand("time",       "Display time", "",                     Commands::TIME);
        RegisterCommand("rat",        "Show ram allocation table", "",        Commands::RAT);    
        RegisterCommand("format",      "Format a disk to the NapalmFS format","",Commands::FORMAT);
        RegisterCommand("ps",          "List PIDS","",                          Commands::PS);

        CurrentPath[0] = '\0';
        if (fat_master_fs != nullptr) { strcat(CurrentPath, "/users/aeros"); }

        // print caret to screen
        PrintCaret();
    }

    void ShellHost::PrintCaret()
    {
        if (fat_master_fs == nullptr) { KernelIO::Terminal.Write("shell", COL4_YELLOW);}
        else {

            System::Environment::Hostname env_host;
            System::Security::Sudo sudo;
            char temp[32] {'\0'};
            strcat(temp,sudo.user);
            KernelIO::Terminal.Write(temp, COL4_CYAN);
            KernelIO::Terminal.WriteChar('@', COL4_WHITE);
            KernelIO::Terminal.Write(CurrentPath, COL4_YELLOW); 
        }

        KernelIO::Terminal.Write(":- ");
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
        if (KernelIO::Keyboard.TerminalBuffer)
        {
            ParseCommand(input);
            
            char* cmd = strsplit_index(input, 0, ' ');
            strlower(cmd);
            if (!streql(cmd, "GFX")) { PrintCaret(); }
            delete cmd;
        }
    }

    void ShellHost::ParseCommand(char* input)
    {
        // get command from input
        char* cmd = strsplit_index(input, 0, ' ');
        strlower(cmd);
        serial_writeln(cmd);

        // hack to use second command for clearing
        if (streql(cmd, "cls")) { Commands::CLEAR(input); delete cmd; return; }
        if (streql(cmd, "dir")) { Commands::LS(input); delete cmd; return; }

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

    char* ShellHost::GetCurrentPath() { return CurrentPath; }


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
                    KernelIO::Terminal.SetCursorX(16);
                    KernelIO::Terminal.WriteLine(KernelIO::Shell.GetCommand(i).Help, COL4_GRAY);
                }
            }
        }
        
        void FORMAT(char* input)
        {
            //System::KernelIO::NapalmFS.Format(true);
        }

        void DISK_DUMP(char* input)
        {
            char* str_sector = strsplit_index(input, 1, ' ');
            char* str_bytes  = strsplit_index(input, 2, ' ');
            uint8_t* buffer = new uint8_t[512];

            uint32_t sector = stod(str_sector);
            uint32_t bytes =  stod(str_bytes);
            if (bytes > 512) { bytes = 512; }

            KernelIO::ATA.ReadSectors(buffer, sector, 1);

            KernelIO::DumpMemory((uint8_t*)buffer, bytes, 12, true);
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
            uint64_t null_idtr = 0;
            asm("xor %%eax, %%eax; lidt %0; int3" :: "m" (null_idtr));
           // System::KernelIO::ACPI.Reboot();
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

        void TIME(char* input)
        {  
            KernelIO::Terminal.WriteLine(System::KernelIO::RTC.GetTimeString(true,true));
        }

        void LS(char* input)
        {
            struct directory dir;
            fat_populate_root_dir(fat_master_fs, &dir);
            char* listdir = strsplit_index(input, 1, ' ');
            if (streql(KernelIO::Shell.GetCurrentPath(), "/"))
            {
                KernelIO::Terminal.WriteLine("Showing: /");
                fat_print_directory(fat_master_fs, &dir);
            }
            else if(listdir == nullptr)
            {
                KernelIO::Terminal.Write("Showing: ");
                KernelIO::Terminal.WriteLine(KernelIO::Shell.GetCurrentPath());
                //fat_print_directory(fat_master_fs, &dir);
                //debug_writeln(KernelIO::Shell.GetCurrentPath());
                fat_dir_by_name(fat_master_fs, &dir, KernelIO::Shell.GetCurrentPath());
            }
            else
            {
                char* path = listdir;
                if (listdir[0] != '/') { path = new char[strlen(listdir) + 1]; path[0] = '/'; strcat(path, listdir); }
                KernelIO::Terminal.Write("Showing: ");
                KernelIO::Terminal.WriteLine(listdir);
                fat_dir_by_name(fat_master_fs,&dir, listdir);

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
              
        }

        void TEXTVIEW(char* input)
        {
            
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

        void PS(char* input)
        {
            List<uint64_t> result = KernelIO::TaskManager.GetPids();
            KernelIO::Terminal.WriteLine("Total Pids: %s",result.Count);
            for (int i=0; i < result.Count; i++)
            {
                KernelIO::Terminal.WriteLine("PID: %s",*result.Get(i));
            }
        }

        void CD(char* input)
        {
            System::Security::Sudo sudo;
            char* path = strsub(input, 3, strlen(input));
            struct directory* dir;
            fat_populate_root_dir(fat_master_fs,dir);
            if(path == nullptr)
            {
                char* old_dir = KernelIO::Shell.GetCurrentPath();
                char homefolder[32]{'\0'};
                strcat(homefolder,"/users/");
                strcat(homefolder,sudo.user);
                debug_writeln(sudo.user);
                KernelIO::Shell.GetCurrentPath()[0] = '\0';
                strcat(KernelIO::Shell.GetCurrentPath(),homefolder);

            
            }
            else if(streql(path,".")) {
                return;
            }
            else if(streql(path,"/")) {
                KernelIO::Shell.GetCurrentPath()[0] = '\0';
                
                if(IsEmpty(KernelIO::Shell.GetCurrentPath()))
                {
                strcat(KernelIO::Shell.GetCurrentPath(),"/");
                }
                return;
            }
            else if(streql(path,"..")) { 
            char temp_path[128]{'\0'};
            if (strlen(path) <= 0) { return; }
            if (streql(fat_change_dir(path), "/") && !streql(path, "/")) { return; }
            char* old_dir = KernelIO::Shell.GetCurrentPath();
            char* new_dir = fat_get_sub_folder(old_dir);
            debug_writeln_ext(new_dir,COL4_CYAN);
            strcat(temp_path,new_dir);
            
                KernelIO::Shell.GetCurrentPath()[0] = '\0';
                strcat(KernelIO::Shell.GetCurrentPath(),temp_path);
            }
            else if(StringContains(path,"/etc") && !sudo.CheckSudo(sudo.user))
            {
                System::KernelIO::Terminal.WriteLine("Access is denied.", COL4_RED);
                return;
            }
            else if (StringContains(path,"/etc") && sudo.CheckSudo(sudo.user))
            {
            if (strlen(path) <= 0) { return; }
            if (streql(fat_change_dir(path), "/") && !streql(path, "/")) { return; }
            KernelIO::Shell.GetCurrentPath()[0] = '\0';
            if (path[0] != '/') { stradd(KernelIO::Shell.GetCurrentPath(), '/'); }
            strcat(KernelIO::Shell.GetCurrentPath(), path); 
            }
            else
            {
            if (strlen(path) <= 0) { return; }
            if (streql(fat_change_dir(path), "/") && !streql(path, "/")) { return; }
            KernelIO::Shell.GetCurrentPath()[0] = '\0';
            if (path[0] != '/') { stradd(KernelIO::Shell.GetCurrentPath(), '/'); }
            strcat(KernelIO::Shell.GetCurrentPath(), path);  
            }
        }

        void RUN(char* input)
        {
            char* app = strsplit_index(input, 1, ' ');
            strlower(app);

            mem_free(app);
        }

        void RAT(char* input)
        {
            System::KernelIO::MemoryManager.PrintRAT(true);
        }
    }
}