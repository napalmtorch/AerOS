#include "hardware/ptfs.hpp"
#include "core/kernel.hpp"

namespace HAL
{
    // initialize ptfs file system
    void PTFSFileSystem::Initialize()
    {
        // read data
        ReadRecordBlock();
    }

    // read record block on disk
    void PTFSFileSystem::ReadRecordBlock()
    {
        // read data from first sector of disk
        System::KernelIO::ATA.ReadSectors(RecordBlockData, 0, 1);

        // set struct pointer
        RecordBlock = (PTFSRecord*)RecordBlockData;
    }

    // print record block data to debugger
    void PTFSFileSystem::PrintRecordBlock()
    {

    }

    void PTFSFileSystem::Format(uint32_t size_mb)
    {
        // check if record block is null
        if (RecordBlock == nullptr) { return; }

        // manualy format disk for 32MB
        RecordBlock->SectorCount = (size_mb * 1024 * 1024) / 512;
        RecordBlock->BytesPerSector = 512;
        RecordBlock->SizeInBytes = size_mb * 1024 * 1024;
        System::KernelIO::ATA.WriteSectors((uint32_t*)RecordBlockData, 0, 1);
    }
}