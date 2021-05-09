#pragma once
#include "lib/types.h"

extern "C"
{
    // multiboot pointer
    extern uint32_t mbootdat;

    // structure to manage multiboot data
    typedef struct 
    {
       /* Multiboot info version number */
        uint32_t flags;
        /* Available memory from BIOS */
        uint32_t mem_lower;
        uint32_t mem_upper;
        /* "root" partition */
        uint32_t boot_device;
        /* Kernel command line */
        char* cmdline;
        /* Boot-Module list */
        uint32_t mods_count;
        uint32_t mods_addr;
        uint32_t syms[4];
        /* Memory Mapping buffer */
        uint32_t mmap_length;
        uint32_t mmap_addr;
        /* Drive Info buffer */
        uint32_t drives_length;
        uint32_t drives_addr;
        /* ROM configuration table */
        uint32_t config_table;
        /* Boot Loader Name */
        uint32_t boot_loader_name;
        /* APM table */
        uint32_t apm_table;
        /* Video */
        uint32_t vbe_control_info;
        uint32_t vbe_mode_info;
        uint16_t vbe_mode;
        uint16_t vbe_interface_seg;
        uint16_t vbe_interface_off;
        uint16_t vbe_interface_len;
    } __attribute__((packed)) multiboot_t;

    // memory map entry
    typedef struct 
    {
        uint32_t size;
        uint64_t addr;
        uint64_t len;
        uint32_t type;
    } __attribute__ ((packed)) mmap_entry_t;
}

#ifdef __cplusplus
    namespace HAL
        {
            class MultibootHeader
            {
                public:
                    // update info
                    void Read();
                    // print to screen
                    void Print();
                    // get flags
                    uint32_t GetFlags();
                    // get memory upper & lower
                    uint32_t GetMemoryUpper();
                    uint32_t GetMemoryLower();
                    // kernel command line
                    char* GetCommandLine();
                    // boot module list
                    uint32_t GetModulesAddress();
                    uint32_t GetModulesCount();
                    uint32_t* GetSymbols();
                    // memory map
                    uint32_t GetMemoryMapAddress();
                    uint32_t GetMemoryMapLength();
                    // drives
                    uint32_t GetDrivesAddress();
                    uint32_t GetDrivesLength();
                    uint32_t GetROMConfigTable();
                    // apm table
                    uint32_t GetAPMTable();
                    // vbe
                    uint32_t GetVBEControlInfo();
                    uint32_t GetVBEModeInfo();
                    uint32_t GetVBEMode();
                    uint32_t GetVBEInterfaceSegment();
                    uint32_t GetVBEInterfaceOff();
                    uint32_t GetVBEInterfaceLength();
                    // get name
                    char* GetName();
                private:
                    multiboot_t* Header;
                    uint32_t Flags;
                    uint32_t MemoryUpper;
                    uint32_t MemoryLower;
                    char* CommandLine;
                    uint32_t ModulesAddress;
                    uint32_t ModulesCount;
                    uint32_t Symbol_1;
                    uint32_t Symbol_2;
                    uint32_t Symbol_3;
                    uint32_t Symbol_4;
                    uint32_t MemoryMapAddress;
                    uint32_t MemoryMapLength;
                    uint32_t DrivesAddress;
                    uint32_t DrivesLength;
                    uint32_t ROMConfigTable;
                    uint32_t APMTable;
                    uint32_t VBEControlInfo;
                    uint32_t VBEModeInfo;
                    uint16_t VBEMode;
                    uint32_t VBEInterfaceSegment;
                    uint32_t VBEInterfaceOff;
                    uint32_t VBEInterfaceLength;
                    char* Name;
            };
        }
#endif
