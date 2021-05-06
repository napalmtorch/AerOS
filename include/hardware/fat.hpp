#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/memory.hpp"

namespace HAL
{
    // bios parameter block
    typedef struct
    {
        uint8_t  JumpCode[3];
        char     OEMIdentifier[8];
        uint16_t BytesPerSector;
        uint8_t  SectorsPerCluster;
        uint16_t ReservedSectors;
        uint8_t  FATCount;
        uint16_t DirectoryCount;
        uint16_t LogicalSectors;
        uint8_t  MediaDescriptorType;
        uint16_t SectorsPerFAT;
        uint16_t SectorsPerTrack;
        uint16_t HeadCount;
        uint32_t HiddenSectors;
        uint32_t LargeSectors;
    } __attribute__((packed)) BIOSParameterBlock;

    // FAT 12/16 extended boot record
    typedef struct
    {
        uint8_t  DriveNumber;
        uint8_t  FlagsWindowsNT;
        uint8_t  Signature;
        uint32_t VolumeIDSerial;
        char     VolumeLabel[11];
        char     SystemIdentifier[8];
        uint8_t  BootCode[448];
        uint16_t BootSignature;
    } __attribute__((packed)) BootRecordFAT16;
    

    // FAT 32 extended boot record
    typedef struct
    {
        uint32_t SectorsPerFAT;
        uint16_t Flags;
        uint16_t FATVerison;
        uint32_t RootClusterNumber;
        uint16_t FSInfoSector;
        uint16_t BackupBootSector;
        uint8_t  Reserved[12];
        uint8_t  DriveNumber;
        uint8_t  FlagsWindowsNT;
        uint8_t  Signature;
        uint32_t VolumeIDSerial;
        char     VolumeLabel[11];
        char     SystemIdentifier[8];
        uint8_t  BootCode[420];
        uint16_t BootSignature;
    } __attribute__((packed)) BootRecordFAT32;

    // file system info structure
    typedef struct
    {
        uint32_t LeadSignature;
        uint8_t  Unused[480];
        uint32_t Signature;
        uint32_t FreeClusterCount;
        uint32_t AvailableClusterStart;
        uint8_t  Reserved[12];
        uint32_t TrailSignature;
    } __attribute__((packed)) FSInfoFAT;
    
    typedef enum
    {
        FAT_TYPE_FAT12 = 12,
        FAT_TYPE_FAT16 = 16,
        FAT_TYPE_FAT32 = 32,
        FAT_TYPE_EXFAT = 36,
    } FAT_TYPE;
    
    class FATFileSystem
    {
        public:
            void Initialize();
            void PrintBIOSParameterBlock();
            void PrintExtendedBootRecord();
        private:
            uint8_t BootSectorData[512];
            uint8_t DataBuffer[512];
            BIOSParameterBlock* BIOSBlock;
            BootRecordFAT32* BootRecord32;
            BootRecordFAT16* BootRecord16;

            FAT_TYPE FATType;
            uint32_t TotalSectors;
            uint32_t FATSize;
            uint32_t RootSectorCount;
            uint32_t FirstDataSector;
            uint32_t FirstFATSector;
            uint32_t FirstRootSector;
            uint32_t DataSectorCount;
            uint32_t ClusterCount;

            void DetermineFATType();
            bool IsFSInfoValid(FSInfoFAT* fs_info);
    };
}