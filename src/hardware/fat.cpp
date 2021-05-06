#include "hardware/fat.hpp"
#include "core/kernel.hpp"

namespace HAL
{  
    void FATFileSystem::Initialize()
    {
        // read bios parameter block data from disk
        System::KernelIO::ATA.ReadSectors((uint8_t*)BootSectorData, 0, 1);

        // set pointers
        BIOSBlock = (BIOSParameterBlock*)BootSectorData;
        BootRecord32 = (BootRecordFAT32*)(BootSectorData + 36);
        BootRecord16 = (BootRecordFAT16*)(BootSectorData + 36);

        // print data
        uint8_t xx = 0; 
        char temp[8];
        for (size_t i = 0; i < 512; i++)
        {
            strhex(BootSectorData[i], temp);
            if (BootSectorData[i] > 0) { debug_write_ext(temp, COL4_GREEN); }
            else { debug_write_ext(temp, COL4_RED); }
            debug_write("  ");
            xx += 4;
            if (xx >= 80) { debug_write("\n"); xx = 0; }
        }
        debug_write("\n");

        PrintBIOSParameterBlock();
        PrintExtendedBootRecord();
    }

    void FATFileSystem::PrintBIOSParameterBlock()
    {
        char temp[8];

        debug_writeln("BIOS PARAMETER BLOCK DATA");

        // jump code
        debug_write_ext("- JUMP CODE                      ", COL4_YELLOW);
        strhex(BIOSBlock->JumpCode[0], temp); debug_write(temp); debug_write("  ");
        strhex(BIOSBlock->JumpCode[1], temp); debug_write(temp); debug_write("  ");
        strhex(BIOSBlock->JumpCode[2], temp); debug_write(temp); debug_writeln("  ");

        // oem identifier
        debug_write_ext("- OEM ID                         ", COL4_YELLOW);
        debug_writeln(BIOSBlock->OEMIdentifier);

        // bytes per sector
        debug_write_ext("- BYTES/SECTOR                   ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->BytesPerSector);

        // sectors per cluster
        debug_write_ext("- SECTORS/CLUSTER                ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->SectorsPerCluster);

        // reserved sectors
        debug_write_ext("- RESERVED SECTORS               ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->ReservedSectors);

        // number of fats on disk
        debug_write_ext("- FAT COUNT                      ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->FATCount);

        // directory entries
        debug_write_ext("- DIRECTORY ENTRIES              ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->DirectoryCount);

        // logical sectors
        debug_write_ext("- LOGICAL SECTORS                ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->LogicalSectors);

        // media descriptor type
        debug_write_ext("- MEDIA DESCRIPTOR               ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->MediaDescriptorType);

        // sectors per fat
        debug_write_ext("- SECTORS/FAT                    ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->SectorsPerFAT);

        // sectors per track
        debug_write_ext("- SECTORS/TRACK                  ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->SectorsPerTrack);

        // heads
        debug_write_ext("- HEADS                          ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->HeadCount);

        // hidden sectors
        debug_write_ext("- HIDDEN SECTORS                 ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->HiddenSectors);

        // large sector count
        debug_write_ext("- LARGE SECTORS                  ", COL4_YELLOW);
        debug_writeln_dec("", BIOSBlock->LargeSectors);

    }

    void FATFileSystem::PrintExtendedBootRecord()
    {

    }
}