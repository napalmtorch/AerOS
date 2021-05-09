#pragma once
#include <hardware/drivers/ata.hpp>
#include "hardware/fat.hpp"
extern "C"
{

#include <stdbool.h>

#define PACKED __attribute__((__packed__))
#define SECTOR_SIZE 512
    
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
    uint8_t jump[3];
    uint8_t oem[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectorCount;
    uint8_t fatCount;
    uint16_t rootEntryCount;
    uint16_t sectorCount;
    uint8_t mediaType;
    uint16_t sectorsPerFat;
    uint16_t sectorsPerTrack;
    uint16_t headCount;
    uint32_t hiddenSectorCount;
    uint32_t largeSectorCount;

    // Extended block
    uint8_t driveNumber;
    uint8_t flags;
    uint8_t signature;
    uint32_t volumeId;
    uint8_t volumeLabel[11];
    uint8_t fileSystem[8];
} PACKED BiosParamBlock;

// ------------------------------------------------------------------------------------------------
typedef struct DirEntry
{
    // Following conventions of DOS 7.0
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attribs;
    uint8_t reserved;
    uint8_t createTimeMs;
    uint16_t createTime;
    uint16_t createDate;
    uint16_t accessDate;
    uint16_t extendedAttribsIndex;
    uint16_t mTime;
    uint16_t mDate;
    uint16_t clusterIndex;
    uint32_t fileSize;
} PACKED DirEntry;

typedef struct fs_node {
	char name[256];			// The filename.
	uint32_t mask;			// The permissions mask.
	uint32_t uid;			// The owning user.
	uint32_t gid;			// The owning group.
	uint32_t flags;			// Flags (node type, etc).
	uint32_t inode;			// Inode number.
	uint32_t length;		// Size of the file, in byte.
	uint32_t impl;			// Used to keep track which fs it belongs to.
	struct fs_node *ptr;	// Used by mountpoints and symlinks.
	uint32_t offset;
	int32_t shared_with;
	uint32_t atime;
	uint32_t mtime;
	uint32_t ctime;
} fs_node_t;

#define ENTRY_AVAILABLE 0x00
#define ENTRY_ERASED 0xe5

// ------------------------------------------------------------------------------------------------
uint FatGetTotalSectorCount(uint8_t *image);
uint FatGetMetaSectorCount(uint8_t *image);
uint FatGetClusterCount(uint8_t *image);
uint FatGetImageSize(uint8_t *image);

uint16_t *FatGetTable(uint8_t *image, uint fatIndex);
uint16_t FatGetClusterValue(uint8_t *image, uint fatIndex, uint clusterIndex);
void FatSetClusterValue(uint8_t *image, uint fatIndex, uint clusterIndex, uint16_t value);
uint FatGetClusterOffset(uint8_t *image, uint clusterIndex);
DirEntry *FatGetRootDirectory(uint8_t *image);

uint8_t *FatAllocImage(uint imageSize);
bool FatInitImage(uint8_t *image, uint8_t *bootSector);

void FatSplitPath(uint8_t dstName[8], uint8_t dstExt[3], const char *path);
uint16_t FatFindFreeCluster(uint8_t *image);
void FatUpdateCluster(uint8_t *image, uint clusterIndex, uint16_t value);
DirEntry *FatFindFreeRootEntry(uint8_t *image);
void FatUpdateDirEntry(DirEntry *entry, uint16_t clusterIndex, const uint8_t name[8], const uint8_t ext[3], uint fileSize);
void FatRemoveDirEntry(DirEntry *entry);
uint16_t FatAddData(uint8_t *image, const void *data, uint len);
void FatRemoveData(uint8_t *image, uint rootClusterIndex);
DirEntry *FatAddFile(uint8_t *image, const char *path, const void *data, uint len);
void FatRemoveFile(uint8_t *image, DirEntry *entry);
}
namespace VFS
{
    class FAT16
    {
        public:
            void TestFat();
            void Initialize();
            void PrintMBR();
            void PrintEXT();
            void PrintInfo();
            bool DiskValid;
            uint8_t BootSectorData[512];
            uint8_t DataBuffer[512];
            uint8_t FATTable[32 * 1024];
            HAL::FATBootRecord* BIOSBlock;
            HAL::FAT32ExtendedBootRecord* BootRecord32;
            HAL::FAT16ExtendedBootRecord* BootRecord16;
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