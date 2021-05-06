#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "core/debug.hpp"

namespace HAL
{
    // record block
    typedef struct
    {
        uint32_t SectorCount;
        uint32_t SizeInBytes;
        uint32_t BytesPerSector;
        uint32_t MaxEntries;
        uint32_t SectorsUsed;
        uint32_t Position;
        char     Label[20];
    } __attribute__((packed)) PTFSRecord;
    
    // file entry
    typedef struct
    {
        uint32_t SectorStart;
        uint32_t SectorCount;
        uint32_t SizeInBytes;
        char     Name[250];
        char     Path[250];
    } __attribute__((packed)) PTFSFileEntry;
    

    class PTFSFileSystem
    {
        public:
            void Initialize();
            void ReadRecordBlock();
            void PrintRecordBlock();
            void Format(uint32_t size_mb);
            void CreateFile(char* path, char* name);
        private:
            uint8_t RecordBlockData[512];
            uint8_t DataBuffer[512];
            PTFSRecord* RecordBlock;
    };
}