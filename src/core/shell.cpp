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
        CommandList[0] = ShellCommand("CLEAR", "Clear the screen", "", Commands::CLEAR);
        CommandList[1] = ShellCommand("LSPCI", "List detected PCI devices", "", Commands::LSPCI);
        CommandList[2] = ShellCommand("CPUINFO", "List processor information", "", Commands::CPUINFO);
        CommandList[3] = ShellCommand("COLOR", "Change terminal colors", "", Commands::COLOR);

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

        for (size_t i = 0; i < 32; i++)
        {
            if (streql(cmd, CommandList[i].Name) == true)
            {
                CommandList[i].Execute(input);
                return;
            }
        }

        if (cmd != nullptr) { delete cmd; }

        // don't print if no text was typed
        if (strlen(input) > 0) { KernelIO::Terminal.WriteLine("Invalid command or file", COL4_RED); }
    }

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

        void COLOR(char* input)
        {
            char* cmd = strsplit_index(input, 0 ,' ');
            char* background = strsplit_index(input,2,' ');
            char* foreground = strsplit_index(input,1,' ');

            if (background != nullptr && foreground != nullptr)
            {
                if(streql(cmd, foreground) || streql(cmd, background))
                { 
                    KernelIO::Terminal.WriteLine("All Parameters are required");
                    KernelIO::Terminal.WriteLine("Usage: color foreground background"); 
                    KernelIO::Terminal.WriteLine("Example: color white black"); return; 
                }

                System::KernelIO::WriteLine(foreground);
                System::KernelIO::WriteLine(background);
                KernelIO::Terminal.Clear(Graphics::GetColorFromName(background));
                KernelIO::Terminal.SetForeColor(Graphics::GetColorFromName(foreground));
                KernelIO::Terminal.WriteLine("Color Set!");

                delete background;
                delete foreground;
                delete cmd;
            }       
        }
    }
}