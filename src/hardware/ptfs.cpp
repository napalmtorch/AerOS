#include "hardware/ptfs.hpp"
#include "core/kernel.hpp"

namespace HAL
{
    // initialization
    void PTFSFileSystem::Initialize()
    {
        ReadBootRecord();
        ReadTableRecord();

        // manually format disk as 64MB
        BootRecord->SectorCount = 131072;
        BootRecord->BytesPerSector = 512;
        BootRecord->SizeInBytes = BootRecord->SectorCount * BootRecord->BytesPerSector;
        System::KernelIO::ATA.WriteSectors(BootRecordData, 0, 1);
    }

    // read data from boot sector
    void PTFSFileSystem::ReadBootRecord()
    {
        // read data from first sector of disk
        System::KernelIO::ATA.ReadSectors(BootRecordData, 0, 1);

        // set struct pointer
        BootRecord = (PTFSBootRecord*)BootRecordData;
    }

    // read data from table sector
    void PTFSFileSystem::ReadTableRecord()
    {
        // read data from second sector of disk
        System::KernelIO::ATA.ReadSectors(TableRecordData, 1, 1);

        // set struct pointer
        TableRecord = (PTFSTableRecord*)TableRecordData;
    }

    // generate new file allocation table
    void PTFSFileSystem::GenerateNewTable()
    {

    }

    // create file
    void PTFSFileSystem::CreateFile(char* path, char* name)
    {

    }

    // print boot sector information
    void PTFSFileSystem::PrintBootRecord()
    {

    }

    // print table record information
    void PTFSFileSystem::PrintTableRecord()
    {

    }
}