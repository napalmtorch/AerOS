#include <lib/types.h>
#include <gui/bitmap.hpp>
#include <hardware/memory.hpp>
#include <hardware/fat.hpp>

extern "C"
{
    
bitmap_t * bitmap_create(char * filename) 
{
    bitmap_t * ret = (bitmap_t*)mem_alloc(sizeof(bitmap_t));
    FILE * file = fopen(filename, 0);
    if(!file) 
    {
        return NULL;
    }
    struct directory dir;
    populate_root_dir(master_fs, &dir);
    uint32_t size = FileSize(master_fs,&dir,filename);
    void * buf = mem_alloc(size);
    fread(file, 0, size, (FILE*)buf);


    bmp_fileheader_t * h = (bmp_fileheader_t*)buf;
    unsigned int offset = h->bfOffBits;

    bmp_infoheader_t * info = (bmp_infoheader_t*)buf + sizeof(bmp_fileheader_t);

    ret->width = info->biWidth;
    ret->height = info->biHeight;
    ret->image_bytes= (char*)((unsigned int)buf + offset);
    ret->buf = (char*)buf;
    ret->total_size= size;
    ret->bpp = info->biBitCount;
    fclose(file);
    return ret;
}



void bitmap_display(bitmap_t * bmp) 
{
    if(!bmp) return;
    uint8_t * image = (uint8_t*)bmp->image_bytes;
    uint32_t * screen = (uint32_t*)0xfd000000;
    int j = 0;
    for(int i = 0; i < bmp->height; i++) {
        char * image_row = (char*)image + i * bmp->width * 3;
        uint32_t * screen_row = (uint32_t*)screen + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t rgb = ((r << 16) | (g << 8) | (b)) & 0x00ffffff;
            rgb = rgb | 0xff000000;
            screen_row[k] = rgb;
        }
    }
}

void bitmap_to_framebuffer(bitmap_t * bmp, uint32_t * frame_buffer) {
    if(!bmp) return;
    uint8_t * image = (uint8_t*)bmp->image_bytes;
    int j = 0;
    for(int i = 0; i < bmp->height; i++) {
        char * image_row = (char*)image + i * bmp->width * 3;
        uint32_t * framebuffer_row = (uint32_t*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t rgb = ((r << 16) | (g << 8) | (b)) & 0x00ffffff;
            rgb = rgb | 0xff000000;
            framebuffer_row[k] = rgb;
        }
    }
}
void bitmap_to_framebuffer2(bitmap_t * bmp, uint32_t * frame_buffer) {
    if(!bmp) return;
    uint8_t * image = (uint8_t*)bmp->image_bytes;
    int j = 0;
    for(int i = 0; i < bmp->height; i++) {
        char * image_row = (char*)image + i * bmp->width * 4;
        uint32_t * framebuffer_row = (uint32_t*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t a = image_row[j++] & 0xff;
            uint32_t rgba = ((a << 24) | (r << 16) | (g << 8) | (b));
            framebuffer_row[k] = rgba;
        }
    }
}
}