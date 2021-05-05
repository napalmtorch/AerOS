#pragma once
#include "lib/types.h"

namespace HAL
{
    // boot record block
    typedef struct
    {
        uint32_t SectorCount;
        uint32_t SizeInBytes;
        uint32_t BytesPerSector;
        char     Label[20];
    } __attribute__((packed)) PTFSBootRecord;

    // table record block
    typedef struct
    {
        uint32_t MaxEntires;
        uint32_t Position;
        uint32_t SectorsUsed;
    } __attribute__((packed)) PTFSTableRecord;

    // file entry
    typedef struct
    {
        uint32_t SectorStart;
        uint32_t SectorCount;
        uint32_t SizeInBytes;
        char     Name[250];
        char     Path[250];
    } __attribute__((packed)) PTFSFile;

    class PTFSFileSystem
    {
        public:      
            // initialization
            void Initialize();

            // read data from boot sector
            void ReadBootRecord();

            // read data from table sector
            void ReadTableRecord();

            // generate new file allocation table
            void GenerateNewTable();

            // create file
            void CreateFile(char* path, char* name);

            // print boot sector information
            void PrintBootRecord();

            // print table record information
            void PrintTableRecord();
        private:
            uint8_t BootRecordData[512];
            uint8_t TableRecordData[512];
            uint8_t DataBuffer[512];
            PTFSBootRecord* BootRecord;
            PTFSTableRecord* TableRecord;
    };
}