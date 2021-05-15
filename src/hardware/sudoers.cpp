#include <core/kernel.hpp>
#include <hardware/sudoers.hpp>
namespace System
{
namespace Security
{
    bool Sudo::CheckSudo(char* user)
    {
        if(StringContains(Sudo::ParseSudoers(),user))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    char* Sudo::ParseSudoers()
    {
        Document = (char*)new uint8_t[1000];
            
        // load message of the day
        if(fat_master_fs != nullptr)
        {
              
            // open file
            file_t *f = fopen(sudoers, NULL);

            if(f)
            { 
                uint8_t c[1000];
                int count;
                while((count = fread(&c, 1000, 1, f)), count > 0) 
                {
                    for(int i = 0; i < count; i++) { stradd(Document, c[i]); }
                }
            }
        }
        return Document;
    }

}

}