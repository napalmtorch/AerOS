#include <hardware/config_parser.hpp>
#include <core/debug.hpp>
namespace System
{
    namespace Config
    {
        ConfigParser::ConfigParser(char* file)
        {
            ParseConfigFile(file);
        }
        void ConfigParser::ParseConfigFile(char* file)
        {
            if(fat_master_fs != nullptr)
            {
                if(fat_file_exists(file))
                {
                    file_t * stream;
                    stream = fopen(file, "r");
                    int count = fread(&file_buffer, sizeof(char), 4096, stream);
                    fclose(stream);
                }
            }
        }
        char* ConfigParser::GetConfigValue(char* string,ResponseType resp)
        {   debug_write_ext("ConfigParser>> ",COL4_CYAN);
            debug_write("parsing config for : ");
            debug_writeln(string);
            char return_string[128]{'\0'};
            uint32_t array_start = 0;
            file_buffer[strlen(file_buffer) - 1] = '\0';
            if(strstr(file_buffer, string)!=NULL)
            {
                char** value = strsplit(file_buffer, '=', &array_start);
                for(uint32_t i=0; i < sizeof(file_buffer); i++)
                {
                    if(value[i]==nullptr) { continue; }
                    if(IsEmpty(value[i])) { continue; }
                    if(strstr(value[i],string)!=NULL)
                    {
                        debug_write_ext("Found Value: ",COL4_CYAN);
                        return value[i+1];
                        break;
                    }
                }
                if(resp == ResponseTypeString)
                    return "Config value not found";
                else if(resp == ResponseTypeBool || resp == ResponseTypeUint || resp == ResponseTypeInt)
                    return 0;
            }
        }

        char* ConfigParser::GetConfigString(char* string)
        {
                return ConfigParser::GetConfigValue(string,ResponseTypeString);    
        }

        bool ConfigParser::GetConfigBool(char* string)
        {
            char* response = GetConfigValue(string,ResponseTypeBool);
            if(streql(response,"True") || streql(response,"TRUE") || streql(response,"true") || streql(response,"1"))
            {
                return true;
            }
            else return false;
        }
        uint32_t ConfigParser::GetConfigUint(char* string)
        {
            return (uint32_t)GetConfigValue(string,ResponseTypeUint);
        }
        
        int32_t ConfigParser::GetConfigInt(char* string)
        {
            return (int32_t)GetConfigValue(string,ResponseTypeInt);
        }
    }
}