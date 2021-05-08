#include "hardware/fat.hpp"
#include "core/kernel.hpp"

namespace HAL
{  
    void FATFileSystem::Initialize()
    {
        // read bios parameter block data from disk
        System::KernelIO::ATA.ReadSectors((uint8_t*)BootSectorData, 0, 1);

        // confirm that boot sector is not blank
        DiskValid = false;
        for (size_t i = 0; i < 512; i++)
        { if (BootSectorData[i] != 0) { DiskValid = true; break; } }
        if (!DiskValid) { System::KernelIO::ThrowWarning("Non-bootable hard disk detected"); return; }

        // set pointers
        BIOSBlock = (FATBootRecord*)BootSectorData;
        BootRecord32 = (FAT32ExtendedBootRecord*)(BootSectorData + 36);
        BootRecord16 = (FAT16ExtendedBootRecord*)(BootSectorData + 36);

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
        debug_write("\n"); */
        //System::KernelIO::DumpMemory(BootSectorData, 512, 12, true);  
        //System::KernelIO::ATA.ReadSectors(DataBuffer, 0x13, 1);      
        //System::KernelIO::DumpMemory(DataBuffer, 512, 12, true);  
    }

    void FATFileSystem::DetermineFATType()
    {
        if (ClusterCount < 4085) { FATType = FAT_TYPE_FAT12; }
        else if (ClusterCount < 65525) { FATType = FAT_TYPE_FAT16; }
        else if (ClusterCount < 268435445) { FATType = FAT_TYPE_FAT32; }
        else { FATType = FAT_TYPE_EXFAT; }
    }

    // check if FS info structure is valid
    bool FATFileSystem::IsFSInfoValid(FATFSInfo32* fs_info)
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

    void FATFileSystem::LoadRootDirectory()
    {
        // load data into buffer
        System::KernelIO::ATA.ReadSectors(FATTable, 0, 64);
    }

    FATFile FATFileSystem::LocateEntry(char* path)
    {
        // output file
        FATFile file;
        FATDirectoryEntry* directory;

        // clear buffer
        mem_fill((uint8_t*)DataBuffer, 0, 512);

        // convert name to dos-compatible FAT-12
        char name_dos[11];
        strcpy(path, name_dos);
        ConvertToDOSName(path, name_dos, 11);

        for (size_t sec = 0; sec < 14; sec++)
        {
            // read sector data
            System::KernelIO::ATA.ReadSectors((uint8_t*)DataBuffer, FirstRootSector + sec, 1);

            // load structure
            directory = (FATDirectoryEntry*)DataBuffer;

            // 16 entries per sector
            for (size_t i = 0; i < 16; i++)
            {
                // get current filename
                char name[11];
                strcpy(directory->Name, name);
                name[11] = 0;

                // check for match
                if (strcmp(name_dos, name) == 0)
                {
                    // located file
                    strcpy(path, file.Name);
                    file.ID = 0;
                    file.CurrentCluster = directory->ClusterStartLow;
                    file.EOF = 0;
                    file.FileLength = directory->SizeInBytes;

                    // set file type
                    if (directory->Attributes == 0x10) { file.Flags = FS_DIRECTORY; }
                    else { file.Flags = FS_FILE; }
                    
                    // return file
                    return file;
                }

                // go to next directory
                directory++;
            }
        }

        // unable to find file
        file.Flags = FS_INVALID;
        return file;
    }

    void FATFileSystem::ReadFile(FATFile file, uint8_t* dest, uint32_t len)
    {
        // starting physical sector
        uint32_t physical_sector = 32 + (file.CurrentCluster - 1);
        
        // read data from sector
        System::KernelIO::ATA.ReadSectors((uint8_t*)DataBuffer, physical_sector, 1);

        // copy data to destination
        mem_copy(DataBuffer, dest, len);

        // locate fat sector
        uint32_t fat_offset = file.CurrentCluster + (file.CurrentCluster / 2);
        uint32_t fat_sector = 1 + (fat_offset / BIOSBlock->BytesPerSector);
        uint32_t entry_offset = fat_offset % BIOSBlock->BytesPerSector;

        // read first fat sector
        System::KernelIO::ATA.ReadSectors((uint8_t*)DataBuffer, fat_sector, 1);
        mem_copy(DataBuffer, FATTable, 512);

        // read second fat sector
        System::KernelIO::ATA.ReadSectors((uint8_t*)DataBuffer, fat_sector + 1, 1);
        mem_copy(DataBuffer, FATTable + 512, 512);

        // read entry for next cluster
        uint16_t next_cluster = *(uint16_t*)&FATTable[entry_offset];

        // test if entry is odd or even
        if (file.CurrentCluster & 0x0001) 
            { next_cluster >>= 4; } // grab high 12 bits
        else { next_cluster &= 0x0FFF; } // grab low 12 bits

        // test for end of file
        if (next_cluster >= 0xFF8)
        {
            file.EOF = 1;
            return;
        }

        // test for file corruption
        if (next_cluster == 0)
        {
            file.EOF = 1;
            return;
        }

        file.CurrentCluster = next_cluster;
    }

    FATFile FATFileSystem::OpenFile(char* name)
    {
        FATFile current_dir;
        char* p;
        bool root_dir = true;
        char path[11];
        strcpy(name, path);

        for (size_t i = 0; i < strlen(path); i++)
        {
            if (path[i] == '\\') { p = (char*)(path + i); }
        }

        if (!p)
        {
            // search root directory
            current_dir = LocateEntry(path);

            // check if file found
            if (current_dir.Flags == FS_FILE) { return current_dir; }

            // unable to fine
            FATFile ret;
            ret.Flags = FS_INVALID;
            return ret;
        }
    }

    // convert name to dos compatible
    void FATFileSystem::ConvertToDOSName(char* src, char* dest, uint32_t len)
    {
        uint32_t i = 0;

        // check if name is valid
        if (len > 11) { return; }
        if (!src || !dest) { return; }

        // set all characters in output name to spaces
        mem_fill((uint8_t*)dest, ' ', len);

        // name
        for (i=0; i < strlen(src)-1 && i < len; i++) 
        {
            if (src[i] == '.' || i==8 ) break;
        }

        // add extension if needed
        if (src[i]=='.') 
        {
            // cant just copy over-extension might not be 3 chars
            for (int k=0; k<3; k++) {

                ++i;
                if ( src[i] )
                    dest[8+k] = src[i];
            }
        }

        strupper(dest);
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

        debug_write_ext("- FAT TYPE:                      ", COL4_YELLOW);
        if (FATType == FAT_TYPE_FAT12) { debug_writeln("FAT-12"); }
        else if (FATType == FAT_TYPE_FAT16) { debug_writeln("FAT-16"); }
        else if (FATType == FAT_TYPE_FAT32) { debug_writeln("FAT-32"); }
        else if (FATType == FAT_TYPE_EXFAT) { debug_writeln("EX-FAT"); }

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
            debug_writeln_ext("FAT32", COL4_YELLOW);
        }
        // EXFAT
        else if (FATType == FAT_TYPE_EXFAT)
        {
            debug_throw_message(MSG_TYPE_ERROR, "EXFAT not supported");
        }
    }

    // print fat calculations information
    void FATFileSystem::PrintFATInformation()
    {
        debug_writeln("FAT INFORMATION");

        debug_write_ext("- TOTAL SECTORS                  ", COL4_YELLOW);
        debug_writeln_dec("", TotalSectors);

        debug_write_ext("- ROOT SECTOR                    ", COL4_YELLOW);
        debug_writeln_dec("", FirstRootSector);

        debug_write_ext("- ROOT SECTOR COUNT              ", COL4_YELLOW);
        debug_writeln_dec("", RootSectorCount);

        debug_write_ext("- DATA SECTOR COUNT              ", COL4_YELLOW);
        debug_writeln_dec("", DataSectorCount);

        debug_write_ext("- CLUSTER COUNT                  ", COL4_YELLOW);
        debug_writeln_dec("", ClusterCount);

        debug_write_ext("- FAT SIZE                       ", COL4_YELLOW);
        debug_writeln_dec("", FATSize);

        debug_write_ext("- FIRST FAT SECTOR               ", COL4_YELLOW);
        debug_writeln_dec("", FirstFATSector);

        debug_write_ext("- FIRST DATA SECTOR              ", COL4_YELLOW);
        debug_writeln_dec("", FirstDataSector);
    }
}