#include "hardware/multiboot.hpp"

namespace HAL
{
    // read info from memory
    void MultibootHeader::Read()
    {
        this->Header = (multiboot_t*)mbootdat;
        this->Flags = Header->flags;
        this->MemoryUpper = Header->mem_upper;
        this->MemoryLower = Header->mem_lower;
        this->CommandLine = Header->cmdline;
        this->ModulesAddress = Header->mods_addr;
        this->ModulesCount = Header->mods_count;
        this->Symbol_1 = Header->syms[0];
        this->Symbol_2 = Header->syms[1];
        this->Symbol_3 = Header->syms[2];
        this->Symbol_4 = Header->syms[3];
        this->MemoryMapAddress = Header->mmap_addr;
        this->MemoryMapLength = Header->mmap_length;
        this->DrivesAddress = Header->drives_addr;
        this->DrivesLength = Header->drives_length;
        this->ROMConfigTable = Header->config_table;
        this->APMTable = Header->apm_table;
        this->Name = (char*)Header->boot_loader_name;
    }
  
    // flags
    uint32_t MultibootHeader::GetFlags() { return Flags; }
    // upper memory
    uint32_t MultibootHeader::GetMemoryUpper() { return MemoryUpper; }
    // lower memory
    uint32_t MultibootHeader::GetMemoryLower() { return MemoryLower; }
    // kernel command line
    uint32_t MultibootHeader::GetCommandLine() { return CommandLine; }
    // boot modules address
    uint32_t MultibootHeader::GetModulesAddress() { return ModulesAddress; }
    // boot modules count
    uint32_t MultibootHeader::GetModulesCount() { return ModulesCount; }
    // symbols
    uint32_t* MultibootHeader::GetSymbols() { return 0; } //{ return new uint32_t[4] { Symbol_1, Symbol_2, Symbol_3, Symbol_4 }; }
    // memory map address
    uint32_t MultibootHeader::GetMemoryMapAddress() { return MemoryMapAddress; }
    // memory map length
    uint32_t MultibootHeader::GetMemoryMapLength() { return MemoryMapLength; }
    // drives address
    uint32_t MultibootHeader::GetDrivesAddress() { return DrivesAddress; }
    // drives length
    uint32_t MultibootHeader::GetDrivesLength() { return DrivesLength; }
    // rom configuration table
    uint32_t MultibootHeader::GetROMConfigTable() { return ROMConfigTable; }
    // apm table
    uint32_t MultibootHeader::GetAPMTable() { return APMTable; }
    // vbe control info
    uint32_t MultibootHeader::GetVBEControlInfo() { return VBEControlInfo; }
    // vbe mode info
    uint32_t MultibootHeader::GetVBEModeInfo() { return VBEModeInfo; }
    // vbe mode
    uint32_t MultibootHeader::GetVBEMode() { return VBEMode; }
    // vbe interface segment
    uint32_t MultibootHeader::GetVBEInterfaceSegment() { return VBEInterfaceSegment; }
    // vbe interface off
    uint32_t MultibootHeader::GetVBEInterfaceOff() { return VBEInterfaceOff; }
    // vbe interface length
    uint32_t MultibootHeader::GetVBEInterfaceLength() { return VBEInterfaceLength; }
    // get name
    char* MultibootHeader::GetName() { return Name; }
}