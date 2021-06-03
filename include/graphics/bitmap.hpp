#pragma once
#include <lib/types.h>
#include <hardware/fat.hpp>
#include <graphics/colors.hpp>

namespace Graphics
{
    typedef struct tagBITMAPFILEHEADER 
    {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    }__attribute__((packed)) bmp_fileheader_t;

    typedef struct tagBITMAPINFOHEADER 
    {
        uint32_t    biSize;
        long        biWidth;
        long        biHeight;
        uint16_t    biPlanes;
        uint16_t    biBitCount;
        uint32_t    biCompression;
        uint32_t    biSizeImage;
        long        biXPelsPerMeter;
        long        biYPelsPerMeter;
        uint32_t    biClrUsed;
        uint32_t    biClrImportant;
    } bmp_infoheader_t;


    class Bitmap
    {
        public:
            uint32_t        Width;
            uint32_t        Height;
            COLOR_DEPTH     Depth;
            uint8_t*        ImageData;
            uint8_t*        Buffer;
            uint32_t        TotalSize;
        public:
            // constructor - blank
            Bitmap();

            ~Bitmap();

            // constructor - new image
            Bitmap(int32_t w, int32_t h, COLOR_DEPTH depth);

            // constructor - load image
            Bitmap(char* fullname);

            void Resize(uint32_t w, uint32_t h);
    };
}