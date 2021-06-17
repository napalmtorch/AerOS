#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/memory.hpp>
#include <hardware/drivers/ata_pio.hpp>
#include <core/debug.hpp>

namespace HAL
{
            // constant sectors
        #define NFS_SECTOR_BOOT_RECORD 0
        #define NFS_SECTOR_TABLE_INFO  1
        #define NFS_SECTOR_TABLE_START 2
        
        // entry types
        #define NFS_ENTRY_NULL 0
        #define NFS_ENTRY_DIR  1
        #define NFS_ENTRY_FILE 2

        // root index
        #define NFS_ROOT_INDEX 0

        // structure for managing boot record information
        typedef struct
        {
            uint32_t sector_count;                      // amount of sectors on the disk
            uint32_t bytes_per_sector;                  // amount of bytes per sector - this must be 512
            uint32_t first_table_sector;                // first sector that contains allocation table data
            uint32_t first_data_sector;                 // first sector that contains physical file data
            uint32_t signature;                         // unique filesystem signature for identification
            uint8_t  reserved[492];                     // unused memory to equate 512 bytes(1 sector)
        } __attribute__((packed)) nfs_boot_record_t;

        // structure for managing allocation table entries
        typedef struct
        {
            uint32_t current_table_sector;
            uint32_t current_table_offset;
            uint32_t current_data_sector;
            uint32_t entry_count;
        } __attribute__((packed)) nfs_table_t;

        // structure used for directory entries
        typedef struct
        {
            char     name[32];                          // name of directory - root is empty
            uint32_t parent_index;                      // index of parent directory - root is -1
            uint8_t  type;                              // type of entry
            uint8_t  status;
            uint8_t  reserved[26];                      // first byte indicates if root directory, otherwise unused memory
        } __attribute__((packed)) nfs_directory_t;

        // structures used for file entries
        typedef struct
        {
            char     name[32];                          // name of file
            uint32_t parent_index;                      // index of parent directory - root is -1
            uint8_t  type;                              // type of entry
            uint8_t  status;
            uint32_t size;                              // size of file in bytes
            uint32_t sector_start;                      // first sector of file data
            uint32_t sector_count;                      // amount of sectors used
            uint32_t data;                              // pointer to data in memory
            uint8_t  reserved[10];                      // unused memory
        } __attribute__((packed)) nfs_file_t;

    class NapalmFileSystem
    {
        public:
            void Mount();
            void ReadBootRecord();
            void ReadEntryTableHeader();
            void ReadRootDirectory();
            nfs_file_t ReadFile(char* path);
            nfs_directory_t GetParentFromPath(char* path);
            int32_t GetDirectoryIndex(nfs_directory_t dir);
            bool FileExists(char* path);
            bool DirectoryExists(char* path);
            void ListDir(char* path);
    };
}