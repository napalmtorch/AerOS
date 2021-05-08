
#include <hardware/fat16.hpp>
#include <hardware/memory.hpp>
#include <lib/string.hpp>

VFS::FAT16 FAT16;
HAL::ATAController ATA;
extern "C" {
// ------------------------------------------------------------------------------------------------
uint FatGetTotalSectorCount(uint8_t *image)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    if (bpb->sectorCount)
    {
        return bpb->sectorCount;
    }
    else
    {
        return bpb->largeSectorCount;
    }
}

// ------------------------------------------------------------------------------------------------
uint FatGetMetaSectorCount(uint8_t *image)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    return
        bpb->reservedSectorCount +
        bpb->fatCount * bpb->sectorsPerFat +
        (bpb->rootEntryCount * sizeof(DirEntry)) / bpb->bytesPerSector;
}

// ------------------------------------------------------------------------------------------------
uint FatGetClusterCount(uint8_t *image)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    uint totalSectorCount = FatGetTotalSectorCount(image);
    uint metaSectorCount = FatGetMetaSectorCount(image);
    uint dataSectorCount = totalSectorCount - metaSectorCount;

    return dataSectorCount / bpb->sectorsPerCluster;
}

// ------------------------------------------------------------------------------------------------
uint FatGetImageSize(uint8_t *image)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    return FatGetTotalSectorCount(image) * bpb->bytesPerSector;
}

// ------------------------------------------------------------------------------------------------
uint16_t *FatGetTable(uint8_t *image, uint fatIndex)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;


    uint offset = (bpb->reservedSectorCount + fatIndex * bpb->sectorsPerFat) * bpb->bytesPerSector;

    return (uint16_t *)(image + offset);
}

// ------------------------------------------------------------------------------------------------
uint16_t FatGetClusterValue(uint8_t *image, uint fatIndex, uint clusterIndex)
{
    uint16_t *fat = FatGetTable(image, fatIndex);

    return fat[clusterIndex];
}

// ------------------------------------------------------------------------------------------------
uint FatGetClusterOffset(uint8_t *image, uint clusterIndex)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    return
        (bpb->reservedSectorCount + bpb->fatCount * bpb->sectorsPerFat) * bpb->bytesPerSector +
        bpb->rootEntryCount * sizeof(DirEntry) +
        (clusterIndex - 2) * (bpb->sectorsPerCluster * bpb->bytesPerSector);
}

// ------------------------------------------------------------------------------------------------
void FatSetClusterValue(uint8_t *image, uint fatIndex, uint clusterIndex, uint16_t value)
{
    uint16_t *fat = FatGetTable(image, fatIndex);

    fat[clusterIndex] = value;
}

// ------------------------------------------------------------------------------------------------
DirEntry *FatGetRootDirectory(uint8_t *image)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    uint offset = (bpb->reservedSectorCount + bpb->fatCount * bpb->sectorsPerFat) * bpb->bytesPerSector;
    uint dataSize = bpb->rootEntryCount * sizeof(DirEntry);


    return (DirEntry *)(image + offset);
}

// ------------------------------------------------------------------------------------------------
uint8_t *FatAllocImage(uint imageSize)
{
    uint8_t *image = (uint8_t *)imageSize;
    
    return image;
}

// ------------------------------------------------------------------------------------------------
bool FatInitImage(uint8_t *image, uint8_t *bootSector)
{
    BiosParamBlock *bpb = (BiosParamBlock *)bootSector;

    // Validate signature
    if (bootSector[0x1fe] != 0x55 || bootSector[0x1ff] != 0xaa)
    {
        return false;
    }

    // Copy to sector 0
    mem_copy(bootSector, image, bpb->bytesPerSector);

    // Initialize clusters
    uint clusterCount = FatGetClusterCount(image);

    FatUpdateCluster(image, 0, 0xff00 | bpb->mediaType);    // media type
    FatUpdateCluster(image, 1, 0xffff);                     // end of chain cluster marker

    for (uint clusterIndex = 2; clusterIndex < clusterCount; ++clusterIndex)
    {
        FatUpdateCluster(image, clusterIndex, 0x0000);
    }

    return true;
}

// ------------------------------------------------------------------------------------------------
uint16_t FatFindFreeCluster(uint8_t *image)
{
    uint clusterCount = FatGetClusterCount(image);

    uint16_t *fat = FatGetTable(image, 0);

    for (uint clusterIndex = 2; clusterIndex < clusterCount; ++clusterIndex)
    {
        uint16_t data = fat[clusterIndex];
        if (data == 0)
        {
            return clusterIndex;
        }
    }

    return 0;
}

// ------------------------------------------------------------------------------------------------
void FatUpdateCluster(uint8_t *image, uint clusterIndex, uint16_t value)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    for (uint fatIndex = 0; fatIndex < bpb->fatCount; ++fatIndex)
    {
        FatSetClusterValue(image, fatIndex, clusterIndex, value);
    }
}

// ------------------------------------------------------------------------------------------------
DirEntry *FatFindFreeRootEntry(uint8_t *image)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;

    DirEntry *start = FatGetRootDirectory(image);
    DirEntry *end = start + bpb->rootEntryCount;

    for (DirEntry *entry = start; entry != end; ++entry)
    {
        uint8_t marker = entry->name[0];
        if (marker == ENTRY_AVAILABLE || marker == ENTRY_ERASED)
        {
            return entry;
        }
    }

    return 0;
}

char toupper(char c)
{
   if (c >= 'a' && c <= 'z')
   {
      c = c - 'a' + 'A';
   }
   return c;
}

// ------------------------------------------------------------------------------------------------
static void SetPaddedString(uint8_t *dst, uint dstLen, const char *src, uint srcLen)
{
    if (src)
    {
        if (srcLen > dstLen)
        {
            mem_copy((uint8_t*)src, dst, dstLen);
        }
        else
        {
            mem_copy((uint8_t*)src, dst, srcLen);
            mem_fill(dst + srcLen, ' ', dstLen - srcLen);
        }

        for (uint i = 0; i < dstLen; ++i)
        {
            dst[i] = toupper(dst[i]);
        }
    }
    else
    {
        mem_fill(dst, ' ', dstLen);
    }
}


char * strrchr ( const char *, int ) { }
char* strchr(const char *s, char c) {
    while (*s != '\0') {
        if (*s == c) {
            return (char *)s;
        }
        s ++;
    }
    return NULL;
}

// ------------------------------------------------------------------------------------------------
void FatSplitPath(uint8_t dstName[8], uint8_t dstExt[3], const char *path)
{
    const char *name = strrchr(path, '/');
    if (name)
    {
        name = name + 1;
    }
    else
    {
        name = path;
    }

    uint nameLen = strlen_c(name);

    char *ext = 0;
    uint extLen = 0;
    char *p = strchr(name, '.');
    if (p)
    {
        nameLen = p - name;
        ext = p + 1;
        extLen = strlen(ext);
    }

    SetPaddedString(dstName, 8, name, nameLen);
    SetPaddedString(dstExt, 3, ext, extLen);
}

// ------------------------------------------------------------------------------------------------
void FatUpdateDirEntry(DirEntry *entry, uint16_t clusterIndex, const uint8_t name[8], const uint8_t ext[3], uint fileSize)
{
    entry->clusterIndex = clusterIndex;
    mem_copy((uint8_t*)name,entry->name, sizeof(entry->name));
    mem_copy((uint8_t*)ext,entry->ext, sizeof(entry->ext));
    entry->fileSize = fileSize;
}

// ------------------------------------------------------------------------------------------------
void FatRemoveDirEntry(DirEntry *entry)
{
    entry->name[0] = ENTRY_AVAILABLE;
}

// ------------------------------------------------------------------------------------------------
uint16_t FatAddData(uint8_t *image, const void *data, uint len)
{
    BiosParamBlock *bpb = (BiosParamBlock *)image;
    uint bytesPerCluster = bpb->sectorsPerCluster * bpb->bytesPerSector;

    // Skip empty files
    if (len == 0)
    {
        return 0;
    }

    uint16_t endOfChainValue = FatGetClusterValue(image, 0, 1);

    uint16_t prevClusterIndex = 0;
    uint16_t rootClusterIndex = 0;

    // Copy data one cluster at a time.
    const uint8_t *p = (const uint8_t *)data;
    const uint8_t *end = p + len;
    while (p < end)
    {
        // Find a free cluster
        uint16_t clusterIndex = FatFindFreeCluster(image);
        if (clusterIndex == 0)
        {
            // Ran out of disk space, free allocated clusters
            if (rootClusterIndex != 0)
            {
                FatRemoveData(image, rootClusterIndex);
            }

            return 0;
        }

        // Determine amount of data to copy
        uint count = end - p;
        if (count > bytesPerCluster)
        {
            count = bytesPerCluster;
        }

        // Transfer bytes into image at cluster location
        uint offset = FatGetClusterOffset(image, clusterIndex);
        mem_copy((uint8_t*)p, image + offset, count);
        p += count;

        // Update FAT clusters
        FatUpdateCluster(image, clusterIndex, endOfChainValue);
        if (prevClusterIndex)
        {
            FatUpdateCluster(image, prevClusterIndex, clusterIndex);
        }
        else
        {
            rootClusterIndex = clusterIndex;
        }

        prevClusterIndex = clusterIndex;
    }

    return rootClusterIndex;
}

// ------------------------------------------------------------------------------------------------
void FatRemoveData(uint8_t *image, uint clusterIndex)
{

    uint16_t endOfChainValue = FatGetClusterValue(image, 0, 1);

    while (clusterIndex != endOfChainValue)
    {
        uint16_t nextClusterIndex = FatGetClusterValue(image, 0, clusterIndex);
        FatUpdateCluster(image, clusterIndex, 0);
        clusterIndex = nextClusterIndex;
    }
}

// ------------------------------------------------------------------------------------------------
DirEntry *FatAddFile(uint8_t *image, const char *path, const void *data, uint len)
{
    // Find Directory Entry
    DirEntry *entry = FatFindFreeRootEntry(image);
    if (!entry)
    {
        return 0;
    }

    // Add File
    uint16_t rootClusterIndex = FatAddData(image, data, len);
    if (!rootClusterIndex)
    {
        return 0;
    }

    // Update Directory Entry
    uint8_t name[8];
    uint8_t ext[3];
    FatSplitPath(name, ext, path);

    FatUpdateDirEntry(entry, rootClusterIndex, name, ext, len);
    return entry;
}
//TODO: Implement me please? :puppy eyes:
fs_node_t *fopen(char *filename, uint32_t flags) {

	return NULL;
}
fs_node_t *fread(char *filename) {

	return NULL;
}
// ------------------------------------------------------------------------------------------------
void FatRemoveFile(uint8_t *image, DirEntry *entry)
{
    FatRemoveData(image, entry->clusterIndex);
    FatRemoveDirEntry(entry);
}
void DetermineFATType()
    {
        if (FAT16.ClusterCount < 4085) { FAT16.FATType = FAT_TYPE_FAT12; }
        else if (FAT16.ClusterCount < 65525) { FAT16.FATType = FAT_TYPE_FAT16; }
        else if (FAT16.ClusterCount < 268435445) { FAT16.FATType = FAT_TYPE_FAT32; }
        else { FAT16.FATType = FAT_TYPE_EXFAT; }
    }
    void PrintFATInformation()
    {
        debug_writeln("FAT INFORMATION");

        debug_write_ext("- TOTAL SECTORS                  ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.TotalSectors);

        debug_write_ext("- ROOT SECTOR                    ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.FirstRootSector);

        debug_write_ext("- ROOT SECTOR COUNT              ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.RootSectorCount);

        debug_write_ext("- DATA SECTOR COUNT              ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.DataSectorCount);

        debug_write_ext("- CLUSTER COUNT                  ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.ClusterCount);

        debug_write_ext("- FAT SIZE                       ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.FATSize);

        debug_write_ext("- FIRST FAT SECTOR               ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.FirstFATSector);

        debug_write_ext("- FIRST DATA SECTOR              ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.FirstDataSector);
    }
      void PrintExtendedBootRecord()
    {
        debug_writeln("EXTENDED BOOT RECORD");

        // FAT12/FAT16
        if (FAT16.FATType == FAT_TYPE_FAT12 || FAT16.FATType == FAT_TYPE_FAT16)
        {
            // drive number
            debug_write_ext("- DRIVE NUMBER                   ", COL4_YELLOW);
            debug_writeln_dec("", FAT16.BootRecord16->DriveNumber);

            // windows nt flags
            debug_write_ext("- WINNT FLAGS                    ", COL4_YELLOW);
            debug_writeln_dec("", FAT16.BootRecord16->FlagsWindowsNT);

            // signature
            debug_write_ext("- SIGNATURE                      ", COL4_YELLOW);
            debug_writeln_hex("0x", FAT16.BootRecord16->Signature);

            // volume id serial
            debug_write_ext("- VOLUME ID SERIAL               ", COL4_YELLOW);
            debug_writeln_hex("0x", FAT16.BootRecord16->VolumeIDSerial);

            // volume label
            debug_write_ext("- VOLUME LABEL                   ", COL4_YELLOW);
            char temp[2] = { ' ', '\0' };
            for (size_t i = 0; i < 11; i++) { temp[0] = FAT16.BootRecord16->VolumeLabel[i]; debug_write(temp); }
            debug_write("\n");

            // system identifier
            debug_write_ext("- SYSTEM ID                      ", COL4_YELLOW);
            for (size_t i = 0; i < 8; i++) { temp[0] = FAT16.BootRecord16->SystemIdentifier[i]; debug_write(temp); }
            debug_write("\n");

            // boot signature
            debug_write_ext("- BOOT SIGNATURE                 ", COL4_YELLOW);
            debug_writeln_hex("0x", FAT16.BootRecord16->BootSignature);
            
        }
        // FAT32
        else if (FAT16.FATType == FAT_TYPE_FAT32)
        {
            debug_writeln_ext("FAT32", COL4_YELLOW);
        }
        // EXFAT
        else if (FAT16.FATType == FAT_TYPE_EXFAT)
        {

        }
    }
    void PrintBIOSParameterBlock()
    {
        char temp[8];

        debug_writeln("BIOS PARAMETER BLOCK DATA");

        // jump code
        debug_write_ext("- JUMP CODE                      ", COL4_YELLOW);
        strhex(FAT16.BIOSBlock->JumpCode[0], temp); debug_write(temp); debug_write("  ");
        strhex(FAT16.BIOSBlock->JumpCode[1], temp); debug_write(temp); debug_write("  ");
        strhex(FAT16.BIOSBlock->JumpCode[2], temp); debug_write(temp); debug_writeln("  ");

        // oem identifier
        debug_write_ext("- OEM ID                         ", COL4_YELLOW);
        debug_writeln(FAT16.BIOSBlock->OEMIdentifier);

        // bytes per sector
        debug_write_ext("- BYTES/SECTOR                   ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->BytesPerSector);

        // sectors per cluster
        debug_write_ext("- SECTORS/CLUSTER                ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->SectorsPerCluster);

        // reserved sectors
        debug_write_ext("- RESERVED SECTORS               ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->ReservedSectors);

        // number of fats on disk
        debug_write_ext("- FAT COUNT                      ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->FATCount);

        // directory entries
        debug_write_ext("- DIRECTORY ENTRIES              ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->DirectoryCount);

        // logical sectors
        debug_write_ext("- LOGICAL SECTORS                ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->LogicalSectors);

        // media descriptor type
        debug_write_ext("- MEDIA DESCRIPTOR               ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->MediaDescriptorType);

        // sectors per fat
        debug_write_ext("- SECTORS/FAT                    ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->SectorsPerFAT);

        // sectors per track
        debug_write_ext("- SECTORS/TRACK                  ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->SectorsPerTrack);

        // heads
        debug_write_ext("- HEADS                          ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->HeadCount);

        // hidden sectors
        debug_write_ext("- HIDDEN SECTORS                 ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->HiddenSectors);

        // large sector count
        debug_write_ext("- LARGE SECTORS                  ", COL4_YELLOW);
        debug_writeln_dec("", FAT16.BIOSBlock->LargeSectors);

    }

void Init()
{


        
        ATA.ReadSectors((uint8_t*)FAT16.BootSectorData, 0, 1);

        // confirm that boot sector is not blank
        FAT16.DiskValid = false;
        for (size_t i = 0; i < 512; i++)
        { if (FAT16.BootSectorData[i] != 0) { FAT16.DiskValid = true; break; } }
        if (!FAT16.DiskValid) { System::KernelIO::ThrowWarning("Non-bootable hard disk detected"); return; }

        // set pointers
        FAT16.BIOSBlock = (FATBootRecord*)FAT16.BootSectorData;
        FAT16.BootRecord32 = (FAT32ExtendedBootRecord*)(FAT16.BootSectorData + 36);
        FAT16.BootRecord16 = (FAT16ExtendedBootRecord*)(FAT16.BootSectorData + 36);

        // get type
        DetermineFATType();

        // calculate properties based on provided data
        FAT16.TotalSectors = (FAT16.BIOSBlock->LogicalSectors == 0) ? FAT16.BIOSBlock->LargeSectors : FAT16.BIOSBlock->LogicalSectors;
        FAT16.FATSize = (FAT16.BIOSBlock->SectorsPerFAT == 0) ? FAT16.BootRecord32->SectorsPerFAT : FAT16.BIOSBlock->SectorsPerFAT;
        FAT16.RootSectorCount = ((FAT16.BIOSBlock->DirectoryCount * 32) + (FAT16.BIOSBlock->BytesPerSector - 1)) / FAT16.BIOSBlock->BytesPerSector;
        FAT16.FirstDataSector = FAT16.BIOSBlock->ReservedSectors + (FAT16.BIOSBlock->FATCount * FAT16.FATSize) + FAT16.RootSectorCount;
        FAT16.FirstFATSector = FAT16.BIOSBlock->ReservedSectors;
        FAT16.DataSectorCount = FAT16.TotalSectors - (FAT16.BIOSBlock->ReservedSectors + (FAT16.BIOSBlock->FATCount * FAT16.FATSize) + FAT16.RootSectorCount);
        FAT16.ClusterCount = FAT16.DataSectorCount / FAT16.BIOSBlock->SectorsPerCluster;
        FAT16.FirstRootSector = FAT16.FirstDataSector - FAT16.RootSectorCount;
        uint imageSize = FAT16.TotalSectors * FAT16.DataSectorCount;
        uint8_t *image = FatAllocImage(imageSize);
        FatInitImage(image,(uint8_t*)FAT16.BootRecord16);
        //PrintBIOSParameterBlock();
        PrintExtendedBootRecord();
        //PrintFATInformation();
}
}
namespace VFS
{
    void FAT16::Initialize() { return Init(); }
}