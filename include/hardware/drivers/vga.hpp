#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/terminal.hpp"
#include "graphics/colors.hpp"
#include "graphics/font.hpp"

namespace HAL
{
    // used to manage video mode properties
    struct VideoMode
    {
        private:
            // dimensions
            uint16_t Width, Height;

            // bits per pixel
            COLOR_DEPTH Depth;

            // text mode flag
            bool TextMode;

            // double buffered flag
            bool DoubleBuffered;
        public:
            // default constructor
            VideoMode() : Width(0), Height(0), Depth(COLOR_DEPTH_4), TextMode(false), DoubleBuffered(false) { }
            // constructor
            VideoMode(uint16_t w, uint16_t h, COLOR_DEPTH bpp, bool text, bool db) 
                : Width(w), Height(h), Depth(bpp), TextMode(text), DoubleBuffered(db) { }
            
            // get dimensions
            uint16_t GetWidth() { return Width; }
            uint16_t GetHeight() { return Height; }
            
            // get depth
            COLOR_DEPTH GetDepth() { return Depth; }

            // get text mode flag
            bool IsTextMode() { return TextMode; }

            // get double buffered flag
            bool IsDoubleBuffered() { return DoubleBuffered; }
    };

    class VGADriver
    {
        public:
            // constructor
            VGADriver();

            // initialize ports
            void Initialize();

            // set active video mode
            void SetMode(VideoMode mode);

            // get video mode
            VideoMode GetMode();

            // check if specified mode is compatible
            bool IsModeCompatible(VideoMode mode);

            // get available mode by index
            VideoMode GetAvailableMode(uint32_t index);

            // get amount of available modes
            uint8_t GetAvailableModeCount();

            // clear the screen
            void Clear(uint8_t color);

            // draw pixel to screen
            void SetPixel(uint16_t x, uint16_t y, uint8_t color);

            // swap buffers
            void Swap();

            // pack 2 4-bit colors into 1 bytes
            uint8_t PackColors(uint8_t fg, uint8_t bg);

        private:
            // get frame buffer segment
            uint8_t* GetFrameBufferSegment();

            // write data to registers
            void WriteRegisters(uint8_t* regs);

            // set plane
            void SetPlane(uint8_t a);

            // load font into memory
            void SetFont(uint8_t* data, uint8_t height);

            // video buffer address
            uint8_t* Buffer;
            uint8_t* BackBuffer;

            // active video mode
            VideoMode Mode;

            // available modes
            VideoMode AvailableModes[5];
            uint8_t AvailableModeCount;

            // ports
            IOPort PortIndexAC, PortWriteAC, PortReadAC;
            IOPort PortWriteMisc, PortReadMisc;
            IOPort PortIndexSeq, PortDataSeq;
            IOPort PortWriteDAC, PortReadDAC, PortDataDAC;
            IOPort PortIndexGC, PortDataGC;
            IOPort PortIndexCRTC, PortDataCRTC;
            IOPort PortReadInstat;
    };
}