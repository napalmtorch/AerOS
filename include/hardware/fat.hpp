#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/memory.hpp>
#include <hardware/terminal.hpp>

// file flags
#define FS_FILE 0
#define FS_DIRECTORY 1
#define FS_INVALID 2

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

    // directory structure
    typedef struct
    {
        char      FileName[8];
        char      Extension[3];
        uint8_t   Attributes;
        uint8_t   Reserved;
        uint8_t   TimeCreatedMs;
        uint16_t  TimeCreated;
        uint16_t  DateCreated;
        uint16_t  DateLastAccessed;
        uint16_t  FirstClusterHiBytes;
        uint16_t  LastModTime;
        uint16_t  LastModDate;
        uint16_t  FirstCluster;
        uint32_t  FileSize;

    } __attribute__((packed)) FATDirectory;

    // mount info structure
    typedef struct
    {
        uint32_t SectorCount;
        uint32_t FATOffset;
        uint32_t RootEntryCount;
        uint32_t RootOffset;
        uint32_t RootSize;
        uint32_t FATSize;
        uint32_t FATEntrySize;
    } __attribute__((packed)) FATMountInfo;

    // file structure
    typedef struct
    {
        char     Name[32];
        uint32_t Flags;
        uint32_t Length;
        uint32_t ID;
        uint32_t EOF;
        uint32_t Position;
        uint32_t Cluster;
        uint32_t DeviceID;
    } __attribute__((packed)) FATFile;

    // fat file system class
    class FATFileSystem
    {
        public:
            uint8_t BootSectorData[512];
            FATBootRecord* BootRecord;
            FAT16ExtendedBootRecord* BootRecord16;
            FAT32ExtendedBootRecord* BootRecord32;

            void Initialize();
            void Mount();
            void PrintMBR();
            void PrintEXT();
            void PrintInfo();
            FATFile LocateEntry(char* dir_name);
            void ReadFile(FATFile* file, uint8_t* buffer, uint32_t len);
            FATFile OpenFile(char* file_name);
            void ConvertToDOSName(char* src, char* dest, uint32_t name_len);
            
    };
}

