#pragma once
#include <lib/types.h>
#include <hardware/fat.hpp>
namespace System
{
    namespace Config
    {
        class ConfigParser
        {
            public:
                ConfigParser(char* file);
                char* GetConfigString(char* string);
                bool GetConfigBool(char* string);
                uint32_t GetConfigUint(char* string);
                int32_t GetConfigInt(char* string);
            private:
                char* GetConfigValue(char* string);
                void ParseConfigFile(char* file);
                char file_buffer[4096]{'\0'};
        };
    }
}