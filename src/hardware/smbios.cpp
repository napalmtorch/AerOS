#include <lib/types.h>
#include <hardware/smbios.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace BIOS
    {
        struct SMBIOS::SMBIOSHeader* smbios;
        struct SMBIOS::SMBIOSEntryPoint* smbinfo;
        struct SMBIOS::SMBIOSBiosInformation* smbios_biosinfo;
        bool IsVMWare;
        bool SMBiosPresent;
        void SMBIOS::Initialize()
        {
            mem = (char *) 0xF0000;
            int length, i;
            unsigned char checksum;
            while ((unsigned int) mem < 0x100000) {
                if (mem[0] == '_' && mem[1] == 'S' && mem[2] == 'M' && mem[3] == '_') 
                {
                length = mem[5];
                checksum = 0;
                    for(i = 0; i < length; i++) {
                        checksum += mem[i];
                    }
                    if(checksum == 0) { break; }
                    
                }
            mem += 16;
            }
            if ((unsigned int) mem == 0x100000)
            {
                KernelIO::ThrowWarning("Could not locate SMBios");
            }
            else
            {
                SMBiosPresent = true;
            }

        }
        void SMBIOS::SetVMwareBypass()
        {
            IsVMWare = true;
        }
        void SMBIOS::DetectMachine()
        {
            if(SMBiosPresent) {
                if(!IsVMWare)
                {
                    smb = SMBIOS::smbios_table_len(smbios);
                    mem_copy((uint8_t*)mem,(uint8_t*)smbinfo,smb);
                    char *RawSMBios = (char *)smbinfo->TableAddress;
                    char DetectionString[1000];
                    for(int i=0; i < smbinfo->TableLength; i++)
                    {
                        stradd(DetectionString,RawSMBios[i]);
                    }
                    if(strstr(DetectionString,"QEMU") != NULL)
                    {
                        machine = 0;
                    }
                    else if(strstr(DetectionString,"Bochs") != NULL)
                    {
                        machine = 1;
                    }
                    else
                    {
                        debug_writeln(DetectionString);
                        machine = 3;
                    }
                }
                else
                {
                machine = 2;
                }
            }
        }

        SMBIOS::MachineType SMBIOS::CheckMachine()
        {
            if(machine == 0) { return Qemu; }
            if(machine == 1) { return Bochs; }
            if(machine == 2) { return VMWare; }
            if(machine == 3) { return Unknown; }
        }

        size_t SMBIOS::smbios_table_len(struct SMBIOSHeader *hd)
        {
        size_t i;
        const char *strtab = (char *)hd + hd->Length;
        for (i = 1; strtab[i - 1] != '\0' || strtab[i] != '\0'; i++);
        return hd->Length + i + 1;
        }
    }
    
}