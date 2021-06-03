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

    void NapalmFileSystem::Mount()
    {
        sec_buff = (uint8_t*)mem_alloc(sec_size);
        sec_clear();

        ReadBootRecord();
        ReadEntryTableHeader();
        ReadRootDirectory();
    }

    void NapalmFileSystem::ReadBootRecord()
    {
        sec_read(NFS_SECTOR_BOOT_RECORD);

        // read information from sector
        nfs_boot_record_t* rec = (nfs_boot_record_t*)((uint32_t)sec_buff);
        boot_record.bytes_per_sector = rec->bytes_per_sector;
        boot_record.first_data_sector = rec->first_data_sector;
        boot_record.first_table_sector = rec->first_table_sector;
        boot_record.sector_count = rec->sector_count;
        boot_record.signature = rec->signature;
        mem_fill(boot_record.reserved, 0, 492);

        // print boot record
        debug_writeln_dec("BYTES/SECTOR                 ", boot_record.bytes_per_sector);
        debug_writeln_dec("TABLE START                  ", boot_record.first_table_sector);
        debug_writeln_dec("DATA START                   ", boot_record.first_data_sector);
        debug_writeln_dec("SECTOR COUNT                 ", boot_record.sector_count);
        debug_writeln_hex("SIGNATURE                    ", boot_record.signature);
    }
    
    void WriteBootRecord() { }

    void NapalmFileSystem::ReadEntryTableHeader()
    {
        sec_read(NFS_SECTOR_TABLE_INFO);

        // read information from sector
        nfs_table_t* t = (nfs_table_t*)((uint32_t)sec_buff);
        entry_table.current_data_sector = t->current_data_sector;
        entry_table.current_table_offset = t->current_table_offset;
        entry_table.current_table_sector = t->current_table_sector;
        entry_table.entry_count = t->entry_count;

        // print table entry header
    }

    void WriteEntryTableHeader() { }

    void NapalmFileSystem::ReadRootDirectory()
    {
        sec_read(NFS_SECTOR_TABLE_START);

        // get dir
        nfs_directory_t* root = (nfs_directory_t*)((uint32_t)sec_buff);

        // copy properties
        mem_copy((uint8_t*)root->name, (uint8_t*)root_dir.name, 32);
        root_dir.status = root->status;
        root_dir.parent_index = root->parent_index;
        root_dir.type = root->type;
        mem_copy(root->reserved, root_dir.reserved, 26);
        
    }

    nfs_file_t NapalmFileSystem::ReadFile(char* path)
    {
        // get file name
        uint32_t parts_count = 0;
        char** parts = str_lsplit(path, '/', &parts_count);
        char* name = (char*)mem_alloc(strlen(parts[parts_count - 1]));
        strcpy(parts[parts_count - 1], name);

        for (size_t i = 0; i < parts_count; i++) { mem_free(parts[i]); }
        mem_free(parts);

        //for (int i = 0; i < strlen(name); i++) { strdel(path); }
        nfs_directory_t parent = GetParentFromPath(path);

        // check if parent directory is valid
        if (parent.type == NFS_ENTRY_NULL) { debug_throw_message(MSG_TYPE_ERROR, "Unable to locate directory while reading file"); return null_file; }

        // create file entry
        nfs_file_t file;

        // loop through table sectors
        int index = 0;
        uint8_t* data = new uint8_t[512];
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // get current entries
            ata_pio_read48(sector, 1, data);

            // loop through entries in sector
            for (size_t i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_file_t))
            {
                nfs_file_t* temp = (nfs_file_t*)((uint32_t)data + i);

                if (temp->type != NFS_ENTRY_NULL)
                {
                    if (streql(temp->name, name) && temp->parent_index == GetDirectoryIndex(parent))
                    {
                        // populate file entry
                        mem_copy((uint8_t*)temp->name, (uint8_t*)file.name, 32);
                        file.type = temp->type;
                        file.parent_index = temp->parent_index;
                        file.status = temp->status;
                        file.size = temp->size;
                        file.sector_start = temp->sector_start;
                        file.sector_count = temp->sector_count;
                        break;
                    }
                }
                index++;
            }
        }

        mem_free(data);

        if (strlen(file.name) > 0 && file.type == NFS_ENTRY_FILE)
        {
            file.data = (uint32_t)mem_alloc(file.size);

            int total = 0;
            for (uint32_t sector = file.sector_start; sector < file.sector_start + file.sector_count; sector++)
            {
                // read sector into buffer
                sec_read(sector);

                // loop through entries in sector
                for (size_t i = 0; i < boot_record.bytes_per_sector; i++)
                {
                    if (total < file.size) { ((uint8_t*)file.data)[total] = sec_buff[i]; }
                    total++;
                }
            }
            debug_throw_message(MSG_TYPE_OK, "Successfully loaded file");
        }
        else { debug_throw_message(MSG_TYPE_ERROR, "Unable to locate file for opening"); return null_file; }

        // return file
        return file;
    }

    nfs_directory_t NapalmFileSystem::GetParentFromPath(char* path)
    {
        // split string
        uint32_t parts_count = 0;
        char** parts = str_lsplit(path, '/', &parts_count);

        // path is root directory
        if (parts_count == 1 && streql(parts[0], "/")) 
        { 
            for (size_t i = 0; i < parts_count; i++) { mem_free(parts[i]); }
            mem_free(parts);
            return root_dir; 
        }
        if (streql(path, "/")) 
        { 
            for (size_t i = 0; i < parts_count; i++) { mem_free(parts[i]); }
            mem_free(parts);
            return root_dir; 
        }

        // create output directory
        nfs_directory_t dir;
        uint32_t p = 0;
        bool goto_next = false;

        // loop through parts of string
        for (int32_t part = 1; part < parts_count - 1; part++)
        {
            goto_next = false;
            // loop through sectors
            for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
            {
                // loop through data in sector
                for (int i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_directory_t))
                {
                    // read sector into bufer
                    sec_read(sector);

                    // get dir
                    nfs_directory_t* temp = (nfs_directory_t*)((uint32_t)sec_buff + i);

                    // found match
                    if (temp->type == NFS_ENTRY_DIR && streql(parts[part], temp->name) && temp->parent_index == p)
                    {
                        // copy properties to output
                        mem_copy((uint8_t*)temp->name, (uint8_t*)dir.name, 32);
                        dir.parent_index = temp->parent_index;
                        dir.type = temp->type;
                        dir.status = temp->status;
                        p = GetDirectoryIndex(dir);
                        goto_next = true;
                    }

                    // invalid entry
                    if (temp->type == NFS_ENTRY_DIR && !streql(parts[part], temp->name) && temp->parent_index != p)
                    {
                        mem_fill((uint8_t*)dir.name, 0, 32);
                        dir.parent_index = 0;
                        dir.status = 0;
                        dir.type = NFS_ENTRY_NULL;
                    }

                    if (goto_next) { break; }
                }

                if (goto_next) { break; }
            }

            mem_free(parts[part]);
        }

        mem_free(parts);
        return dir;
    }

    int32_t NapalmFileSystem::GetDirectoryIndex(nfs_directory_t dir)
    {
        int index = 0;
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // loop through entries in sector
            for (size_t i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_directory_t))
            {
                // read sector into buffer
                sec_read(sector);

                nfs_directory_t* temp = (nfs_directory_t*)((uint32_t)sec_buff + i);
                if (streql(temp->name, dir.name) && temp->parent_index == dir.parent_index && temp->type == dir.type && temp->status == dir.status)
                { return index; }

                index++;
            }
        }
    }
}