#include "graphics/bitmap.hpp"
#include <hardware/memory.hpp>
#include <core/kernel.hpp>

namespace Graphics
{
    // constructor - blank
    Bitmap::Bitmap() { }

    // constructor - new bitmap
    Bitmap::Bitmap(int32_t w, int32_t h, COLOR_DEPTH depth)
    {
        if ((uint8_t)depth < 24) { System::KernelIO::ThrowError("Only 24 and 32-bit bitmaps are supported"); return; }

        Width = w;
        Height = h;
        Depth = depth;
        ImageData = new uint8_t[Width * Height * 4];
    }

    // constructor - load from disk
    Bitmap::Bitmap(char* fullname)
    {
        HAL::nfs_file_t file = System::KernelIO::NapalmFS.ReadFile(fullname);    
        debug_write("NAME: ");
        debug_writeln_ext(file.name, COL4_YELLOW);

        bmp_fileheader_t* h = (bmp_fileheader_t*)file.data;
        uint32_t offset = h->bfOffBits;
        debug_writeln_dec("BMP SIZE:   ", h->bfSize);
        debug_writeln_dec("BMP OFFSET: ", offset);

        bmp_infoheader_t* info = (bmp_infoheader_t*)(file.data + sizeof(bmp_fileheader_t));
        
        Width = info->biWidth;
        Height = info->biHeight;
        ImageData = (uint8_t*)((uint32_t)file.data + offset);
        Buffer = (uint8_t*)file.data;
        TotalSize = file.size;
        Depth = (COLOR_DEPTH)info->biBitCount;
        if (Width == 0 || Height == 0 || Depth == 0 || file.size < 0 || strlen(file.name) == 0) { System::KernelIO::ThrowError("Unable to locate file"); return; }

        uint32_t* new_data = (uint32_t*)mem_alloc(Width * Height * 4);
        for (int32_t yy = Height - 1; yy >= 0; yy--)
        {
            for (int32_t xx = 0; xx < Width; xx++)
            {
                if (Depth == COLOR_DEPTH_24)
                {
                    uint32_t offset = (3 * (xx + (yy * Width)));
                    uint32_t color = Graphics::RGBToPackedValue(ImageData[offset + 2], ImageData[offset + 1], ImageData[offset]);
                    uint32_t output_offset = xx + ((Height - yy - 1) * Width);
                    new_data[output_offset] = color;
                }
                else if (Depth == COLOR_DEPTH_32)
                {
                    uint32_t offset = (4 * (xx + (yy * Width)));
                    uint32_t color = Graphics::RGBToPackedValue(ImageData[offset + 2], ImageData[offset + 1], ImageData[offset]);
                    uint32_t output_offset = xx + ((Height - yy - 1) * Width);
                    new_data[output_offset] = color;
                }
            }
        }  
        ImageData = (uint8_t*)new_data;
        mem_free((void*)file.data);
        

        debug_writeln_dec("BMP WIDTH:  ", Width);
        debug_writeln_dec("BMP HEIGHT: ", Height);
        debug_writeln_dec("BMP DEPTH:  ", (uint32_t)Depth);
        debug_writeln_dec("BMP SIZE:   ", TotalSize);

        /*
        if(fat_master_fs != nullptr)
        {
            // open file
            file_t *f = fopen(fullname, NULL);

            // file was located
            if(f)
            { 
                // print filename
                System::KernelIO::Write("Loading bitmap '");
                System::KernelIO::Write(fullname);
                System::KernelIO::Write("'\n");

                struct directory dir;
                fat_populate_root_dir(fat_master_fs, &dir);
                uint32_t size = fat_get_file_size(fullname);

                // read data from disk
                void* buf = mem_alloc(size);
                fread(buf, size, 1, (file_t*)f);

                bmp_fileheader_t* h = (bmp_fileheader_t*)buf;
                uint32_t offset = h->bfOffBits;
                debug_writeln_dec("BMP SIZE:   ", h->bfSize);
                debug_writeln_dec("BMP OFFSET: ", offset);

                bmp_infoheader_t* info = (bmp_infoheader_t*)(buf + sizeof(bmp_fileheader_t));
                
                Width = info->biWidth;
                Height = info->biHeight;
                ImageData = (uint8_t*)((uint32_t)buf + offset);
                Buffer = (uint8_t*)buf;
                TotalSize = size;
                Depth = (COLOR_DEPTH)info->biBitCount;
                

                debug_writeln_dec("BMP WIDTH:  ", Width);
                debug_writeln_dec("BMP HEIGHT: ", Height);
                debug_writeln_dec("BMP DEPTH:  ", (uint32_t)Depth);
                debug_writeln_dec("BMP SIZE:   ", TotalSize);

                fclose(f);
            }
            // unable to locate file
            else
            {
                // print filename
                System::KernelIO::Write("["); System::KernelIO::Write("  !!  "); System::KernelIO::Write("] ");
                System::KernelIO::Write("Unable to load bitmap '");
                System::KernelIO::Write(fullname);
                System::KernelIO::Write("'\n");
                return;
            }
        }  
        */
    }

    Bitmap::~Bitmap()
    {
        delete Buffer;
    }

    void Bitmap::Resize(uint32_t w, uint32_t h)
    {
        int32_t x_ratio = (int32_t)((Width << 16) / w) + 1;
        int32_t y_ratio = (int32_t)((Height << 16) / h) + 1;
        int32_t xx, yy;
        uint32_t* new_data = (uint32_t*)mem_alloc(w * h);
        for (size_t i = 0; i < h; i++)
        {
            for (size_t j = 0; j < w; j++)
            {
                xx = ((j * x_ratio) >> 16);
                yy = ((i * y_ratio) >> 16);
                new_data[j + (i * w)] = ImageData[(xx + (yy * Width))];
            }
        }
        if (ImageData != nullptr) { mem_free(ImageData); }
        ImageData = (uint8_t*)new_data;
        Width = w;
        Height = h;
    }
}