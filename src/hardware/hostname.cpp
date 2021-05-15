#include <core/kernel.hpp>
#include <hardware/hostname.hpp>

namespace System
{
  namespace Environment
  {
     char* Hostname::GetHostName()
     {
        char* Document = (char*)new uint8_t[1000];
            
        // load message of the day
        if(fat_master_fs != nullptr)
        {
              
            // open file
            file_t *f = fopen("/etc/hostname", NULL);

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
