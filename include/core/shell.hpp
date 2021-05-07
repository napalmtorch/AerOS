#pragma once
#include <lib/types.h>
#include <lib/string.hpp>

namespace System
{
    struct ShellCommand
    {
        char Name[16];
        char Help[64];
        char Usage[32];
        void (*Execute)(char* input);
        ShellCommand();
        ShellCommand(char* name, char help[], char usage[], void (*execute)(char*));
        ~ShellCommand();
    };

    class ShellHost
    {
        public:
            ShellHost();
            ~ShellHost();
            void Initialize();
            void PrintCaret();
            void HandleInput(char* input);
            void ParseCommand(char* input);
            char* GetCurrentPath();
        private:
            String CurrentPath;
            ShellCommand CommandList[32];
    };

    namespace Commands
    {
        void CLEAR(char* input);
        void LSPCI(char* input);
        void CPUINFO(char* input);
        void COLOR(char* input);
    }
}