#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"

namespace HAL
{
    struct VbeInfoBlock {
	    char VbeSignature[4];             // == "VESA"
		uint16_t VbeVersion;                 // == 0x0300 for VBE 3.0
		uint16_t OemStringPtr[2];            // isa vbeFarPtr
		uint8_t Capabilities[4];
		uint16_t VideoModePtr[2];         // isa vbeFarPtr
		uint16_t TotalMemory;             // as # of 64KB blocks
    } __attribute__((packed));

    struct ModeInfoBlock {
        uint16_t attributes;
        uint8_t winA, winB;
        uint16_t granularity;
		uint16_t winsize;
		uint16_t segmentA, segmentB;
		uint16_t realFctPtr[2];
		uint16_t pitch; // bytes per scanline

		uint16_t Xres, Yres;
		uint8_t Wchar, Ychar, planes, bpp, banks;
		uint8_t memory_model, bank_size, image_pages;
		uint8_t reserved0;

		uint8_t red_mask, red_position;
		uint8_t green_mask, green_position;
		uint8_t blue_mask, blue_position;
		uint8_t rsv_mask, rsv_position;
		uint8_t directcolor_attributes;

		uint32_t physbase;  // your LFB (Linear Framebuffer) address ;)
		uint32_t reserved1;
		uint16_t reserved2;
	} __attribute__((packed));

    class VESA
    {
    private:
        VbeInfoBlock vib;
        ModeInfoBlock mib;
		int16_t width;
		int16_t height;
		bool vibset = false;
        void* buffer;
		void populate_vib();
    public:
        VESA();
		~VESA();

        bool SwitchMode(int16_t width, int16_t height, uint8_t depth);
        void SetPixel(int16_t x, int16_t y, uint32_t color);
        void SetPixel(int16_t x, int16_t y, uint16_t color);
        void SetPixel(int16_t x, int16_t y, uint8_t color);
        uint32_t GetPixel32(int16_t x, int16_t y);
        uint16_t GetPixel16(int16_t x, int16_t y);
        uint8_t GetPixel8(int16_t x, int16_t y);
        void Render();
    };
}