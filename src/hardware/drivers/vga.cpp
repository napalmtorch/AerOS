#include "hardware/drivers/vga.hpp"
#include "core/kernel.hpp"

// 80x25 mode data
static uint8_t MODE_80x25_DATA[61] = 
{
    /* MISC */
    0x67,
    /* SEQ */
    0x03, 0x00, 0x03, 0x00, 0x02,
    /* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
    0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
    0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
    0xFF,
    /* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
    0xFF,
    /* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x0C, 0x00, 0x0F, 0x08, 0x00
};

// 80x50 mode data
static uint8_t MODE_80x50_DATA[61] = 
{
    /* MISC */
    0x67,
    /* SEQ */
    0x03, 0x00, 0x03, 0x00, 0x02,
    /* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
    0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x01, 0x40,
    0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
    0xFF, 
    /* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
    0xFF, 
    /* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x0C, 0x00, 0x0F, 0x08, 0x00,
};

// 90x60 mode data
static uint8_t MODE_90x60_DATA[61] =
{
    /* MISC */
    0xE7,
    /* SEQ */
    0x03, 0x01, 0x03, 0x00, 0x02,
    /* CRTC */
    0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E,
    0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
    0xEA, 0x0C, 0xDF, 0x2D, 0x08, 0xE8, 0x05, 0xA3,
    0xFF,
    /* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
    0xFF,
    /* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x0C, 0x00, 0x0F, 0x08, 0x00,
};

// 320x200 mode data
static uint8_t MODE_320x200_DATA[61] = 
{
    /* MISC */
    0x63,
    /* SEQ */
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    /* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    /* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    /* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00,	0x00
};

// register counts
#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)

namespace HAL
{
    // constructor
    VGADriver::VGADriver() { this->Initialize(); }

    // initialize ports
    void VGADriver::Initialize()
    {
        // set port values
        this->PortIndexAC = IOPort(0x3C0);
        this->PortWriteAC = IOPort(0x3C0);
        this->PortReadAC = IOPort(0x3C1);
        this->PortWriteMisc = IOPort(0x3C2);
        this->PortIndexSeq = IOPort(0x3C4);
        this->PortDataSeq = IOPort(0x3C5);
        this->PortReadDAC = IOPort(0x3C7);
        this->PortWriteDAC = IOPort(0x3C8);
        this->PortDataDAC = IOPort(0x3C9);
        this->PortReadMisc = IOPort(0x3CC);
        this->PortIndexGC = IOPort(0x3CE);
        this->PortDataGC = IOPort(0x3CF);
        this->PortIndexCRTC = IOPort(0x3D4);
        this->PortDataCRTC = IOPort(0x3D5);
        this->PortReadInstat = IOPort(0x3DA);

        // set available modes
        AvailableModeCount = 5;
        AvailableModes[0] = VideoMode(80,  25,  COLOR_DEPTH_4, true,  false);
        AvailableModes[1] = VideoMode(80,  50,  COLOR_DEPTH_4, true,  false);
        AvailableModes[2] = VideoMode(90,  60,  COLOR_DEPTH_4, true,  false);
        AvailableModes[3] = VideoMode(320, 200, COLOR_DEPTH_8, false, false);
        AvailableModes[4] = VideoMode(320, 200, COLOR_DEPTH_8, false, true);

        SetMode(AvailableModes[0]);
    }

     // set active video mode
    void VGADriver::SetMode(VideoMode mode)
    {
        // check if mode is compatible
        if (!IsModeCompatible(mode)) { System::KernelIO::ThrowError("Incompatible VGA mode"); return; }

        // set mode property
        Mode = mode;

        // write font and registers - not the best method to determine mode but works
        switch (mode.GetHeight())
        {
            // 80x25
            case 25:  { WriteRegisters(MODE_80x25_DATA); SetFont(Graphics::FONT_8x16.GetData(), 16); break; }
            // 80x50
            case 50:  { WriteRegisters(MODE_80x50_DATA); SetFont(Graphics::FONT_8x8.GetData(), 8);   break; }
            // 90x60
            case 60:  { WriteRegisters(MODE_90x60_DATA); SetFont(Graphics::FONT_8x8.GetData(), 8);   break; }
            // 320x200
            case 200: { WriteRegisters(MODE_320x200_DATA); break; }

            // get buffer address
            Buffer = GetFrameBufferSegment();
        }
    }

    // get video mode
    VideoMode VGADriver::GetMode() { return Mode; }

    // check if specified mode is compatible
    bool VGADriver::IsModeCompatible(VideoMode mode)
    {
        for (size_t i = 0; i < AvailableModeCount; i++)
        {
            if (mode.GetWidth() == AvailableModes[i].GetWidth() && mode.GetHeight() == AvailableModes[i].GetHeight() && mode.GetDepth() == AvailableModes[i].GetDepth() &&
                mode.IsTextMode() == AvailableModes[i].IsTextMode() && mode.IsDoubleBuffered() == AvailableModes[i].IsDoubleBuffered())
            {
                return true;
            }
        }
        return false;
    }

    // get available mode by index
    VideoMode VGADriver::GetAvailableMode(uint32_t index)
    {
        if (index >= AvailableModeCount) { System::KernelIO::ThrowError("Invalid VGA mode indentifer"); return AvailableModes[0]; }
        return AvailableModes[index];
    }

    // get amount of available modes
    uint8_t VGADriver::GetAvailableModeCount() { return AvailableModeCount; }

    // clear the screen
    void VGADriver::Clear(uint8_t color)
    {
        // temporary variable
        uint32_t i = 0;

        // clear - text mode
        if (Mode.IsTextMode())
        {
            for (i = 0; i < (Mode.GetWidth() * Mode.GetHeight()) * 2; i += 2)
            {
                Buffer[i] = 0x20;
                Buffer[i + 1] = PackColors((uint8_t)((Buffer[i + 1] & 0xF0) >> 4), color);
            }
        }
        // clear - graphics mode
        else
        {
            // double buffered mode
            if (Mode.IsDoubleBuffered())
            {
                /* TODO: not yet implemented */
                return; 
            }

            // direct memory access mode
            for (i = 0; i < Mode.GetWidth() * Mode.GetHeight(); i++) { Buffer[i] = color; }

        }
    }

    // draw pixel to screen
    void VGADriver::SetPixel(uint16_t x, uint16_t y, uint8_t color)
    {
        // check if pixel is within screen bounds
        if (x >= Mode.GetWidth() || y >= Mode.GetHeight()) { return; }
        uint32_t offset = 0;

        // set pixel in text mode - space character with colored background
        if (Mode.IsTextMode())
        {
            offset = (x + (y * Mode.GetWidth())) * 2;
            Buffer[offset] = 0x20;
            Buffer[offset + 1] = PackColors(((uint8_t)(Buffer[offset + 1] & 0xF0) >> 4), color);
        }
        // set pixel in graphics mode
        else
        {
            // double buffered mode
            if (Mode.IsDoubleBuffered())
            {
                /* TODO: not yet implemented */
                return; 
            }

            // direct memory access mode
            Buffer[offset] = color;
        }
    }

    // swap buffers
    void VGADriver::Swap()
    {
        // confirm mode is double buffered
        if (!Mode.IsDoubleBuffered()) { return; }

        // for (size_t i = 0; i < Mode.GetWidth() * Mode.GetHeight(); i++) { *(Buffer[] + i) = *(BackBuffer + i); }
    }


    // pack 2 4-bit colors into 1 bytes
    uint8_t VGADriver::PackColors(uint8_t fg, uint8_t bg) { return (uint8_t)(fg | (bg << 4)); }

    // get frame buffer segment
    uint8_t* VGADriver::GetFrameBufferSegment()
    {
        PortIndexGC.WriteInt8(6);

        // calculate segment
		uint32_t seg = PortDataGC.ReadInt8();
		seg >>= 2; seg &= 3;

        // convert segment to memory offset
		switch (seg)
		{
			case 0: { break; }
			case 1: { seg = 0xA0000; break; }
			case 2: { seg = 0xB0000; break; }
			case 3: { seg = 0xB8000; break; }
		}

        // return memory offset
		return (uint8_t*)seg;
    }

    // write data to registers
    void VGADriver::WriteRegisters(uint8_t* regs)
    {
        uint32_t i;

		// misc
		PortWriteMisc.WriteInt8(*(regs++));
		
		// sequencer
		for (i = 0; i < VGA_NUM_SEQ_REGS; i++)
		{
			PortIndexSeq.WriteInt8(i);
			PortDataSeq.WriteInt8(*(regs++));
		}

		// unlock crtc
		PortIndexCRTC.WriteInt8(0x03);
		PortDataCRTC.WriteInt8(PortDataCRTC.ReadInt8() | 0x80);
		PortIndexCRTC.WriteInt8(0x11);
		PortDataCRTC.WriteInt8(PortDataCRTC.ReadInt8() & ~0x80);

		// confirm unlock
		regs[0x03] |= 0x80;
		regs[0x11] &= ~0x80;

		// write crtc
		for (i = 0; i < VGA_NUM_CRTC_REGS; i++)
		{
			PortIndexCRTC.WriteInt8(i);
			PortDataCRTC.WriteInt8(*(regs++));
		}

		// gc
		for (i = 0; i < VGA_NUM_GC_REGS; i++)
		{
			PortIndexGC.WriteInt8(i);
			PortDataGC.WriteInt8(*(regs++));
		}

		// ac
		for (i = 0; i < VGA_NUM_AC_REGS; i++)
		{
			(void)PortReadInstat.ReadInt8();
			PortIndexAC.WriteInt8(i);
			PortWriteAC.WriteInt8(*(regs++));
		}

		// lock palette and unblank display
		(void)PortReadInstat.ReadInt8();
		PortIndexAC.WriteInt8(0x20);
    }

    // set plane
    void VGADriver::SetPlane(uint8_t a)
    {
        uint8_t mask;

		a &= 3;
		mask = (uint8_t)(1 << a);

		PortIndexGC.WriteInt8(4);
		PortDataGC.WriteInt8(a);

		PortIndexSeq.WriteInt8(2);
		PortDataSeq.WriteInt8(mask);
    }

    // load font into memory
    void VGADriver::SetFont(uint8_t* data, uint8_t height)
    {
        uint8_t seq2, seq4, gc4, gc5, gc6;

        // sequencer 2
		PortIndexSeq.WriteInt8(2);
		seq2 = PortDataSeq.ReadInt8();

        // sequencer 4
		PortIndexSeq.WriteInt8(4);
		seq4 = PortDataSeq.ReadInt8();
		PortDataSeq.WriteInt8((uint8_t)(seq4 | 0x04));

        // gc 4
		PortIndexGC.WriteInt8(4);
		gc4 = PortDataGC.ReadInt8();

        // gc 5
		PortIndexGC.WriteInt8(5);
		gc5 = PortDataGC.ReadInt8();
		PortDataGC.WriteInt8((uint8_t)(gc5 & ~0x10));

        // gc 6
		PortIndexGC.WriteInt8(6);
		gc6 = PortDataGC.ReadInt8();
		PortDataGC.WriteInt8((uint8_t)(gc6 & ~0x02));

        // set plane to 2
		SetPlane(2);

        // get memory offset
		uint8_t* seg = GetFrameBufferSegment();

        // copy font data to memory
		for (uint32_t i = 0; i < 256; i++)
		{
			for (uint32_t j = 0; j < height; j++)
			{
				seg[(i * 32) + j] = data[(i * height) + j];
			}
		}

        // restore register data
		PortIndexSeq.WriteInt8(2);
		PortDataSeq.WriteInt8(seq2);
		PortIndexSeq.WriteInt8(4);
		PortDataSeq.WriteInt8(seq4);
		PortIndexGC.WriteInt8(4);
		PortDataGC.WriteInt8(gc4);
		PortIndexGC.WriteInt8(5);
		PortDataGC.WriteInt8(gc5);
		PortIndexGC.WriteInt8(6);
		PortDataGC.WriteInt8(gc6);
    }
}