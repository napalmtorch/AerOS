#pragma once

#include <lib/types.h>
extern "C" {
struct bios_parameter_block {
    uint16_t bytes_per_sector;          // IMPORTANT
    uint8_t sectors_per_cluster;        // IMPORTANT
    uint16_t reserved_sectors;          // IMPORTANT
    uint8_t FAT_count;                  // IMPORTANT
    uint16_t dir_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor_type;
    uint16_t count_sectors_per_FAT12_16; // FAT12/FAT16 only.
    uint16_t count_sectors_per_track;
    uint16_t count_heads_or_sizes_on_media;
    uint32_t count_hidden_sectors;
    uint32_t large_sectors_on_media;  // This is set instead of total_sectors if it's > 65535

    // Extended Boot Record
    uint32_t count_sectors_per_FAT32;   // IMPORTANT
    uint16_t flags;
    uint16_t FAT_version;
    uint32_t cluster_number_root_dir;   // IMPORTANT
    uint16_t sector_number_FSInfo;
    uint16_t sector_number_backup_boot_sector;
    uint8_t drive_number;
    uint8_t windows_flags;
    uint8_t signature;                  // IMPORTANT
    uint32_t volume_id;
    char volume_label[12];
    char system_id[9];
};

#define READONLY  1
#define HIDDEN    (1 << 1)
#define SYSTEM    (1 << 2)
#define VolumeID  (1 << 3)
#define DIRECTORY (1 << 4)
#define ARCHIVE   (1 << 5)
#define LFN (READONLY | HIDDEN | SYSTEM | VolumeID)

struct dir_entry {
    char *name;
    uint8_t dir_attrs;
    uint32_t first_cluster;
    uint32_t file_size;
};
typedef struct FILE {
    //struct dir_entry file_ent;
    uint32_t curr_cluster;
    uint32_t file_size; // total file size
    uint32_t fptr; // index into the file
    uint32_t buffptr; // index into currbuf
    uint8_t currbuf[]; // flexible member for current cluster
} FILE;


FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *stream);

int fclose(FILE *stream);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
struct directory {
    uint32_t cluster;
    struct dir_entry *entries;
    uint32_t num_entries;
};

// REFACTOR
// I want to get rid of this from the header. This should be internal
// implementation, but for now, it's too convenient for stdio.c impl.

// EOC = End Of Chain
#define EOC 0x0FFFFFF8

struct f32 {
    //FILE *f;
    uint32_t *FAT;
    struct bios_parameter_block bpb;
    uint32_t partition_begin_sector;
    uint32_t fat_begin_sector;
    uint32_t cluster_begin_sector;
    uint32_t cluster_size;
    uint32_t cluster_alloc_hint;
};

typedef struct f32 f32;

void getCluster(f32 *fs, uint8_t *buff, uint32_t cluster_number);
uint32_t get_next_cluster_id(f32 *fs, uint32_t cluster);

// END REFACTOR

f32 *makeFilesystem(char *fatSystem);
void destroyFilesystem(f32 *fs);

const struct bios_parameter_block *getBPB(f32 *fs);

void populate_root_dir(f32 *fs, struct directory *dir);
void populate_dir(f32 *fs, struct directory *dir, uint32_t cluster);
void free_directory(f32 *fs, struct directory *dir);

uint8_t *readFile(f32 *fs, struct dir_entry *dirent);
void writeFile(f32 *fs, struct directory *dir, uint8_t *file, char *fname, uint32_t flen);
void mkdir(f32 *fs, struct directory *dir, char *dirname);
void delFile(f32 *fs, struct directory *dir, char *filename);
void DirByName(f32 *fs,struct directory *dir,char* name);
uint32_t FileSize(f32 *fs,struct directory *dir,char* name);
void cat_file(f32 *fs,struct directory *dir,char* name);
void print_directory(f32 *fs, struct directory *dir);
uint32_t count_free_clusters(f32 *fs);

extern f32 *master_fs;
}
