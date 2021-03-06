#include <core/shell.hpp>
#include <core/kernel.hpp>
#include <hardware/sudoers.hpp>
#include <hardware/fat.hpp>
#include <hardware/hostname.hpp>
#include <hardware/drivers/rtc.hpp>
#include <lib/string.hpp>
#include <software/security.hpp>
#include <software/html_parse.hpp>

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
        RegisterCommand("format",     "Format primary disk as NFS", "",       Commands::FORMAT);
        RegisterCommand("ps",         "List all running threads","",          Commands::PS);
        RegisterCommand("kill",       "Kill Pid by Name","",                  Commands::KILL);
        RegisterCommand("whoami",     "Show current user information", "",    Commands::WHOAMI);
        RegisterCommand("winls",      "List all running windows", "",         Commands::WINLS);
        RegisterCommand("su",         "Switch to high permission User!","",   Commands::SU);
        RegisterCommand("getperms",   "Show your current user level","",      Commands::GETPERMS);
        RegisterCommand("smp_test",   "Test Syncronous Multiprocessing","",   Commands::SMPTEST);
        RegisterCommand("html_test",  "Run HTML Parser to test output","",    Commands::HTMLTEST);

        CurrentPath[0] = '\0';
        strcat(CurrentPath, "/");
        sysuser = "aeros";
        AerOS::Permissions cur;
        cur.curperms = AerOS::Perms::User;
        // print caret to screen
        PrintCaret();
    }

    void ShellHost::PrintCaret()
    {
        //if (fat_master_fs == nullptr) { KernelIO::Terminal.Write("shell", COL4_YELLOW);}
        //else {

            System::Environment::Hostname env_host;
            System::Security::Sudo sudo;
            char temp[32] {'\0'};
            strcat(temp,sudo.user);
            KernelIO::Terminal.Write(temp, COL4_CYAN);
            KernelIO::Terminal.WriteChar('@', COL4_WHITE);
            KernelIO::Terminal.Write(CurrentPath, COL4_YELLOW); 
        //}

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
        if (strlen(input) == 0) { return; }
        
        // get command from input
        char* cmd = strsplit_index(input, 0, ' ');
        strlower(cmd);

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
           /* struct directory dir;
            fat_populate_root_dir(fat_master_fs, &dir);
            char* dirname = strsplit_index(input, 1, ' ');
            if(dirname == nullptr) { System::KernelIO::Terminal.WriteLine("No filename specified"); }
            else { fat_create_dir(fat_master_fs,&dir,dirname); } */
            System::KernelIO::Terminal.WriteLine("Yes i am looking at you Napalm xDDDD",COL4_CYAN);
        }

        void TIME(char* input)
        {  
            KernelIO::Terminal.WriteLine(System::KernelIO::RTC.GetTimeString(true,true));
        }

        void LS(char* input)
        {
            char* listdir = strsplit_index(input, 1, ' ');
            if(listdir == nullptr) { 
                ShellHost Host;
            listdir = KernelIO::Shell.GetCurrentPath(); }
            else if(streql(listdir,"//")) { listdir = "/"; }
           if(!EndsWith(listdir,"/"))
           {
                stradd(listdir,'/');
           }
            System::KernelIO::NapalmFS.ListDir(listdir);
        }

        void CAT(char* input)
        {
            char* listdir = strsplit_index(input, 1, ' ');
            if(System::KernelIO::NapalmFS.FileExists(listdir)) {
                auto file = System::KernelIO::NapalmFS.ReadFile(listdir);
                System::KernelIO::Terminal.WriteLine((char*)file.data);
            }
        }
        
        void PANIC(char* input)
        {
            char* err = strsub(input, 6, strlen(input));
            if (err == nullptr) { debug_throw_panic("Nothing actually crashed, I'm just a cunt."); return; }
            debug_throw_panic(err);
        }

        void GFX(char* input)
        {
            KernelIO::XServer.Start();
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
            KernelIO::TaskManager.PrintThreads();
        }
        void KILL(char* input)
        {
         char* name = strsub(input, 5, strlen(input));
         KernelIO::Terminal.Write ("Checking if Thread '");
         KernelIO::Terminal.Write(name,COL4_CYAN);
         KernelIO::Terminal.WriteLine("' exists");
         //We first check if we have a running thread by name, this is case sensitive
         if(KernelIO::TaskManager.ThreadRunning(name)) {
             if(KernelIO::TaskManager.CheckKillPriority(name)) {
             //We found a thread 
             KernelIO::Terminal.WriteLine("Thread is Active!",COL4_GREEN);
             //Lets check if we are permitted to kill the thread first, right now we can only 
             //kill processes that belong to "aeros", all others should fail
             if(KernelIO::TaskManager.CanKill(name))
             {
             //lets check if killing the thread succeded!
             //For this we call thread->Stop(); and set the state to "Failed" since it was killed!
             //FIXME: We should also have a "Stopped" state since Halted is basically just pausing the thread and a closed thread is not always completed,
             //instead it was stopped by the user.
             if(KernelIO::TaskManager.KillRunning(name))
             {
                //Yep, we did it
               KernelIO::Terminal.WriteLine("Thread has been killed!",COL4_GREEN);  
             }
             else
             {
                 //How the fuck did we get here?
                KernelIO::Terminal.WriteLine("There was an error killing the specified thread!",COL4_RED); 
             }
             }
             else
             {
                 KernelIO::Terminal.WriteLine("You cannot kill a thread that doesnt belong to you!",COL4_RED); 
             }
             }
             else {
                 KernelIO::Terminal.WriteLine("You cannot kill a protected thread!",COL4_RED); 
             }
         }  
         else
         {
             //Thread with given name was not found!
             KernelIO::Terminal.WriteLine("Thread is not Active!",COL4_RED);
         }
        }
        void WHOAMI(char* input) {
            KernelIO::Terminal.WriteLine(KernelIO::Shell.sysuser);
        }
        void SU(char* input) {
            AerOS::Permissions set_perms;
            set_perms.curperms = AerOS::Perms::All;
            KernelIO::Shell.sysuser = "root"; return;
        }
        void CD(char* input)
        {
            char* path = strsub(input, 3, strlen(input));
            if(streql(path,"..")) {
                
                if(streql(System::KernelIO::Shell.GetCurrentPath(),"/")) { return; }
                char new_path[32]{'\0'};
                strdel(System::KernelIO::Shell.GetCurrentPath());
                auto parent = KernelIO::NapalmFS.GetParentFromPath(System::KernelIO::Shell.GetCurrentPath());
                if(parent.name != nullptr) {
                stradd(new_path,'/');
                strcat(new_path,parent.name);
                stradd(new_path,'/');
                }
                
                KernelIO::Shell.GetCurrentPath()[0] = '\0';
                strcat(System::KernelIO::Shell.GetCurrentPath(),new_path);
            } else if(streql(path,".")) { return; }
            else {
                if(KernelIO::NapalmFS.DirectoryExists(path))
                {
            KernelIO::Shell.GetCurrentPath()[0] = '\0';
            strcat(System::KernelIO::Shell.GetCurrentPath(),path);
            debug_write("New Path: ");
            debug_writeln(KernelIO::Shell.GetCurrentPath());
                }
                else
                {
                    debug_write("Path: ");
                    debug_write(path);
                    debug_writeln(" not found!");
                }
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

        void WINLS(char* input)
        {
            
        }

        void GETPERMS(char* input)
        {
            AerOS::Permissions cur_perms;
            if(cur_perms.IsGuest(KernelIO::Shell.sysuser)&&!cur_perms.IsUser(KernelIO::Shell.sysuser)&&!cur_perms.IsRoot(KernelIO::Shell.sysuser))
            {
                KernelIO::Terminal.WriteLine("Guest users dont have permissions");
            }
            else if(cur_perms.IsUser(KernelIO::Shell.sysuser)&&!cur_perms.IsGuest(KernelIO::Shell.sysuser)&&!cur_perms.IsRoot(KernelIO::Shell.sysuser))
            {
                KernelIO::Terminal.WriteLine("You seem to have User permissions");
            }
            if(cur_perms.IsRoot(KernelIO::Shell.sysuser) &&!cur_perms.IsGuest(KernelIO::Shell.sysuser)&&!cur_perms.IsUser(KernelIO::Shell.sysuser))
            {
                KernelIO::Terminal.WriteLine("Give me root and i conquer the world ;) ");
            }

        }

        void SMPTEST(char* input)
        {
            HAL::MADT* apicinfo = KernelIO::ACPI.GetAPICInfo();
            if (apicinfo == NULL)
            {
                KernelIO::Terminal.WriteLine("something went wrong!");
                return;
            }
            KernelIO::Terminal.WriteLine("CPU cores: %s", (uint32_t)apicinfo->numcore);
        }

        void HTMLTEST(char* input) {
                        //Web::Parser *parse = new Web::Parser("file:///sys/web/welcome.html");
            Web::Parser *parse = new Web::Parser("file:///sys/web/test.html");
            parse->CheckDoctype();
            parse->CheckForTitle();
            parse->CheckH1Tag();
            parse->RawDump();
        }
    }
}