#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/drivers/vga.hpp"

namespace HAL
{
    typedef struct {
	    char Signature[4];             // == "VESA"
		uint16_t Version;                 // == 0x0300 for VBE 3.0
		uint16_t OEMString[2];            // isa vbeFarPtr
		uint8_t Capabilities[4];
		uint16_t VideoMode[2];         // isa vbeFarPtr
		uint16_t TotalMemory;             // as # of 64KB blocks
    } __attribute__((packed)) VBEInfoBlock;

    typedef struct 
	{
        uint16_t Attributes;
        uint8_t WindowA, WindowB;
        uint16_t Granularity;
		uint16_t WindowSize;
		uint16_t SegmentA, SegmentB;
		uint16_t WindowFunction[2];
		uint16_t Pitch; // bytes per scanline

		uint16_t Width, Height;
		uint8_t CharWidth, CharHeight, Planes, Depth, Banks;
		uint8_t MemoryModel, BankSize, ImagePages;
		uint8_t Reserved0;

		uint8_t RedMask, RedPosition;
		uint8_t GreenMask, GreenPosition;
		uint8_t BlueMask, BluePosition;
		uint8_t RSVMask, RSVPosition;
		uint8_t DirectColor;

		uint32_t PhysicalBase;  // your LFB (Linear Framebuffer) address ;)
		uint32_t Reserved1;
		uint16_t Reserved2;
	} __attribute__((packed)) VBEModeInfoBlock;

    class VESA
    {
    public:
        VBEInfoBlock InfoBlock;
        VBEModeInfoBlock ModeInfoBlock;
		uint32_t Width, Height;
		bool InfoBlockSet = false;
		bool Enabled = false;
        void* Buffer;
		void PopulateInfoBlock();
    public:
        void Initialize();
		void Disable();
        bool SetMode(int16_t width, int16_t height, uint8_t depth);
		void Clear(uint32_t color);
		void Clear(uint16_t color);
		void Clear(uint8_t color);
        void SetPixel(int16_t x, int16_t y, uint32_t color);
        void SetPixel(int16_t x, int16_t y, uint16_t color);
        void SetPixel(int16_t x, int16_t y, uint8_t color);
		void Render();
		uint8_t  GetPixel8(int16_t x, int16_t y);
        uint32_t GetPixel32(int16_t x, int16_t y);
        uint16_t GetPixel16(int16_t x, int16_t y);
		uint32_t GetWidth();
		uint32_t GetHeight();		
    };
}