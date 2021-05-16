#include <lib/types.h>
#include <hardware/fat.hpp>
#include <hardware/memory.hpp>
#include <hardware/drivers/ata_pio.hpp>
#include <core/kernel.hpp>
#include <apps/win_term.hpp>

extern "C"
{
    // private declarations
    static void read_bpb(f32 *fs, struct fat_bios_block_t *bpb);
    static uint32_t sector_for_cluster(f32 *fs, uint32_t cluster);
    static void get_sector(f32 *fs, uint8_t *buff, uint32_t sector, uint32_t count);
    static void put_sector(f32 *fs, uint8_t *buff, uint32_t sector, uint32_t count);

    // master file system base
    f32 *fat_master_fs;

    void mem_copy2(uint8_t* src, uint8_t* dest, uint32_t len)
    {
        mem_copy(dest,src,len);    
    }

    // convert lower case character to upper case
    int k_toupper(int c) { if(c >= 97 && c <= 122) { return c - 32; } return c; }

    // trim spaces from name
    static void trim_spaces(char *c, int max)
    {
        int32_t i = 0;
        while(*c != ' ' && i++ < max) { c++; }
        if(*c == ' ') { *c = 0; }
    }

    // read sector from disk
    static void get_sector(f32 *fs, uint8_t *buff, uint32_t sector, uint32_t count) { ata_pio_read48(sector, count, buff); }

    // write sector to disk
    static void put_sector(f32 *fs, uint8_t *buff, uint32_t sector, uint32_t count) 
    {
        for(size_t i = 0; i < count; i++) { ata_pio_write48(sector + i, 1, buff + (i * 512)); }
    }

    static void fat_flush(f32 *fs) 
    {
        // TODO: This is not endian-safe. Must marshal the integers into a byte buffer.
        put_sector(fs, (uint8_t *)fs->FAT, fs->fat_begin_sector, fs->bpb.count_sectors_per_FAT32);
    }

    static uint16_t readi16(uint8_t *buff, size_t offset)
    {
        uint8_t *ubuff = buff + offset;
        return ubuff[1] << 8 | ubuff[0];
    }
    static uint32_t readi32(uint8_t *buff, size_t offset) 
    {
        uint8_t *ubuff = buff + offset;
        return
            ((ubuff[3] << 24) & 0xFF000000) |
            ((ubuff[2] << 16) & 0x00FF0000) |
            ((ubuff[1] << 8) & 0x0000FF00) |
            (ubuff[0] & 0x000000FF);
    }

    static void read_bpb(f32 *fs, struct fat_bios_block_t *bpb) 
    {
        uint8_t sector0[512];
        get_sector(fs, sector0, 0, 1);

        bpb->bytes_per_sector = readi16(sector0, 11);;
        bpb->sectors_per_cluster = sector0[13];
        bpb->reserved_sectors = readi16(sector0, 14);
        bpb->FAT_count = sector0[16];
        bpb->dir_entries = readi16(sector0, 17);
        bpb->total_sectors = readi16(sector0, 19);
        bpb->media_descriptor_type = sector0[21];
        bpb->count_sectors_per_FAT12_16 = readi16(sector0, 22);
        bpb->count_sectors_per_track = readi16(sector0, 24);
        bpb->count_heads_or_sizes_on_media = readi16(sector0, 26);
        bpb->count_hidden_sectors = readi32(sector0, 28);
        bpb->large_sectors_on_media = readi32(sector0, 32);
        // EBR
        bpb->count_sectors_per_FAT32 = readi32(sector0, 36);
        bpb->flags = readi16(sector0, 40);
        bpb->FAT_version = readi16(sector0, 42);
        bpb->cluster_number_root_dir = readi32(sector0, 44);
        bpb->sector_number_FSInfo = readi16(sector0, 48);
        bpb->sector_number_backup_boot_sector = readi16(sector0, 50);
        // Skip 12 bytes
        bpb->drive_number = sector0[64];
        bpb->windows_flags = sector0[65];
        bpb->signature = sector0[66];
        bpb->volume_id = readi32(sector0, 67);
        mem_copy2((uint8_t*)&bpb->volume_label, (uint8_t*)sector0 + 71, 11); bpb->volume_label[11] = 0;
        mem_copy2((uint8_t*)&bpb->system_id, sector0 + 82, 8); bpb->system_id[8] = 0;
    }

    static uint32_t sector_for_cluster(f32 *fs, uint32_t cluster) 
    {
        return fs->cluster_begin_sector + ((cluster - 2) * fs->bpb.sectors_per_cluster);
    }

    // CLUSTER NUMBERS START AT 2
    void fat_get_cluster(f32* fs, uint8_t *buff, uint32_t cluster_number) 
    {
        if(cluster_number >= EOC) { debug_throw_panic("[FS] : Can't get cluster. Hit End Of Chain."); }
        uint32_t sector = sector_for_cluster(fs, cluster_number);
        uint32_t sector_count = fs->bpb.sectors_per_cluster;
        get_sector(fs, buff, sector, sector_count);
    }

    static void put_cluster(f32 *fs, uint8_t *buff, uint32_t cluster_number) 
    {
        uint32_t sector = sector_for_cluster(fs, cluster_number);
        uint32_t sector_count = fs->bpb.sectors_per_cluster;
        put_sector(fs, buff, sector, sector_count);
    }

    uint32_t fat_get_next_cluster_id(f32 *fs, uint32_t cluster) 
    {
        return fs->FAT[cluster] & 0x0FFFFFFF;
    }

    static char *parse_long_name(uint8_t *entries, uint8_t entry_count) 
    {
        char *name = (char*)mem_alloc(entry_count * 13);
        int i, j;
        for(i = 0; i < entry_count; i++) 
        {
            uint8_t *entry = entries + (i * 32);
            uint8_t entry_no = (uint8_t)entry[0] & 0x0F;
            char *name_offset = name + ((entry_no - 1) * 13);

            for(j = 1; j < 10; j+=2) 
            {
                if(entry[j] >= 32 && entry[j] <= 127) { *name_offset = entry[j]; }
                else { *name_offset = 0; }
                name_offset++;
            }
            for(j = 14; j < 25; j+=2) 
            {
                if(entry[j] >= 32 && entry[j] <= 127) { *name_offset = entry[j]; }
                else { *name_offset = 0; }
                name_offset++;
            }
            for(j = 28; j < 31; j+=2) 
            {
                if(entry[j] >= 32 && entry[j] <= 127) { *name_offset = entry[j]; }
                else { *name_offset = 0; }
                name_offset++;
            }
        }
        return name;
    }

    static void clear_cluster(f32 *fs, uint32_t cluster) 
    {
        uint8_t buffer[fs->cluster_size];
        mem_fill(buffer, 0, fs->cluster_size);
        put_cluster(fs, buffer, cluster);
    }

    static uint32_t allocateCluster(f32 *fs) 
    {
        uint32_t i, ints_per_fat = (512 * fs->bpb.count_sectors_per_FAT32) / 4;
        for(i = fs->cluster_alloc_hint; i < ints_per_fat; i++) 
        {
            if(fs->FAT[i] == 0) 
            {
                fs->FAT[i] = 0x0FFFFFFF;
                clear_cluster(fs, i);
                fs->cluster_alloc_hint = i+1;
                return i;
            }
        }
        for(i = 0; i < fs->cluster_alloc_hint; i++) 
        {
            if(fs->FAT[i] == 0) 
            {
                fs->FAT[i] = 0x0FFFFFFF;
                clear_cluster(fs, i);
                fs->cluster_alloc_hint = i+1;
                return i;
            }
        }
        return 0;
    }

    // Creates a checksum for an 8.3 filename
    // must be in directory-entry format, i.e.
    // fat32.c -> "FAT32   C  "
    static uint8_t checksum_fname(char *fname) 
    {
        uint32_t i;
        uint8_t checksum = 0;
        for(i = 0; i < 11; i++)
         {
            uint8_t highbit = (checksum & 0x1) << 7;
            checksum = ((checksum >> 1) & 0x7F) | highbit;
            checksum = checksum + fname[i];
        }
        return checksum;
    }

    static void write_8_3_filename(char *fname, uint8_t *buffer) 
    {
        mem_fill(buffer, ' ', 11);
        uint32_t namelen = strlen(fname);
        // find the extension
        int i;
        int dot_index = -1;
        for(i = namelen-1; i >= 0; i--) 
        {
            if(fname[i] == '.') { dot_index = i; break; }
        }

        // Write the extension
        if(dot_index >= 0) 
        {
            for(i = 0; i < 3; i++) 
            {
                uint32_t c_index = dot_index + 1 + i;
                uint8_t c = c_index >= namelen ? ' ' : k_toupper(fname[c_index]);
                buffer[8 + i] = c;
            }
        }
        else { for(i = 0; i < 3; i++) { buffer[8 + i] = ' '; }
        }

        // Write the filename.
        uint32_t firstpart_len = namelen;
        if(dot_index >= 0) { firstpart_len = dot_index; }
        if(firstpart_len > 8) {

            // Write the weird tilde thing.
            for(i = 0; i < 6; i++) { buffer[i] = k_toupper(fname[i]); }
            buffer[6] = '~';
            buffer[7] = '1'; // probably need to enumerate like files and increment.
        }
        else 
        {
            // Just write the file name.
            for(size_t j = 0; j < firstpart_len; j++) { buffer[j] = k_toupper(fname[j]); }
        }
    }

    static uint8_t *locate_entries(f32 *fs, uint8_t *cluster_buffer, struct directory *dir, uint32_t count, uint32_t *found_cluster) 
    {
        uint32_t dirs_per_cluster = fs->cluster_size / 32;

        uint32_t i;
        int64_t index = -1;
        uint32_t cluster = dir->cluster;
        while(1) 
        {
            fat_get_cluster(fs, cluster_buffer, cluster);

            uint32_t in_a_row = 0;
            for(i = 0; i < dirs_per_cluster; i++) 
            {
                uint8_t *entry = cluster_buffer + (i * 32);
                uint8_t first_byte = entry[0];
                if(first_byte == 0x00 || first_byte == 0xE5) { in_a_row++; }
                else { in_a_row = 0; }

                if(in_a_row == count) { index = i - (in_a_row - 1); break; }
            }

            if(index >= 0) { break; }

            uint32_t next_cluster = fs->FAT[cluster];
            if(next_cluster >= EOC) 
            {
                next_cluster = allocateCluster(fs);
                if(!next_cluster) { return 0; }
                fs->FAT[cluster] = next_cluster;
            }
            cluster = next_cluster;
        }

        *found_cluster = cluster;
        return cluster_buffer + (index * 32);
    }

    static void write_long_filename_entries(uint8_t *start, uint32_t num_entries, char *fname) 
    {
        // Create a short filename to use for the checksum.
        char shortfname[12];
        shortfname[11] = 0;
        write_8_3_filename(fname, (uint8_t *)shortfname);
        uint8_t checksum = checksum_fname(shortfname);

        /* Write the long-filename entries */
        // tracks the number of characters we've written into
        // the long-filename entries.
        uint32_t writtenchars = 0;
        char *nameptr = fname;
        uint32_t namelen = strlen(fname);
        uint8_t *entry = NULL;
        uint32_t i;
        for(i = 0; i < num_entries; i++)
         {
            // reverse the entry order
            entry = start + ((num_entries - 1 - i) * 32);
            // Set the entry number
            entry[0] = i+1;
            entry[13] = checksum;

            // Characters are 16 bytes in long-filename entries (j+=2)
            // And they only go in certain areas in the 32-byte
            // block. (why we have three loops)
            uint32_t j;
            for(j = 1; j < 10; j+=2) 
            {
                if(writtenchars < namelen) { entry[j] = *nameptr; }
                else { entry[j] = 0; }
                nameptr++;
                writtenchars++;
            }
            for(j = 14; j < 25; j+=2) 
            {
                if(writtenchars < namelen) { entry[j] = *nameptr; }
                else { entry[j] = 0; }
                nameptr++;
                writtenchars++;
            }
            for(j = 28; j < 31; j+=2) 
            {
                if(writtenchars < namelen) { entry[j] = *nameptr; }
                else { entry[j] = 0; }
                nameptr++;
                writtenchars++;
            }
            // Mark the attributes byte as LFN (Long File Name)
            entry[11] = LFN;
        }
        // Mark the last(first) entry with the end-of-long-filename bit
        entry[0] |= 0x40;
    }

    f32 *fs_create(char *fatSystem) 
    {
        f32 *fs = (f32*)mem_alloc(sizeof (struct f32));
        if(!identify()) { return NULL; }

        debug_writeln("Filesystem identified!\n");
        read_bpb(fs, &fs->bpb);

        trim_spaces(fs->bpb.system_id, 8);
        if(strcmp(fs->bpb.system_id, "FAT32") != 0) 
        {
            debug_write("Found: ");
            debug_writeln(fs->bpb.system_id);
            mem_free(fs);
            return NULL;
        }

        debug_writeln_dec("Sectors per cluster: ", fs->bpb.sectors_per_cluster);

        fs->partition_begin_sector = 0;
        fs->fat_begin_sector = fs->partition_begin_sector + fs->bpb.reserved_sectors;
        fs->cluster_begin_sector = fs->fat_begin_sector + (fs->bpb.FAT_count * fs->bpb.count_sectors_per_FAT32);
        fs->cluster_size = 512 * fs->bpb.sectors_per_cluster;
        fs->cluster_alloc_hint = 0;

        // Load the FAT
        uint32_t bytes_per_fat = 512 * fs->bpb.count_sectors_per_FAT32;
        fs->FAT = (uint32_t*)mem_alloc(bytes_per_fat);
        uint32_t sector_i;
        for(sector_i = 0; sector_i < fs->bpb.count_sectors_per_FAT32; sector_i++) 
        {
            uint8_t sector[512];
            get_sector(fs, sector, fs->fat_begin_sector + sector_i, 1);
            uint32_t integer_j;
            for(integer_j = 0; integer_j < 512/4; integer_j++) 
            {
                fs->FAT[sector_i * (512 / 4) + integer_j] = readi32(sector, integer_j * 4);
            }
        }
        return fs;
    }

    void fs_destroy(f32 *fs) 
    {
        debug_writeln("Destroying filesystem.\n");
        fat_flush(fs);
        mem_free(fs->FAT);
        mem_free(fs);
    }

    const struct fat_bios_block_t *fat_get_bpb(f32 *fs) { return &fs->bpb; }

    void fat_populate_root_dir(f32 *fs, struct directory *dir) { fat_populate_dir(fs, dir, 2); }

    // Populates dirent with the directory entry starting at start
    // Returns a pointer to the next 32-byte chunk after the entry
    // or NULL if either start does not point to a valid entry, or
    // there are not enough entries to build a struct dir_entry_t
    static uint8_t *read_dir_entry(f32 *fs, uint8_t *start, uint8_t *end, struct dir_entry_t *dirent) 
    {
        uint8_t first_byte = start[0];
        uint8_t *entry = start;
        if(first_byte == 0x00 || first_byte == 0xE5) { return NULL; }

        uint32_t LFNCount = 0;
        while(entry[11] == LFN) 
        {
            LFNCount++;
            entry += 32;
            if(entry == end) { return NULL; }
        }
        if(LFNCount > 0) { dirent->name = parse_long_name(start, LFNCount); }
        else {
            // There's no long file name.
            // Trim up the short filename.
            dirent->name = (char*)mem_alloc(13);
            mem_copy2((uint8_t*)dirent->name, entry, 11);
            dirent->name[11] = 0;
            char extension[4];
            mem_copy2((uint8_t*)extension, (uint8_t*)dirent->name + 8, 3);
            extension[3] = 0;
            trim_spaces(extension, 3);

            dirent->name[8] = 0;
            trim_spaces(dirent->name, 8);

            if(strlen(extension) > 0) 
            {
                uint32_t len = strlen(dirent->name);
                dirent->name[len++] = '.';
                mem_copy2((uint8_t*)dirent->name + len, (uint8_t*)extension, 4);
            }
        }

        dirent->dir_attrs = entry[11];;
        uint16_t first_cluster_high = readi16(entry, 20);
        uint16_t first_cluster_low = readi16(entry, 26);
        dirent->first_cluster = first_cluster_high << 16 | first_cluster_low;
        dirent->file_size = readi32(entry, 28);
        return entry + 32;
    }

    // This is a complicated one. It parses a directory entry into the dir_entry_t pointed to by target_dirent.
    // root_cluster  must point to a buffer big enough for two clusters.
    // entry         points to the entry the caller wants to parse, and must point to a spot within root_cluster.
    // nextentry     will be modified to hold the next spot within root_entry to begin looking for entries.
    // cluster       is the cluster number of the cluster loaded into root_cluster.
    // secondcluster will be modified IF this function needs to load another cluster to continue parsing
    //               the entry, in which case, it will be set to the value of the cluster loaded.
    //
    void next_dir_entry(f32 *fs, uint8_t *root_cluster, uint8_t *entry, uint8_t **nextentry, struct dir_entry_t *target_dirent, uint32_t cluster, uint32_t *secondcluster) 
    {

        uint8_t *end_of_cluster = root_cluster + fs->cluster_size;
        *nextentry = read_dir_entry(fs, entry, end_of_cluster, target_dirent);
        if(!*nextentry) 
        {
            // Something went wrong!
            // Either the directory entry spans the bounds of a cluster,
            // or the directory entry is invalid.
            // Load the next cluster and retry.

            // Figure out how much of the last cluster to "replay"
            uint32_t bytes_from_prev_chunk = end_of_cluster - entry;

            *secondcluster = fat_get_next_cluster_id(fs, cluster);
            if(*secondcluster >= EOC) 
            {
                // There's not another directory cluster to load
                // and the previous entry was invalid!
                // It's possible the filesystem is corrupt or... you know...
                // my software could have bugs.
                debug_throw_panic("FOUND BAD DIRECTORY ENTRY!");
            }
            // Load the cluster after the previous saved entries.
            fat_get_cluster(fs, root_cluster + fs->cluster_size, *secondcluster);
            // Set entry to its new location at the beginning of root_cluster.
            entry = root_cluster + fs->cluster_size - bytes_from_prev_chunk;

            // Retry reading the entry.
            *nextentry = read_dir_entry(fs, entry, end_of_cluster + fs->cluster_size, target_dirent);
            if(!*nextentry) 
            {
                // Still can't parse the directory entry.
                // Something is very wrong.
                debug_throw_panic("FAILED TO READ DIRECTORY ENTRY! THE SOFTWARE IS BUGGY!\n");
            }
        }
    }

    // TODO: Refactor this. It is so similar to fat_delete_file that it would be nice
    // to combine the similar elements.
    // WARN: If you fix a bug in this function, it's likely you will find the same
    // bug in fat_delete_file.
    void fat_populate_dir(f32 *fs, struct directory *dir, uint32_t cluster) 
    {
        dir->cluster = cluster;
        uint32_t dirs_per_cluster = fs->cluster_size / 32;
        uint32_t max_dirs = 0;
        dir->entries = 0;
        uint32_t entry_count = 0;

        while(1) 
        {
            max_dirs += dirs_per_cluster;
            dir->entries = (dir_entry_t*)(max_dirs * sizeof (struct dir_entry_t));
            // Double the size in case we need to read a directory entry that
            // spans the bounds of a cluster.
            uint8_t root_cluster[fs->cluster_size * 2];
            fat_get_cluster(fs, root_cluster, cluster);

            uint8_t *entry = root_cluster;
            while((uint32_t)(entry - root_cluster) < fs->cluster_size) 
            {
                uint8_t first_byte = *entry;
                if(first_byte == 0x00 || first_byte == 0xE5) 
                {
                    // This directory entry has never been written
                    // or it has been deleted.
                    entry += 32;
                    continue;
                }

                uint32_t secondcluster = 0;
                uint8_t *nextentry = NULL;
                struct dir_entry_t *target_dirent = dir->entries + entry_count;
                next_dir_entry(fs, root_cluster, entry, &nextentry, target_dirent, cluster, &secondcluster);
                entry = nextentry;
                if(secondcluster) { cluster = secondcluster; }

                entry_count++;
            }

            cluster = fat_get_next_cluster_id(fs, cluster);
            if(cluster >= EOC) { break; }
        }
        dir->num_entries = entry_count;
    }

    static void zero_FAT_chain(f32 *fs, uint32_t start_cluster) 
    {
        uint32_t cluster = start_cluster;
        while(cluster < EOC && cluster != 0) 
        {
            uint32_t next_cluster = fs->FAT[cluster];
            fs->FAT[cluster] = 0;
            cluster = next_cluster;
        }
        fat_flush(fs);
    }

    // TODO: Refactor this. It is so similar to fat_populate_dir that it would be nice
    // to combine the similar elements.
    // WARN: If you fix a bug in this function, it's likely you will find the same
    // bug in fat_populate_dir.
    void fat_delete_file(f32 *fs, struct directory *dir, char *filename) 
    {
        uint32_t cluster = dir->cluster;

        // Double the size in case we need to read a directory entry that
        // spans the bounds of a cluster.
        uint8_t root_cluster[fs->cluster_size * 2];
        struct dir_entry_t target_dirent;

        // Try to locate and invalidate the directory entries corresponding to the
        // filename in dirent.
        while(1) {
            fat_get_cluster(fs, root_cluster, cluster);

            uint8_t *entry = root_cluster;
            while((uint32_t)(entry - root_cluster) < fs->cluster_size) 
            {
                uint8_t first_byte = *entry;
                if(first_byte == 0x00 || first_byte == 0xE5) 
                {
                    // This directory entry has never been written
                    // or it has been deleted.
                    entry += 32;
                    continue;
                }

                uint32_t secondcluster = 0;
                uint8_t *nextentry = NULL;
                next_dir_entry(fs, root_cluster, entry, &nextentry, &target_dirent, cluster, &secondcluster);

                // We have a target dirent! see if it's the one we want!
                if(strcmp(target_dirent.name, filename) == 0) 
                {
                    // We found it! Invalidate all the entries.
                    mem_fill(entry, 0, nextentry - entry);
                    put_cluster(fs, root_cluster, cluster);
                    if(secondcluster) { put_cluster(fs, root_cluster + fs->cluster_size, secondcluster); }
                    zero_FAT_chain(fs, target_dirent.first_cluster);
                    mem_free(target_dirent.name);
                    return;
                }
                else {
                    // We didn't find it. Continue.
                    entry = nextentry;
                    if(secondcluster) { cluster = secondcluster; }
                }
                mem_free(target_dirent.name);

            }
            cluster = fat_get_next_cluster_id(fs, cluster);
            if(cluster >= EOC) return;
        }
    }

    void fat_free_dir(f32 *fs, struct directory *dir)
    {
        uint32_t i;
        for(i = 0; i < dir->num_entries; i++) {
            mem_free(dir->entries[i].name);
        }
        mem_free(dir->entries);
    }

    uint8_t *fat_read_file(f32 *fs, struct dir_entry_t *dirent) 
    {
        uint8_t *file = (uint8_t*)mem_alloc(dirent->file_size);
        uint8_t *filecurrptr = file;
        uint32_t cluster = dirent->first_cluster;
        uint32_t copiedbytes = 0;
        while(1) 
        {
            uint8_t cbytes[fs->cluster_size];
            fat_get_cluster(fs, cbytes, cluster);

            uint32_t remaining = dirent->file_size - copiedbytes;
            uint32_t to_copy = remaining > fs->cluster_size ? fs->cluster_size : remaining;

            mem_copy2(filecurrptr, cbytes, to_copy);

            filecurrptr += fs->cluster_size;
            copiedbytes += to_copy;

            cluster = fat_get_next_cluster_id(fs, cluster);
            if(cluster >= EOC) { break; }
        }
        return file;
    }

    static void fat_write_file_impl(f32 *fs, struct directory *dir, uint8_t *file, char *fname, uint32_t flen, uint8_t attrs, uint32_t setcluster) 
    {
        uint32_t required_clusters = flen / fs->cluster_size;
        if(flen % fs->cluster_size != 0) required_clusters++;
        if(required_clusters == 0) required_clusters++; // Allocate at least one cluster.
        // One for the traditional 8.3 name, one for each 13 charaters in the extended name.
        // Int division truncates, so if there's a remainder from length / 13, add another entry.
        uint32_t required_entries_long_fname = (strlen(fname) / 13);
        if(strlen(fname) % 13 > 0) { required_entries_long_fname++; }

        uint32_t required_entries_total = required_entries_long_fname + 1;

        uint32_t cluster; // The cluster number that the entries are found in
        uint8_t root_cluster[fs->cluster_size];
        uint8_t *start_entries = locate_entries(fs, root_cluster, dir, required_entries_total, &cluster);
        write_long_filename_entries(start_entries, required_entries_long_fname, fname);

        // Write the actual file entry;
        uint8_t *actual_entry = start_entries + (required_entries_long_fname * 32);
        write_8_3_filename(fname, actual_entry);

        // Actually write the file!
        uint32_t writtenbytes = 0;
        uint32_t prevcluster = 0;
        uint32_t firstcluster = 0;
        uint32_t i;
        if(setcluster) 
        {
            // Caller knows where the first cluster is.
            // Don't allocate or write anything.
            firstcluster = setcluster;
        }
        else 
        {
            for(i = 0; i < required_clusters; i++) 
            {
                uint32_t currcluster = allocateCluster(fs);
                if(!firstcluster) { firstcluster = currcluster; }

                uint8_t cluster_buffer[fs->cluster_size];
                mem_fill(cluster_buffer, 0, fs->cluster_size);
                uint32_t bytes_to_write = flen - writtenbytes;
                if(bytes_to_write > fs->cluster_size) { bytes_to_write = fs->cluster_size; }

                mem_copy2(cluster_buffer, file + writtenbytes, bytes_to_write);
                writtenbytes += bytes_to_write;
                put_cluster(fs, cluster_buffer, currcluster);
                if(prevcluster) { fs->FAT[prevcluster] = currcluster; }

                prevcluster = currcluster;
            }
        }

        // Write the other fields of the actual entry
        // We do it down here because we need the first cluster
        // number.

        // attrs
        actual_entry[11] = attrs;

        // high cluster bits
        actual_entry[20] = (firstcluster >> 16) & 0xFF;
        actual_entry[21] = (firstcluster >> 24) & 0xFF;

        // low cluster bits
        actual_entry[26] = (firstcluster) & 0xFF;
        actual_entry[27] = (firstcluster >> 8) & 0xFF;

        // file size
        actual_entry[28] = flen & 0xFF;
        actual_entry[29] = (flen >> 8) & 0xFF;
        actual_entry[30] = (flen >> 16) & 0xFF;
        actual_entry[31] = (flen >> 24) & 0xFF;

        // Write the cluster back to disk
        put_cluster(fs, root_cluster, cluster);
        fat_flush(fs);
    }

    void fat_write_file(f32 *fs, struct directory *dir, uint8_t *file, char *fname, uint32_t flen) { fat_write_file_impl(fs, dir, file, fname, flen, 0, 0); }

    static void fat_create_dir_subdirs(f32 *fs, struct directory *dir, uint32_t parentcluster) 
    {
        fat_write_file_impl(fs, dir, NULL, ".", 0, DIRECTORY, dir->cluster);
        fat_write_file_impl(fs, dir, NULL, "..", 0, DIRECTORY, parentcluster);
    }

    void fat_create_dir(f32 *fs, struct directory *dir, char *dirname) 
    {
        uint32_t i;
        uint32_t max_name;
        for(i = 0; i < dir->num_entries; i++) 
        {
            if(!streql(dir->entries[i].name,dirname))
            {
        
                fat_write_file_impl(fs, dir, NULL, dirname, 0, DIRECTORY, 0);

                // We need to add the subdirectories '.' and '..'
                struct directory subdir;
                fat_populate_dir(fs, &subdir, dir->cluster);
                uint32_t i;
                for(i = 0; i < subdir.num_entries; i++) 
                {
                    if(strcmp(subdir.entries[i].name, dirname) == 0) 
                    {
                        struct directory newsubdir;
                        fat_populate_dir(fs, &newsubdir, subdir.entries[i].first_cluster);
                        fat_create_dir_subdirs(fs, &newsubdir, subdir.cluster);
                        fat_free_dir(fs, &newsubdir);
                    }
                }
                fat_free_dir(fs, &subdir);
            }
            else { term_writeln_ext("File or directory already exists",COL4_RED); }
        }
    }
        char *strdup(const char *s) 
    {
        char *news = (char*)mem_alloc(strlen_c(s) + 1);
        char *temp = news;
        while(*temp++ = *s++);
        return news;
    }

    const char *strchr_c(const char *s, int c) 
    {
        while(*s) {
            if(*s == c) return s;
            s++;
        }
        return NULL;
    }

    char *strchr(char *s, int c) 
    {
        while(*s) {
            if(*s == c) return s;
            s++;
        }
        return NULL;
    }

    char *strtok_r(char *str, const char *delim, char **saveptr) 
    {
        char *begin;
        if(str) { begin = str; }
        else if (*saveptr) { begin = *saveptr; }
        else { return NULL; }

        while(strchr_c(delim, begin[0])) { begin++; }

        char *next = NULL;
        for(int i = 0; i < strlen_c(delim); i++) 
        {
            char *temp = strchr(begin, delim[i]);
            if(temp < next || next == NULL) { next = temp; }
        }

        if(!next) 
        {
            *saveptr = NULL;
            return begin;
        }
        *next = 0;
        *saveptr=next+1;
        return begin;
    }
    
    static inline int entry_for_path(const char *path, struct dir_entry_t *entry) 
    {
        struct directory dir;
        fat_populate_root_dir(fat_master_fs, &dir);
        int found_file = 0;
        if(path[0] != '/') { return found_file; }

        char *cutpath = strdup(path);
        char *tokstate = NULL;
        char *next_dir = strtok_r(cutpath, "/", &tokstate);
        struct dir_entry_t *currentry = NULL;
        entry->name = NULL;

        while (next_dir) 
        {
            int found = 0;
            for(int entryi = 0; entryi < dir.num_entries; entryi++) 
            {
                currentry = &dir.entries[entryi];
                if(strcmp(currentry->name, next_dir) == 0) 
                {
                    if(entry->name) mem_free(entry->name);
                    *entry = *currentry;
                    // TODO: Make sure this doesn't leak. Very dangerous:
                    entry->name = strdup(currentry->name);

                    uint32_t cluster = currentry->first_cluster;
                    fat_free_dir(fat_master_fs, &dir);
                    fat_populate_dir(fat_master_fs, &dir, cluster);
                    found = 1;
                    break;

                }
            }
            if(!found) 
            {
                mem_free(cutpath);
                fat_free_dir(fat_master_fs, &dir);
                return 0;
            }
            next_dir = strtok_r(NULL, "/", &tokstate);
        }
        fat_free_dir(fat_master_fs, &dir);
        mem_free(cutpath);
        return 1;
    }

    bool fat_file_exists(char* name)
    {
        struct dir_entry_t entry;
        if(entry_for_path(name, &entry))
        {
            return true;
        }
        return false; 
    } 
    char *replace(const char *s, char ch, const char *repl) {
    int count = 0;
    const char *t;
    for(t=s; *t; t++)
        count += (*t == ch);

    size_t rlen = strlen_c(repl);
    char *res = (char*)mem_alloc(strlen_c(s) + (rlen-1)*count + 1);
    char *ptr = res;
    for(t=s; *t; t++) {
        if(*t == ch) {
            mem_copy((uint8_t*)repl,(uint8_t*)ptr, rlen);
            ptr += rlen;
        } else {
            *ptr++ = *t;
        }
    }
    *ptr = 0;
    return res;
}

    char* fat_get_sub_folder(char* str) //char* str contains the full path
    {
            uint32_t arraylen = 0;
            char** split = strsplit(str, '/', &arraylen);
            int size = 0; 
            char* path;
            debug_writeln(str);
            //prepare array
            for(int i = 0; i < 256; i++) ///sizeof cant handle many nested folders, lets hope this gets never reached....
            {
                if(split[i] == nullptr) { continue; }
                if(IsEmpty(split[i])) { continue; }
                if(i == 0) { continue; }
                size +=1;

            }
            //check if we hit root
            if(size-1 == 0 || size-1 < 0)
                {
                    path = "/";
                    return path;
                }
                else
                {
                    path = split[size-1];
                }
            //lets build our new path
            char dirpath[256]{'\0'};
            stradd(dirpath,'/');
            for(int a=0; a<size; a++)
            {
                if(split[a] == nullptr) { continue; }
                if(IsEmpty(split[a])) { continue; }
                if(a == 0) { continue; }
                stradd(dirpath,'/');
                strcat(dirpath,split[a]);
                continue;
            }
            //lets check if the pathbuilder added an additional slash
            //if yes we remove it
            char* final_string;
            if(dirpath[0] == '/' && dirpath[1]=='/')
            {
                final_string = dirpath+1;
            }
            //finally, check if we get a FS entry for our path,
            //if yes we return this, otherwise we return the base string
            struct dir_entry_t compare;
            if(entry_for_path(final_string,&compare))
            {
                  struct directory * dir;
                  fat_free_dir(fat_master_fs,dir);
                  populate_dir_by_name(fat_master_fs,dir,final_string);  
                
                    return fat_change_dir_absolute(final_string);
            }
            else
            {
                return str;
            }
        }
    char* fat_change_dir(char* dir)
    {
        struct dir_entry_t entry;
        if(entry_for_path(dir, &entry))
        {
            return entry.name;
        }
        return dir;
    }
    char* fat_change_in_current_folder(char* str)
    {
        struct dir_entry_t entry;
        if(entry_for_path(str,&entry))
        {
                    struct directory * dir;
                  fat_free_dir(fat_master_fs,dir);
                  populate_dir_by_name(fat_master_fs,dir,str);
            return str;
        }
        else
        {
            return System::KernelIO::Shell.GetCurrentPath();
        }
    }
    char* fat_change_dir_absolute(char* dir)
    {
        struct dir_entry_t entry;
        if(entry_for_path(dir, &entry))
        {
            return dir;
        }
        return "/";
    }
    void populate_dir_by_name(f32 *fs,struct directory *dir,char* name)
    {
        uint32_t i;
        uint32_t max_name;
        struct dir_entry_t entry;
        if(entry_for_path(name, &entry))
        {
 
            for(i = 0; i < dir->num_entries; i++) 
            {

                uint32_t cluster = dir->entries[i].first_cluster;
                if(cluster == 0) { cluster = 2; }
                fat_free_dir(fs, dir);
                fat_populate_dir(fs, dir, cluster);
                break;
            }
        }
    }
    void fat_dir_by_name(f32 *fs,struct directory *dir,char* name)
    {
        uint32_t i;
        uint32_t max_name;
        struct dir_entry_t entry;
        if(entry_for_path(name, &entry))
        {
 
            for(i = 0; i < dir->num_entries; i++) 
            {

                uint32_t cluster = dir->entries[i].first_cluster;
                if(cluster == 0) { cluster = 2; }
                fat_free_dir(fs, dir);
                fat_populate_dir(fs, dir, cluster);
                fat_print_directory(fs, dir);
                break;
            }
        }
    }
    uint32_t fat_get_file_size(char* name)
    {
        struct dir_entry_t entry;
        if(entry_for_path(name, &entry))
        {
            debug_writeln_ext("FOUND file_t", COL4_GREEN);
            return entry.file_size;
        }
        debug_writeln_ext("UNABLE TO FIND file_t", COL4_RED);
        return 0;
    }

    void fat_cat_file(f32 *fs, struct directory *dir, char* name)
    {
        uint32_t i;
        uint32_t max_name;
        file_t *f = fopen(name, NULL);
        if(f) 
        {
            #define BCOUNT 1000
            uint8_t c[BCOUNT];
            int count, total;
            while((count = fread(&c, BCOUNT, 1, f)), count > 0) 
            {
                for(int i = 0; i < count; i++) { System::KernelIO::Terminal.WriteChar(c[i]); }
                total += count;
            }

            fclose(f);
        }
        else
        {
            System::KernelIO::Terminal.WriteLine("File not found, if you are sure it exists make sure to add a / to the filename.",COL4_RED);
            System::KernelIO::Terminal.WriteLine("For example: 'cat /yourfile.txt' ",COL4_RED);

        }
    }
    char print_temp[16];
    void fat_print_directory(f32 *fs, struct directory *dir) 
    {
        System::KernelIO::Terminal.WriteLine("Listing Directory",COL4_CYAN);
        System::KernelIO::Terminal.NewLine();

        uint32_t i;
        uint32_t max_name = 0;
        for(i = 0; i < dir->num_entries; i++) 
        {
            uint32_t namelen = strlen(dir->entries[i].name);
            max_name = namelen > max_name ? namelen : max_name;
        }

        char *namebuff = (char*)mem_alloc(max_name + 1);

        for(i = 0; i < dir->num_entries; i++) 
        {
            uint32_t j;
            for(j = 0; j < max_name; j++) { namebuff[j] = ' '; }
            namebuff[max_name] = 0;

            for(j = 0; j < strlen(dir->entries[i].name); j++) { namebuff[j] = dir->entries[i].name[j]; }

            if(dir->entries[i].dir_attrs == DIRECTORY) 
            { 
                System::KernelIO::Terminal.Write("-- ");
                System::KernelIO::Terminal.WriteLine(namebuff, COL4_GREEN);
            } 
            else 
            { 
                System::KernelIO::Terminal.Write(namebuff, COL4_YELLOW); 
                System::KernelIO::Terminal.Write("        ");
                strdec(dir->entries[i].file_size, print_temp);
                System::KernelIO::Terminal.Write(print_temp);
                System::KernelIO::Terminal.WriteLine(" bytes");
            }

            uint32_t cluster = dir->entries[i].first_cluster;
            uint32_t cluster_count = 1;

            while(1) 
            {
                cluster = fs->FAT[cluster];
                if(cluster >= EOC) break;
                if(cluster == 0) { debug_throw_panic("BAD CLUSTER CHAIN! FS IS CORRUPT!"); }
                cluster_count++;
            }
        }
        
        mem_free(namebuff);
    }

    uint32_t fat_count_free_clusters(f32 *fs) 
    {
        uint32_t clusters_in_fat = (fs->bpb.count_sectors_per_FAT32 * 512) / 4;
        uint32_t i;
        uint32_t count = 0;
        for(i = 0; i < clusters_in_fat; i++) 
        {
            if(fs->FAT[i] == 0) { count++; }
        }
        return count;
    }


 
    file_t *fopen(const char *pathname, const char *mode) 
    {
        struct dir_entry_t entry;
        if(!entry_for_path(pathname, &entry)) 
        {
            mem_free(entry.name);
            return NULL;
        }
    //    printf("Got entry: %s [%d]\n", entry.name, entry.first_cluster);
        mem_free(entry.name);

        file_t* f = (file_t*)mem_alloc(sizeof (void) + fat_master_fs->cluster_size);
        f->curr_cluster = entry.first_cluster;
        f->file_size = entry.file_size;
        f->fptr = 0;
        f->buffptr = 0;
        fat_get_cluster(fat_master_fs, f->currbuf, f->curr_cluster);
        return f;
    }

    int fclose(file_t *stream) { mem_free(stream); }

    size_t fread(void *ptr, size_t size, size_t nmemb, file_t *stream) 
    {
        size_t bytes_to_read = size * nmemb;
        size_t bytes_read = 0;

        if(stream->fptr + bytes_to_read > stream->file_size)
        {
            bytes_to_read = stream->file_size - stream->fptr;
        }

        //printf("Reading %d bytes.\n", bytes_to_read);
        while(bytes_to_read > 0) 
        {
            if(stream->buffptr + bytes_to_read > fat_master_fs->cluster_size) 
            {
                // Need to read at least 1 more cluster
                size_t to_read_in_this_cluster = fat_master_fs->cluster_size - stream->buffptr;
                mem_copy2((uint8_t*)ptr + bytes_read, stream->currbuf + stream->buffptr, to_read_in_this_cluster);
                bytes_read += to_read_in_this_cluster;
                //printf("buffptr = 0\n");
                stream->buffptr = 0;
                //printf("Getting next cluster.\n");
                stream->curr_cluster = fat_get_next_cluster_id(fat_master_fs, stream->curr_cluster);
                //printf("Next cluster: %x\n", stream->curr_cluster);
                if(stream->curr_cluster >= EOC) 
                {
                    //printf("Returning.\n");
                    stream->fptr += bytes_read;
                    return bytes_read;
                }
                //printf("getting next cluster.\n");
                fat_get_cluster(fat_master_fs, stream->currbuf, stream->curr_cluster);
                bytes_to_read -= to_read_in_this_cluster;
            }
            else 
            {
                //printf("buffptr: %d\n", stream->buffptr);
                mem_copy2((uint8_t*)ptr + bytes_read, stream->currbuf + stream->buffptr, bytes_to_read);
                bytes_read += bytes_to_read;
                stream->buffptr += bytes_to_read;
                bytes_to_read = 0;
            }
        }
        //printf("Returning.\n");
        stream->fptr += bytes_read;
        return bytes_read;
    }

    size_t fwrite(const void *ptr, size_t size, size_t nmemb, file_t *stream);
}