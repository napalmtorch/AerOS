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

                    if (streql("NAPALM", temp->name))
                    {
                        System::KernelIO::Terminal.WriteLine("Will, you did something very stupid...");
                    }

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
        uint8_t* buff = new uint8_t[boot_record.bytes_per_sector];
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            mem_fill(buff, 0, boot_record.bytes_per_sector);

            // loop through entries in sector
            for (size_t i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_directory_t))
            {
                // read sector into buffer
                ata_pio_read48(sector, 1, buff);

                nfs_directory_t* temp = (nfs_directory_t*)((uint32_t)buff + i);
                if (streql(temp->name, dir.name) && temp->parent_index == dir.parent_index && temp->type == dir.type && temp->status == dir.status)
                { return index; }

                index++;
            }

        }
        mem_free(buff);
    }
    void NapalmFileSystem::ListDir(char* path)
    {
            auto parent = GetParentFromPath(path);
            char* name;
            char size[32];
            COL4 color;
                System::KernelIO::Terminal.Write("Directory Listing of: ",COL4_CYAN);
                System::KernelIO::Terminal.WriteLine(path,COL4_CYAN);
                System::KernelIO::Terminal.WriteLine("  .",COL4_CYAN);
                System::KernelIO::Terminal.WriteLine("  ..",COL4_CYAN);
            for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
            {
                for (int i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_directory_t))
                {
                sec_read(sector);
                nfs_file_t* file = (nfs_file_t*)((uint32_t)sec_buff + i);
                nfs_directory_t* dir = (nfs_directory_t*)((uint32_t)sec_buff + i);
                if(file->parent_index == (uint)GetDirectoryIndex(parent) || dir->parent_index == (uint)GetDirectoryIndex(parent))
                {
                    if(strlen(dir->name) == 0) { continue; }
                    if(strlen(file->name) == 0) { continue; }
                    if(streql(dir->name,"NAPALM")) { continue; } //NAPALM is the root directory name, since we dont need this and just care about the other contents we just skip it
                    if(file->type == NFS_ENTRY_FILE) { name = file->name; color = COL4_YELLOW; } else if( dir->type == NFS_ENTRY_DIR) { name = dir->name; color = COL4_CYAN;} 
                System::KernelIO::Terminal.Write("Name: ",COL4_MAGENTA);
                System::KernelIO::Terminal.Write(name,color);
                 if(file->type == NFS_ENTRY_FILE) {
                System::KernelIO::Terminal.Write("          ");
                System::KernelIO::Terminal.Write("Size: ",COL4_MAGENTA);
                strdec(file->size,size);
                System::KernelIO::Terminal.WriteLine(size,COL4_YELLOW);
                 }
                 else
                 {
                     System::KernelIO::Terminal.NewLine();
                 }
                }
            }
        }
        
    }
    bool NapalmFileSystem::FileExists(char* path)
    {
        nfs_directory_t parent = GetParentFromPath(path);
        if (parent.type != NFS_ENTRY_DIR) { return false; }

        uint32_t parts_count = 0;
        char** parts = str_lsplit(path, '/', &parts_count);
        char* name = parts[parts_count - 1];

        // locate specific file
        bool found = false;
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // loop through entries in sector
            for (size_t i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_directory_t))
            {
                // read sector into buffer
                sec_read(sector);
                nfs_file_t* file = (nfs_file_t*)((uint32_t)sec_buff + i);

                if (streql(file->name, name) && file->parent_index == GetDirectoryIndex(parent) && file->type == NFS_ENTRY_FILE)
                {
                    found = true;
                    break;
                }
            }

            if (found) { break; }
        }

        for (size_t i = 0; i < parts_count; i++) { mem_free(parts[i]); }
        mem_free(parts);
        return found;
    }

    bool NapalmFileSystem::DirectoryExists(char* path)
    {
        if (path[strlen(path) - 1] == '/') { strdel(path); }
        nfs_directory_t parent = GetParentFromPath(path);
        if (parent.type != NFS_ENTRY_DIR) { return false; }
        int32_t pindex = GetDirectoryIndex(parent);

        uint32_t parts_count = 0;
        char** parts = str_lsplit(path, '/', &parts_count);
        char* name = parts[parts_count - 1];

        // locate specific file
        bool found = false;
        for (size_t sector = boot_record.first_table_sector; sector < boot_record.first_data_sector; sector++)
        {
            // loop through entries in sector
            for (size_t i = 0; i < boot_record.bytes_per_sector; i += sizeof(nfs_directory_t))
            {
                // read sector into buffer
                sec_read(sector);

                nfs_directory_t* dir = (nfs_directory_t*)((uint32_t)sec_buff + i);

                if (streql(dir->name, name) && dir->parent_index == pindex && dir->type == NFS_ENTRY_DIR)
                {
                    found = true;
                    break;
                }
            }

            if (found) { break; }
        }

        for (size_t i = 0; i < parts_count; i++) { mem_free(parts[i]); }
        mem_free(parts);
        return found;
    }
}