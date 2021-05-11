#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <graphics/canvas.hpp>

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
            ShellCommand GetCommand(uint32_t index);
        private:
            String CurrentPath;
            ShellCommand CommandList[32];
    };

    namespace Commands
    {
        void CLEAR(char* input);
        void LSPCI(char* input);
        void CPUINFO(char* input);
        void FG(char* input);
        void BG(char* input);
        void DUMP(char* input);
        void HELP(char* input);
        void DISK_DUMP(char* input);
        void SHUTDOWN(char* input);
        void LEGACY_SHUTDOWN(char* input);
        void REBOOT(char* input);
        void TEST(char* input);
        void PANIC(char* input);
        void FAT_MBR(char* input);
        void FAT_EXT(char* input);
        void FAT_INFO(char* input);
        void GFX(char* input);
        void LIST_TEST(char*input);
        void LS(char*input);
        void CAT(char*input);
        void MKDIR(char*input);
    }
}