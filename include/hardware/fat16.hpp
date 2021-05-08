#pragma once
#include <hardware/drivers/ata.hpp>
extern "C"
{
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned int uint;

#include <stdbool.h>

#define PACKED __attribute__((__packed__))

#define SECTOR_SIZE 512
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
        typedef enum
    {
        FAT_TYPE_FAT12 = 12,
        FAT_TYPE_FAT16 = 16,
        FAT_TYPE_FAT32 = 32,
        FAT_TYPE_EXFAT = 36,
    } FAT_TYPE;
// ------------------------------------------------------------------------------------------------
typedef struct BiosParamBlock
{
    u8 jump[3];
    u8 oem[8];
    u16 bytesPerSector;
    u8 sectorsPerCluster;
    u16 reservedSectorCount;
    u8 fatCount;
    u16 rootEntryCount;
    u16 sectorCount;
    u8 mediaType;
    u16 sectorsPerFat;
    u16 sectorsPerTrack;
    u16 headCount;
    u32 hiddenSectorCount;
    u32 largeSectorCount;

    // Extended block
    u8 driveNumber;
    u8 flags;
    u8 signature;
    u32 volumeId;
    u8 volumeLabel[11];
    u8 fileSystem[8];
} PACKED BiosParamBlock;

// ------------------------------------------------------------------------------------------------
typedef struct DirEntry
{
    // Following conventions of DOS 7.0
    u8 name[8];
    u8 ext[3];
    u8 attribs;
    u8 reserved;
    u8 createTimeMs;
    u16 createTime;
    u16 createDate;
    u16 accessDate;
    u16 extendedAttribsIndex;
    u16 mTime;
    u16 mDate;
    u16 clusterIndex;
    u32 fileSize;
} PACKED DirEntry;

#define ENTRY_AVAILABLE 0x00
#define ENTRY_ERASED 0xe5

// ------------------------------------------------------------------------------------------------
uint FatGetTotalSectorCount(u8 *image);
uint FatGetMetaSectorCount(u8 *image);
uint FatGetClusterCount(u8 *image);
uint FatGetImageSize(u8 *image);

u16 *FatGetTable(u8 *image, uint fatIndex);
u16 FatGetClusterValue(u8 *image, uint fatIndex, uint clusterIndex);
void FatSetClusterValue(u8 *image, uint fatIndex, uint clusterIndex, u16 value);
uint FatGetClusterOffset(u8 *image, uint clusterIndex);
DirEntry *FatGetRootDirectory(u8 *image);

u8 *FatAllocImage(uint imageSize);
bool FatInitImage(u8 *image, u8 *bootSector);

void FatSplitPath(u8 dstName[8], u8 dstExt[3], const char *path);
u16 FatFindFreeCluster(u8 *image);
void FatUpdateCluster(u8 *image, uint clusterIndex, u16 value);
DirEntry *FatFindFreeRootEntry(u8 *image);
void FatUpdateDirEntry(DirEntry *entry, u16 clusterIndex, const u8 name[8], const u8 ext[3], uint fileSize);
void FatRemoveDirEntry(DirEntry *entry);
u16 FatAddData(u8 *image, const void *data, uint len);
void FatRemoveData(u8 *image, uint rootClusterIndex);
DirEntry *FatAddFile(u8 *image, const char *path, const void *data, uint len);
void FatRemoveFile(u8 *image, DirEntry *entry);
}
namespace VFS
{
    class FAT16
    {
        public:
        void TestFat();
        void Initialise();
            bool DiskValid;
            uint8_t BootSectorData[512];
            uint8_t DataBuffer[512];
            uint8_t FATTable[32 * 1024];
            FATBootRecord* BIOSBlock;
            FAT32ExtendedBootRecord* BootRecord32;
            FAT16ExtendedBootRecord* BootRecord16;
        // read bios parameter block data from disk
            FAT_TYPE FATType;
            uint32_t TotalSectors;
            uint32_t FATSize;
            uint32_t RootSectorCount;
            uint32_t FirstDataSector;
            uint32_t FirstFATSector;
            uint32_t FirstRootSector;
            uint32_t DataSectorCount;
            uint32_t ClusterCount;
    };
}