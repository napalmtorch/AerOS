#pragma once
#include <lib/types.h>
namespace System
{
    namespace BIOS
    {
        class SMBIOS
        {
            public:
                void Initialize();
                void DetectMachine();
                void SetVMwareBypass();
                int machine;
                enum MachineType {
                    Qemu = 0,
                    Bochs = 1,
                    VMWare = 2,
                    Unknown = 3
                };
                MachineType CheckMachine();
                struct SMBIOSEntryPoint 
                {
                    char EntryPointString[4];            //This is _SM_
                    unsigned char Checksum;              //This value summed with all the values of the table; should be 0 (overflow)
                    unsigned char Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS; this is 0x1F
                    unsigned char MajorVersion;          //Major Version of SMBIOS
                    unsigned char MinorVersion;          //Minor Version of SMBIOS
                    unsigned short MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
                    unsigned char EntryPointRevision;    //...
                    char FormattedArea[5];               //...
                    char EntryPointString2[5];           //This is _DMI_
                    unsigned char Checksum2;             //Checksum for values from EntryPointString2 to the end of table
                    unsigned short TableLength;          //Length of the Table containing all the structures
                    unsigned int TableAddress;	         //Address of the Table
                    unsigned short NumberOfStructures;   //Number of structures in the table
                    unsigned char BCDRevision;           //Unused
                };
                struct SMBIOSHeader 
                {
                    unsigned char Type;
                    unsigned char Length;
                    unsigned short Handle;
                };
                enum SMBIOSType {
                    BiosInformation = 0,
                    SystemInformation = 1,
                    SystemEnclosure = 3,
                    ProcessorInformation = 4,
                    CacheInformation = 7,
                    SystemSlot = 9,
                    PhysicalMemoryArray = 16,
                    MemoryDevice = 17,
                    MemoryArrayMappedAddress = 19,
                    SystemBootInformation = 32
                };
                struct SMBIOSBiosInformation {
                    unsigned char header;
                    char* vendor;
                    char* version;
                    uint16_t starting_address_segment;
                    char* release_date;
                    uint8_t rom_size;
                };
            private:
                bool SMBiosDebug = true;
                char *mem;
                unsigned int smb;
                int timeout = 1000;
                size_t smbios_table_len(struct SMBIOSHeader * hd);
        };
        
    }
}