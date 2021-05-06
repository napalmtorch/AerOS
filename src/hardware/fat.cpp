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

        // calculate properties based on provided data
        TotalSectors = (BIOSBlock->LogicalSectors == 0) ? BIOSBlock->LargeSectors : BIOSBlock->LogicalSectors;
        FATSize = (BIOSBlock->SectorsPerFAT == 0) ? BootRecord32->SectorsPerFAT : BIOSBlock->SectorsPerFAT;
        RootSectorCount = ((BIOSBlock->DirectoryCount * 32) + (BIOSBlock->BytesPerSector - 1)) / BIOSBlock->BytesPerSector;
        FirstDataSector = BIOSBlock->ReservedSectors + (BIOSBlock->FATCount * FATSize) + RootSectorCount;
        FirstFATSector = BIOSBlock->ReservedSectors;
        DataSectorCount = TotalSectors - (BIOSBlock->ReservedSectors + (BIOSBlock->FATCount * FATSize) + RootSectorCount);
        ClusterCount = DataSectorCount / BIOSBlock->SectorsPerCluster;
        FirstRootSector = FirstDataSector - RootSectorCount;

        // get type
        DetermineFATType();

        // print boot sector as hex
        /*
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
        */

        // print information
        PrintBIOSParameterBlock();
        PrintExtendedBootRecord();
    }

    void FATFileSystem::DetermineFATType()
    {
        if (ClusterCount < 4085) { FATType = FAT_TYPE_FAT12; }
        else if (ClusterCount < 65525) { FATType = FAT_TYPE_FAT16; }
        else if (ClusterCount < 268435445) { FATType = FAT_TYPE_FAT32; }
        else { FATType = FAT_TYPE_EXFAT; }
    }

    int32_t FATFileSystem::ReadTable(uint32_t cluster)
    {
        // check if cluster number is valid
        if (cluster < 2 || cluster >= ClusterCount) { System::KernelIO::ThrowError("Invalid cluster number"); return -1; }

        // clear fat table array
        mem_fill(FATTable, 0, 32 * 1024);

        // fat 12
        if (FATType == FAT_TYPE_FAT12)
        {
            System::KernelIO::ThrowError("FAT12 not yet implemented");
            return -1;
        }
        // fat 16
        else if (FATType == FAT_TYPE_FAT16)
        {
            // calculate offsets
            uint32_t cluster_size = BIOSBlock->BytesPerSector * BIOSBlock->SectorsPerCluster;
            uint32_t fat_offset = cluster * 2;
            uint32_t fat_sector = FirstFATSector + (fat_offset / cluster_size);
            uint32_t entry_offset = fat_offset % cluster_size;

            // read sector into table
            System::KernelIO::ATA.ReadSectors((uint8_t*)FATTable, fat_sector, 1);

            // calculate table value
            uint16_t table_value = *(uint16_t*)FATTable[entry_offset];
            
            // return table value
            return table_value;
        }
        // fat 32
        else if (FATType == FAT_TYPE_FAT32)
        {
            // calculate offsets
            uint32_t cluster_size = BIOSBlock->BytesPerSector * BIOSBlock->SectorsPerCluster;
            uint32_t fat_offset = cluster * 4;
            uint32_t fat_sector = FirstFATSector + (fat_offset / cluster_size);
            uint32_t entry_offset = fat_offset % cluster_size;

            // read sector into table
            System::KernelIO::ATA.ReadSectors((uint8_t*)FATTable, fat_sector, 1);

            // calculate fat table value - ignore high 4-bits
            uint32_t table_value = *(uint32_t*)&FATTable[entry_offset] & 0x0FFFFFFF;

            // return table value
            return table_value;
        }
        // exfat
        else if (FATType == FAT_TYPE_EXFAT)
        {
            System::KernelIO::ThrowError("EXFAT not yet implemented");
            return -1;
        }     

        return -1; 
    }

    int32_t FATFileSystem::WriteTable(uint32_t cluster, uint32_t cluster_val)
    {
        // check if cluster number is valid
        if (cluster < 2 || cluster >= ClusterCount) { System::KernelIO::ThrowError("Invalid cluster number"); return -1; }

        // clear fat table array
        mem_fill(FATTable, 0, 32 * 1024);

         // fat 12
        if (FATType == FAT_TYPE_FAT12)
        {
            System::KernelIO::ThrowError("FAT12 not yet implemented");
            return -1;
        }
        // fat 16
        else if (FATType == FAT_TYPE_FAT16)
        {
            // calculate offsets
            uint32_t cluster_size = BIOSBlock->BytesPerSector * BIOSBlock->SectorsPerCluster;
            uint32_t fat_offset = cluster * 2;
            uint32_t fat_sector = FirstFATSector + (fat_offset / cluster_size);
            uint32_t entry_offset = fat_offset % cluster_size;

            // read sector into table
            System::KernelIO::ATA.ReadSectors((uint8_t*)FATTable, fat_sector, 1);

            // copy cluster value into fat table
            *(uint16_t*)&FATTable[entry_offset] = (uint16_t)cluster_val;

            // write modified fat table to disk
            System::KernelIO::ATA.WriteSectors((uint32_t*)FATTable, fat_sector, 1);
            
            // return table value
            return 0;
        }
        // fat 32
        else if (FATType == FAT_TYPE_FAT32)
        {
            // calculate offsets
            uint32_t cluster_size = BIOSBlock->BytesPerSector * BIOSBlock->SectorsPerCluster;
            uint32_t fat_offset = cluster * 4;
            uint32_t fat_sector = FirstFATSector + (fat_offset / cluster_size);
            uint32_t entry_offset = fat_offset % cluster_size;

            // read sector into table
            System::KernelIO::ATA.ReadSectors((uint8_t*)FATTable, fat_sector, 1);

            // calculate fat table value - ignore high 4-bits
            uint32_t table_value = *(uint32_t*)&FATTable[entry_offset] & 0x0FFFFFFF;

            // return table value
            return table_value;
        }
        // exfat
        else if (FATType == FAT_TYPE_EXFAT)
        {
            System::KernelIO::ThrowError("EXFAT not yet implemented");
            return -1;
        }

        return -1;
    }

    // check if FS info structure is valid
    bool FATFileSystem::IsFSInfoValid(FSInfoFAT* fs_info)
    {
        // check if signatures are correct
        if (fs_info->LeadSignature != 0x41615252 || fs_info->Signature != 0x61417272 || fs_info->TrailSignature != 0xAA550000)
        {
            System::KernelIO::ThrowError("Invalid FSInfo structure");
            return false;
        }

        // signatures are valid
        return true;
    }

     // print bios parameter block information
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

    // print extended boot record information
    void FATFileSystem::PrintExtendedBootRecord()
    {
        debug_writeln("EXTENDED BOOT RECORD");

        // FAT12/FAT16
        if (FATType == FAT_TYPE_FAT12 || FATType == FAT_TYPE_FAT16)
        {
            // drive number
            debug_write_ext("- DRIVE NUMBER                   ", COL4_YELLOW);
            debug_writeln_dec("", BootRecord16->DriveNumber);

            // windows nt flags
            debug_write_ext("- WINNT FLAGS                    ", COL4_YELLOW);
            debug_writeln_dec("", BootRecord16->FlagsWindowsNT);

            // signature
            debug_write_ext("- SIGNATURE                      ", COL4_YELLOW);
            debug_writeln_hex("0x", BootRecord16->Signature);

            // volume id serial
            debug_write_ext("- VOLUME ID SERIAL               ", COL4_YELLOW);
            debug_writeln_dec("", BootRecord16->VolumeIDSerial);

            // volume label
            debug_write_ext("- VOLUME LABEL                   ", COL4_YELLOW);
            char temp[2] = { ' ', '\0' };
            for (size_t i = 0; i < 11; i++) { temp[0] = BootRecord16->VolumeLabel[i]; debug_write(temp); }
            debug_write("\n");

            // system identifier
            debug_write_ext("- SYSTEM ID                      ", COL4_YELLOW);
            for (size_t i = 0; i < 8; i++) { temp[0] = BootRecord16->SystemIdentifier[i]; debug_write(temp); }
            debug_write("\n");

            // boot signature
            debug_write_ext("- BOOT SIGNATURE                 ", COL4_YELLOW);
            debug_writeln_hex("0x", BootRecord16->BootSignature);
            
        }
        // FAT32
        else if (FATType == FAT_TYPE_FAT32)
        {

        }
        // EXFAT
        else if (FATType == FAT_TYPE_EXFAT)
        {

        }
    }
}