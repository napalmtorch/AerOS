#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/memory.hpp"

#define FS_FILE         0
#define FS_DIRECTORY    1
#define FS_INVALID      2

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
    } __attribute__((packed)) FATBootRecord;

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
    } __attribute__((packed)) FAT16ExtendedBootRecord;
    

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
    } __attribute__((packed)) FAT32ExtendedBootRecord;

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
    } __attribute__((packed)) FATFSInfo32;
    
    // directory entry
    typedef struct
    {
        char     Name[8];
        char     Extension[3];
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
    } __attribute__((packed)) FATDirectoryEntry;
    
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
    } __attribute__((packed)) FATLongNameEntry;

    // file entry
    typedef struct
    {
        char    Name[32];
        uint32_t Flags;
        uint32_t FileLength;
        uint32_t ID;
        uint32_t EOF;
        uint32_t Position;
        uint32_t CurrentCluster;
        uint32_t Drive;
    } __attribute__((packed)) FATFile; 

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
            FATFile OpenFile(char* name);
            void ReadFile(FATFile file, uint8_t* dest, uint32_t len);
        private:
            bool DiskValid;
            uint8_t BootSectorData[512];
            uint8_t DataBuffer[512];
            uint8_t FATTable[32 * 1024];
            FATBootRecord* BIOSBlock;
            FAT32ExtendedBootRecord* BootRecord32;
            FAT16ExtendedBootRecord* BootRecord16;

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
            bool IsFSInfoValid(FATFSInfo32* fs_info);
            FATFile LocateEntry(char* path);
            void ConvertToDOSName(char* src, char* dest, uint32_t len);
    };
}