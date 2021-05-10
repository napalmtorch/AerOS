#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"
#include "hardware/drivers/pci.hpp"

namespace HAL
{
    namespace PCI
    {
        enum class Register : uint16_t{
		    IDr = 0,
			Enable = 1,
			Width = 2,
			Height = 3,
			MaxWidth = 4,
			MaxHeight = 5,
			Depth = 6,
			BitsPerPixel = 7,
			PseudoColor = 8,
			RedMask = 9,
			GreenMask = 10,
			BlueMask = 11,
			BytesPerLine = 12,
			FrameBufferStart = 13,
			FrameBufferOffset = 14,
			VRamSize = 15,
			FrameBufferSize = 16,
			Capabilities = 17,
			MemStart = 18,
			MemSize = 19,
			ConfigDone = 20,
			Sync = 21,
			Busy = 22,
			GuestID = 23,
			CursorID = 24,
			CursorX = 25,
			CursorY = 26,
			CursorOn = 27,
			HostBitsPerPixel = 28,
			ScratchSize = 29,
			MemRegs = 30,
			NumDisplays = 31,
			PitchLockReg = 32,
			FifoNumRegisters = 293
		};
		enum class ID : uint32_t{
		    Magic = 0x900000,
		    V0 = Magic << 8,
		    V1 = (Magic << 8) | 1,
			V2 = (Magic << 8) | 2,
			Invalid = 0xFFFFFFFF
		};
		enum class FIFO : uint32_t{
		    Min = 0,
			Max = 4,
			NextCmd = 8,
			Stop = 12
		};
		enum class FIFOCommand{
		    Update = 1,
			RECT_FILL = 2,
			RECT_COPY = 3,
			DEFINE_BITMAP = 4,
			DEFINE_BITMAP_SCANLINE = 5,
			DEFINE_PIXMAP = 6,
			DEFINE_PIXMAP_SCANLINE = 7,
			RECT_BITMAP_FILL = 8,
			RECT_PIXMAP_FILL = 9,
			RECT_BITMAP_COPY = 10,
			RECT_PIXMAP_COPY = 11,
			FREE_OBJECT = 12,
			RECT_ROP_FILL = 13,
			RECT_ROP_COPY = 14,
			RECT_ROP_BITMAP_FILL = 15,
			RECT_ROP_PIXMAP_FILL = 16,
			RECT_ROP_BITMAP_COPY = 17,
			RECT_ROP_PIXMAP_COPY = 18,
			DEFINE_CURSOR = 19,
			DISPLAY_CURSOR = 20,
			MOVE_CURSOR = 21,
			DEFINE_ALPHA_CURSOR = 22
		};
		enum class IOPortOffset : uint8_t{
		    Index = 0,
			Value = 1,
			Bios = 2,
			IRQ = 3
		};
		enum class Capability{
		    None = 0,
			RectFill = 1,
			RectCopy = 2,
			RectPatFill = 4,
			LecacyOffscreen = 8,
			RasterOp = 16,
			Cursor = 32,
			CursorByPass = 64,
			CursorByPass2 = 128,
			EigthBitEmulation = 256,
			AlphaCursor = 512,
			Glyph = 1024,
			GlyphClipping = 0x00000800,
			Offscreen1 = 0x00001000,
			AlphaBlend = 0x00002000,
			ThreeD = 0x00004000,
			ExtendedFifo = 0x00008000,
			MultiMon = 0x00010000,
			PitchLock = 0x00020000,
			IrqMask = 0x00040000,
        	DisplayTopology = 0x00080000,
		    Gmr = 0x00100000,
			Traces = 0x00200000,
			Gmr2 = 0x00400000,
			ScreenObject2 = 0x00800000
		};
        class VMWareSVGA2
        {
        };
    }
}