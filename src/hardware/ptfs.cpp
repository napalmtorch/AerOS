#include "hardware/ptfs.hpp"
#include "core/kernel.hpp"

namespace HAL
{
    // initialize ptfs file system
    void PTFSFileSystem::Initialize()
    {
        // read data
        ReadRecordBlock();
        PrintRecordBlock();
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
        System::KernelIO::WriteLine("FILE SYSTEM INFORMATION", COL4_YELLOW);
        System::KernelIO::WriteLineDecimal("SECTORS COUNT                   ", (int32_t)RecordBlock->SectorCount);
        System::KernelIO::WriteLineDecimal("SECTORS USED                    ", (int32_t)RecordBlock->SectorsUsed);
        System::KernelIO::WriteLineDecimal("SIZE                            ", (int32_t)RecordBlock->SizeInBytes);
        System::KernelIO::WriteLineDecimal("BYTES/SECTOR                    ", (int32_t)RecordBlock->BytesPerSector);
        System::KernelIO::WriteLineDecimal("MAX ENTRIES                     ", (int32_t)RecordBlock->MaxEntries);
        System::KernelIO::WriteLineDecimal("POSITION                        ", (int32_t)RecordBlock->Position);
    }

    void PTFSFileSystem::Format(uint32_t size_mb)
    {
        // check if record block is null
        if (RecordBlock == nullptr) { return; }

        // manualy format disk for 32MB
        RecordBlock->SectorCount = (size_mb * 1024 * 1024) / 512;
        RecordBlock->BytesPerSector = 512;
        RecordBlock->SizeInBytes = size_mb * 1024 * 1024;
        RecordBlock->MaxEntries = RecordBlock->SectorCount / 20;
        RecordBlock->SectorsUsed = 1;
        RecordBlock->Position = RecordBlock->MaxEntries + 1;
        System::KernelIO::ATA.WriteSectors((uint32_t*)RecordBlockData, 0, 1);
    }
}