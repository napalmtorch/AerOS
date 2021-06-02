#include "hardware/nfs.hpp"
#include "core/kernel.hpp"

namespace HAL
{
     // empty entries
    nfs_file_t      null_file = { 0 };
    nfs_directory_t null_dir  = { 0 };

    // sector buffer
    uint32_t sec_size = 512;
    uint8_t* sec_buff;

    // data structures
    nfs_boot_record_t boot_record;
    nfs_table_t       entry_table;
    nfs_directory_t   root_dir;

    // modify buffer
    void sec_clear() { mem_fill(sec_buff, 0, sec_size); }
    void sec_read(uint32_t sec) { ata_pio_read48(sec, 1, sec_buff); }
    void sec_write(uint32_t sec) { ata_pio_write48(sec, 1, sec_buff); }

    char** str_lsplit(char* text, char sep, uint32_t* len)
    {
        char** output;

        uint32_t delim_count = 1;
        for (size_t i = 0; i < strlen(text); i++) { if (text[i] == sep) { delim_count++; } }
        output = new char*[delim_count];

        for (size_t i = 0; i < delim_count; i++)
        {
            output[i] = strsplit_index(text, i, sep);
            *len += 1;
        }
        return output;
    }


    void NapalmFileSystem::Initialize()
    {
        // create sector buffer
        sec_buff = (uint8_t*)mem_alloc(sec_size);

        // format disk
        Format(true);

        // read boot record from disk
        ReadBootRecord();
    }

    void NapalmFileSystem::ReadBootRecord()
    {
        sec_read(NFS_SECTOR_BOOT_RECORD);

        // load data into structure
        nfs_boot_record_t* rec = (nfs_boot_record_t*)sec_buff;
        boot_record.bytes_per_sector = rec->bytes_per_sector;
        boot_record.first_data_sector = rec->first_data_sector;
        boot_record.first_table_sector = rec->first_table_sector;
        boot_record.sector_count = rec->sector_count;

        debug_writeln_dec("BYTES/SECTOR                ", boot_record.bytes_per_sector);
        debug_writeln_dec("SECTOR COUNT                ", boot_record.sector_count);
        debug_writeln_dec("TABLE START                 ", boot_record.first_table_sector);
        debug_writeln_dec("DATA START                  ", boot_record.first_data_sector);
    }
    
    void NapalmFileSystem::Wipe()
    {
        debug_write("Wiping disk");
        uint32_t dot = 0;
        for (size_t i = 0; i < 1024; i++)
        {
            sec_clear();
            sec_write(i);
            
            dot++;
            if (dot >= 1024 / 16)
            {
                debug_write(".");
                dot = 0;
            }
        }
        debug_write("\n");
    }

    bool NapalmFileSystem::Format(bool wipe)
    {
        // wipe drive if requested
        if (wipe) { Wipe(); }

        // write boot record
        debug_throw_message(MSG_TYPE_OK, "Writing new boot record");
        WriteBootRecord(512, 65536);

        // write allocation table info
        debug_throw_message(MSG_TYPE_OK, "Writing allocation table info");
        WriteTableInfo();

        // write root directory
        debug_throw_message(MSG_TYPE_OK, "Creating root directory");
        WriteRoot();

        // success message
        debug_throw_message(MSG_TYPE_OK, "Successfully formatted disk");
        return true;
    }

    void NapalmFileSystem::WriteBootRecord(uint32_t sector_size, uint32_t sector_count)
    {
        // set properties
        boot_record.bytes_per_sector   = sector_size;
        boot_record.sector_count       = sector_count;
        boot_record.first_table_sector = NFS_SECTOR_TABLE_START;
        boot_record.first_data_sector  = NFS_SECTOR_TABLE_START + ((sizeof(nfs_directory_t) * 4096) / 512);
        for (size_t i = 0; i < 498; i++) { boot_record.reserved[0] = 0x00; }

        // clear buffer
        sec_clear();

        // write data to buffer
        nfs_boot_record_t* rec  = (nfs_boot_record_t*)sec_buff;
        rec->bytes_per_sector   = boot_record.bytes_per_sector;
        rec->sector_count       = boot_record.sector_count;
        rec->first_table_sector = boot_record.first_table_sector;
        rec->first_data_sector  = boot_record.first_data_sector;
        mem_copy(boot_record.reserved, rec->reserved, 498);

        // write buffer to disk
        sec_write(NFS_SECTOR_BOOT_RECORD);
    }

    void NapalmFileSystem::WriteRoot()
    {
        // set name
        mem_fill((uint8_t*)root_dir.name, 0, 32);
        strcat(root_dir.name, "NAPALM");
        // set parent to self
        root_dir.parent_index = NFS_ROOT_INDEX;
        // set entry type
        root_dir.type = NFS_ENTRY_DIR;
        // set root flag
        root_dir.reserved[0] = 1;

        // clear buffer
        sec_clear();

        // write data to buffer
        nfs_directory_t* root_ptr = (nfs_directory_t*)sec_buff;
        mem_copy((uint8_t*)root_dir.name, (uint8_t*)root_ptr->name, 32);
        mem_copy(root_dir.reserved, root_ptr->reserved, 27);
        root_ptr->parent_index    = root_dir.parent_index;
        root_ptr->type            = root_dir.type;

        // write buffer to disk
        sec_write(NFS_SECTOR_TABLE_START);
    }

    void NapalmFileSystem::WriteTableInfo()
    {
        // set properties
        entry_table.entry_sector_current = boot_record.first_table_sector;
        entry_table.data_sector_current  = boot_record.first_data_sector;
        entry_table.entry_sector_offset  = 64;
        entry_table.entry_count          = 1;
        mem_fill(entry_table.reserved, 0, 496);

        UpdateTableInfo();
    }

    void NapalmFileSystem::UpdateTableInfo()
    {
         sec_clear();
        nfs_table_t* table = (nfs_table_t*)sec_buff;

        // write data to buffer
        nfs_table_t* table_ptr = (nfs_table_t*)sec_buff;
        table_ptr->entry_sector_current = entry_table.entry_sector_current;
        table_ptr->data_sector_current  = entry_table.data_sector_current;
        table_ptr->entry_sector_offset  = entry_table.entry_sector_offset;
        table_ptr->entry_count          = entry_table.entry_count;
        mem_copy(entry_table.reserved, table_ptr->reserved, 496);

        // write buffer to disk
        sec_write(NFS_SECTOR_TABLE_INFO);
    }
    
    bool NapalmFileSystem::WriteFileEntry(nfs_file_t file)
    {
        // error checks
        if (strlen(file.name) == 0) { debug_throw_message(MSG_TYPE_ERROR, "File name cannot be null"); return false; }
        if (strlen(file.name) > 32) { debug_throw_message(MSG_TYPE_ERROR, "File name is too long(max 32 characters)"); return false; }
        if (file.type != NFS_ENTRY_FILE) { debug_throw_message(MSG_TYPE_ERROR, "Invalid entry type for file"); return false; }

        // read from disk into buffer
        sec_read(entry_table.entry_sector_current);

        // get entry in table
        nfs_file_t* entry = (nfs_file_t*)((uint32_t)sec_buff + entry_table.entry_sector_offset);
        strcpy(file.name, entry->name);
        entry->parent_index = file.parent_index;
        entry->type = NFS_ENTRY_FILE;
        entry->size = file.size;
        entry->sector_start = entry_table.data_sector_current;
        entry->sector_count = file.size / boot_record.bytes_per_sector;
        if (entry->sector_count < 1) { entry->sector_count = 1; }
        mem_fill(entry->reserved, 0, 11);

        // write buffer to disk
        sec_write(entry_table.entry_sector_current);

        // increment and return
        entry_table.entry_sector_offset += sizeof(nfs_file_t);
        entry_table.data_sector_current += entry->sector_count;
        if (entry_table.entry_sector_offset > 512 - sizeof(nfs_file_t)) { entry_table.entry_sector_current++; entry_table.entry_sector_offset = 0; }
        entry_table.entry_count++;
        UpdateTableInfo();
        return true;
    }

    bool NapalmFileSystem::WriteDirEntry(nfs_directory_t dir)
    {
        // error checks
        if (strlen(dir.name) == 0) { debug_throw_message(MSG_TYPE_ERROR, "Directory name cannot be null"); return false; }
        if (strlen(dir.name) > 32) { debug_throw_message(MSG_TYPE_ERROR, "Directory name is too long(max 32 characters)"); return false; }
        if (dir.type != NFS_ENTRY_DIR) { debug_throw_message(MSG_TYPE_ERROR, "Invalid entry type for directory"); return false; }

        // read from disk into buffer
        sec_read(entry_table.entry_sector_current);

        // get entry in table
        nfs_directory_t* entry = (nfs_directory_t*)((uint32_t)sec_buff + entry_table.entry_sector_offset);
        strcpy(dir.name, entry->name);
        entry->parent_index = dir.parent_index;
        entry->type = NFS_ENTRY_DIR;
        mem_fill(entry->reserved, 0, 27);

        // write buffer to disk
        sec_write(entry_table.entry_sector_current);

        // increment and return
        entry_table.entry_sector_offset += sizeof(nfs_directory_t);
        if (entry_table.entry_sector_offset > 512 - sizeof(nfs_directory_t)) { entry_table.data_sector_current++; entry_table.entry_sector_offset = 0; }
        entry_table.entry_count++;
        UpdateTableInfo();
        return true;
    }

    bool NapalmFileSystem::FileExists(char* path)
    {

    }

    bool NapalmFileSystem::DirectoryExists(char* path)
    {
        
    }

    int32_t NapalmFileSystem::GetFileIndex(nfs_file_t file)
    {
        int32_t index = 0;

        // loop through sectors
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // load sector from disk
            sec_read(sector);

            // loop through data in sector
            for (size_t j = 0; j < boot_record.bytes_per_sector; j += sizeof(nfs_file_t))
            {
                nfs_file_t* temp = (nfs_file_t*)((uint32_t)sec_buff + j);

                // check for match
                if (streql(file.name, temp->name) && file.parent_index == temp->parent_index && file.type == NFS_ENTRY_FILE) 
                { return index; }

                // increment index
                index++;
            }
        }
        return -1;
    }

    int32_t NapalmFileSystem::GetDirectoryIndex(nfs_directory_t dir)
    {
        int32_t index = 0;

        // loop through sectors
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // load sector from disk
            sec_read(sector);

            // loop through data in sector
            for (size_t j = 0; j < boot_record.bytes_per_sector; j += sizeof(nfs_directory_t))
            {
                nfs_directory_t* temp = (nfs_directory_t*)((uint32_t)sec_buff + j);

                // check for match
                if (streql(dir.name, temp->name) && dir.parent_index == temp->parent_index && dir.type == NFS_ENTRY_DIR && dir.reserved[0] == temp->reserved[0]) 
                { return index; }

                // increment index
                index++;
            }
        }
        return -1;
    }

    nfs_directory_t NapalmFileSystem::GetParentFromPath(char* path)
    {
        uint32_t count = 0;
        nfs_directory_t output_dir = null_dir;
        char** items = str_lsplit(path, '/', &count);

        for (size_t i = 0; i < count; i++)
        {
            char* item = items[i];
            
            // loop through table sectors
            for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
            {
                // load sector from disk
                sec_read(sector);

                // loop through data in sector
                for (size_t j = 0; j < boot_record.bytes_per_sector; j += sizeof(nfs_directory_t))
                {
                    // get directory at index
                    nfs_directory_t* temp = (nfs_directory_t*)((uint32_t)sec_buff + j);

                    // found match
                    if (streql(temp->name, item) && temp->type == NFS_ENTRY_DIR)
                    {
                        strcpy(temp->name, output_dir.name);
                        output_dir.type = temp->type;
                        output_dir.parent_index = temp->parent_index;
                        output_dir.reserved[0] = temp->reserved[0];
                    }
                }
            }

            // free path chunk
            mem_free(items[i]);
        }

        mem_free(items);

        if (output_dir.type == NFS_ENTRY_NULL)
        {
            strcpy(root_dir.name, output_dir.name);
            output_dir.type = root_dir.type;
            output_dir.parent_index = root_dir.parent_index;
            output_dir.reserved[0] = root_dir.reserved[0];
        }

        return output_dir;
    }

    nfs_file_t NapalmFileSystem::OpenFile(char* path)
    {
        nfs_file_t output_file;
        nfs_directory_t parent = GetParentFromPath(path);
        if (parent.type == NFS_ENTRY_NULL) { debug_throw_message(MSG_TYPE_ERROR, "Unable to locate parent directory while opening file"); return null_file; }

        uint32_t split_len = 0;
        char** split = str_lsplit(path, '/', &split_len);
        char*  fname = split[split_len - 1];
        bool found = false;

        // loop through table sectors
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // load sector from disk
            sec_read(sector);

            // loop through data in sector
            for (size_t j = 0; j < boot_record.bytes_per_sector; j += sizeof(nfs_file_t))
            {
                // get directory at index
                nfs_file_t* temp = (nfs_file_t*)((uint32_t)sec_buff + j);

                // found file
                if (streql(fname, temp->name) && temp->parent_index == GetDirectoryIndex(parent) && temp->type == NFS_ENTRY_FILE) 
                { 
                    strcpy(temp->name, output_file.name);
                    output_file.parent_index = temp->parent_index;
                    output_file.type = temp->type;
                    output_file.size = temp->size;
                    output_file.sector_start = temp->sector_start;
                    output_file.sector_count = temp->sector_count;
                    found = true; 
                    break; 
                    }
            }

            if (found) { break; }
        }

        if (!found) { debug_throw_message(MSG_TYPE_ERROR, "Unable to locate file for opening"); return null_file; }

        // read data into buffer
        uint8_t* dest = (uint8_t*)mem_alloc(output_file.size);
        output_file.data = (uint32_t)dest;

        // copy data to destination
        int32_t total = 0;
        for (size_t sector = output_file.sector_start; sector < output_file.sector_start + output_file.sector_count; sector++)
        {
            if (total >= output_file.size) { break; }
            sec_clear();
            sec_read(sector);

            // loop through data in sector
            for (size_t j = 0; j < boot_record.bytes_per_sector; j++)
            {
                if (total < output_file.size)
                {
                    *(uint8_t*)(dest + total) = sec_buff[j];
                    total++;
                }
            }
        }

        // free used memory
        for (size_t i = 0; i < split_len; i++) { mem_free(split[i]); }
        mem_free(split);

        // return file
        UpdateTableInfo();
        return output_file;
    }

    bool NapalmFileSystem::WriteFile(char* path, uint8_t* data, uint32_t size)
    {
        nfs_directory_t parent = GetParentFromPath(path);
        if (parent.type == NFS_ENTRY_NULL) { debug_throw_message(MSG_TYPE_ERROR, "Tried to save file at invalid directory"); return false; }
        debug_throw_message(MSG_TYPE_OK, "Located file parent directory");

        // split path string
        uint32_t chunk_count = 0;
        char** path_chunks = str_lsplit(path, '/', &chunk_count);

        // create file entry
        nfs_file_t file;
        strcpy(path_chunks[chunk_count - 1], file.name);
        file.type = NFS_ENTRY_FILE;
        file.parent_index = GetDirectoryIndex(parent);
        file.size = size;
        file.sector_start = entry_table.data_sector_current;
        file.sector_count = file.size / boot_record.bytes_per_sector;
        if (file.sector_count < 1) { file.sector_count = 1; }
        mem_fill(file.reserved, 0, 11);
        for (size_t i = 0; i < chunk_count; i++) { mem_free(path_chunks[i]); }
        mem_free(path_chunks);

        // write file entry
        WriteFileEntry(file);
        debug_throw_message(MSG_TYPE_OK, "Created file entry");

        // write data
        int32_t total = 0;
        for (size_t sector = file.sector_start; sector < file.sector_start + file.sector_count; sector++)
        {
            if (total >= size) { break; }
            sec_clear();

            for (size_t j = 0; j < boot_record.bytes_per_sector; j++)
            {
                if (total < size) { sec_buff[j] = data[total]; }
                total++;
            }
            sec_write(sector);
        }

        debug_throw_message(MSG_TYPE_OK, "Copied data to disk");
        UpdateTableInfo();
        return true;
    }
}