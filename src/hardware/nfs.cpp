#include "hardware/nfs.hpp"

namespace HAL
{
    void NapalmFileSystem::Initialize()
    {

    }

    void NapalmFileSystem::ReadBootRecord()
    {

    }
    
    void NapalmFileSystem::Wipe()
    {

    }

    bool NapalmFileSystem::Format(bool wipe)
    {

    }

    void NapalmFileSystem::WriteBootRecord(uint32_t sector_size, uint32_t sector_count)
    {

    }

    void NapalmFileSystem::WriteRoot()
    {

    }

    void NapalmFileSystem::WriteTableInfo()
    {

    }

    void NapalmFileSystem::UpdateTableInfo()
    {

    }
    
    bool NapalmFileSystem::WriteFileEntry(nfs_file_t file)
    {

    }

    bool NapalmFileSystem::WriteDirEntry(nfs_directory_t dir)
    {

    }

    bool NapalmFileSystem::FileExists(char* path)
    {

    }

    bool NapalmFileSystem::DirectoryExists(char* path)
    {
        
    }

    int32_t NapalmFileSystem::GetFileIndex(nfs_file_t file)
    {

    }

    int32_t NapalmFileSystem::GetDirectoryIndex(nfs_directory_t dir)
    {

    }

    nfs_directory_t NapalmFileSystem::GetParentFromPath(char* path)
    {

    }

    nfs_file_t NapalmFileSystem::OpenFile(char* path)
    {

    }

    bool NapalmFileSystem::WriteFile(char* path, uint8_t* data, uint32_t size)
    {
        
    }
}