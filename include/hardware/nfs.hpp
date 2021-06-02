#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/memory.hpp>
#include <core/debug.hpp>

namespace HAL
{
            // constant sectors
        #define NFS_SECTOR_BOOT_RECORD 0
        #define NFS_SECTOR_TABLE_INFO  1
        #define NFS_SECTOR_TABLE_START 2
        
        // entry types
        #define NFS_ENTRY_NULL 0
        #define NFS_ENTRY_FILE 1
        #define NFS_ENTRY_DIR  2

        // root index
        #define NFS_ROOT_INDEX 0

        // structure for managing boot record information
        typedef struct
        {
            uint16_t bytes_per_sector;                  // amount of bytes per sector - this must be 512
            uint32_t sector_count;                      // amount of sectors on the disk
            uint32_t first_table_sector;                // first sector that contains allocation table data
            uint32_t first_data_sector;                 // first sector that contains physical file data
            uint8_t  reserved[498];                     // unused memory to equate 512 bytes(1 sector)
        } __attribute__((packed)) nfs_boot_record_t;

        // structure for managing allocation table entries
        typedef struct
        {
            uint32_t entry_count;                       // total amount of entries in allocation table
            uint32_t entry_sector_offset;               // amount of bytes into the sector that the allocation position is at
            uint32_t entry_sector_current;              // sector allocation pointer 
            uint32_t data_sector_current;               // current data sector
            uint8_t  reserved[496];                     // unused memory to equate 512 bytes(1 sector)
        } __attribute__((packed)) nfs_table_t;

        // structure used for directory entries
        typedef struct
        {
            char     name[32];                          // name of directory - root is empty
            int32_t  parent_index;                      // index of parent directory - root is -1
            uint8_t  type;                              // type of entry
            uint8_t  reserved[27];                      // first byte indicates if root directory, otherwise unused memory
        } __attribute__((packed)) nfs_directory_t;

        // structures used for file entries
        typedef struct
        {
            char     name[32];                          // name of file
            int32_t  parent_index;                      // index of parent directory - root is -1
            uint8_t  type;                              // type of entry
            uint32_t size;                              // size of file in bytes
            uint32_t sector_start;                      // first sector of file data
            uint32_t sector_count;                      // amount of sectors used
            uint32_t data;                              // pointer to data in memory
            uint8_t  reserved[11];                      // unused memory
        } __attribute__((packed)) nfs_file_t;

    class NapalmFileSystem
    {
        public:
            void Initialize();
            void ReadBootRecord();
            void Wipe();
            bool Format(bool wipe);
            void WriteBootRecord(uint32_t sector_size, uint32_t sector_count);
            void WriteRoot();
            void WriteTableInfo();
            void UpdateTableInfo();
            bool WriteFileEntry(nfs_file_t file);
            bool WriteDirEntry(nfs_directory_t dir);
            bool FileExists(char* path);
            bool DirectoryExists(char* path);
            int32_t GetFileIndex(nfs_file_t file);
            int32_t GetDirectoryIndex(nfs_directory_t dir);
            nfs_directory_t GetParentFromPath(char* path);
            nfs_file_t OpenFile(char* path);
            bool WriteFile(char* path, uint8_t* data, uint32_t size);
    };
}