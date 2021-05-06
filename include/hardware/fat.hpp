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
    } __attribute__((packed)) FSInfoFAT32;
    
    // directory entry
    typedef struct
    {
        uint8_t  Name[8];
        uint8_t  Extension[3];
        uint8_t  Attributes;
        uint8_t  FlagsWindowsNT;
        uint8_t  CreationTimeTenthSecond;
        uint16_t CreationTime;
        uint16_t CreationDate;
        uint16_t LastAccessDate;
        uint16_t ClusterStartHigh;
        uint16_t LastModifiedTime;
        uint16_t LastModifiedDate;
        uint16_t ClusterStartLow;
        uint32_t SizeInBytes;
    } __attribute__((packed)) DirectoryEntryFAT;
    
    // long file name entry
    typedef struct
    {
        uint8_t Order;
        uint8_t NameStart[10];
        uint8_t Attribute;
        uint8_t Type;
        uint8_t Checksum;
        uint8_t NameMiddle[12];
        uint16_t Zero;
        uint8_t NamEnd[4];
    } __attribute__((packed)) LongNameEntryFAT;
    

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
            uint8_t FATTable[32 * 1024];
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
            int32_t ReadTable(uint32_t cluster);
            int32_t WriteTable(uint32_t cluster, uint32_t cluster_val);
            bool IsFSInfoValid(FSInfoFAT* fs_info);
    };
}