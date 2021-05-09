#include <hardware/drivers/vesa.hpp>
#include <boot/realmode.hpp>
#include <core/debug.hpp>

#define LNG_PTR(seg, off) ((seg << 4) | off)
#define REAL_PTR(arr) LNG_PTR(arr[1], arr[0])
#define SEG(addr) (((uint32_t)addr >> 4) & 0xF000)
#define OFF(addr) ((uint32_t)addr & 0xFFFF)

namespace HAL
{
    void VESA::Initialize()
    {
        InfoBlockSet = false;
        Enabled = false;
        Width = 0;
        Height = 0;
    }

    void VESA::Disable()
    {
        if (Buffer != nullptr) { delete Buffer; }
    }

    // populate the VbeInfoBlock
    void VESA::PopulateInfoBlock()
    {
        // if yet set, skip this
        if (InfoBlockSet) return;

        System::KernelIO::ThrowMessage(MSG_TYPE_SYSTEM, "VESA: Gathering available VESA modes from BIOS...");
        
        // set the locvib location
        VBEInfoBlock* info = (VBEInfoBlock*)0x80000;

        // call the bios interrupt
        regs16_t regs;
        regs.ax = 0x4f00;
        regs.es = 0x8000;
        regs.di = 0x0;

        int32(0x10, &regs);
        
        // state that the VbeInfoBlock was set up
        InfoBlockSet = true;
        
        // store data
        InfoBlock = *info;
        System::KernelIO::ThrowOK("Initialized VESA driver");
    }

    bool VESA::SetMode(int16_t width, int16_t height, uint8_t depth)
    {
        // if vesa is enabled, delete the old buffer
        if (Buffer != nullptr) { Buffer = nullptr; }
        if (!Enabled) { Enabled = true; }

        // populate the vib
        PopulateInfoBlock();
        
        // prepare the environment for the BIOS calls
        regs16_t regs;
        VBEModeInfoBlock* mode_info = (VBEModeInfoBlock*)(0x80000 + sizeof(VBEInfoBlock) + 512);
        uint16_t* modes = (uint16_t*)REAL_PTR(InfoBlock.VideoMode);
        // resulting mode
        uint16_t mode;
        // if the mode was set or not
        bool set = false;
        for (int i = 0; modes[i] != 0xFFFF; i++)
        {
            // set up interrupt to get mode info
            regs.ax = 0x4f01;
            regs.cx = modes[i];
            regs.es = SEG(mode_info);
            regs.di = OFF(mode_info);
            // call interrupt
            int32(0x10, &regs);

            // check if the mode is correct
            if (mode_info->Width == width && mode_info->Height == height && mode_info->Depth == depth)
            {
                set = true;
                mode = modes[i];
                break;
            }

            // otherwise, continue
        }
        // if no mode was found, return false
        if (!set) { System::KernelIO::ThrowError("VESA: No matching mode was found!"); return false; }
        
        // set the backbuffer up
        System::KernelIO::ThrowSystem("VESA: setting up buffer");
        // buffer size
        int size = (mode_info->Height * mode_info->Pitch);
        switch (depth)
        {
            case 8:
                // 8bit buffer
                Buffer = new uint8_t[size];
                break;
            case 16:
                // 16bit buffer
                Buffer = new uint16_t[size];
                break;
            case 32:
                // 32bit buffer
                Buffer = new uint32_t[size];
                break;
            default:
                // depth not found
                System::KernelIO::ThrowError("VESA: Depth not supported");
                return false;
        }

        Width = mode_info->Height;
        Height = mode_info->Width;

        // enable the mode with the acording interrupt
        regs.ax = 0x4F02;
        regs.bx = mode | 0x4000;
        int32(0x10, &regs);
        System::KernelIO::ThrowSystem("VESA: done.");

        // store mib data
        ModeInfoBlock = *mode_info;
        // return
        return true;
    }

    void VESA::Clear(uint32_t color)
    {
        if (ModeInfoBlock.Depth != 32) { return; }
        for (size_t i = 0; i < Width * Height; i++) { ((uint32_t*)Buffer)[i] = color; }
    }
	void VESA::Clear(uint16_t color)
    {
        if (ModeInfoBlock.Depth != 16) { return; }
        for (size_t i = 0; i < Width * Height; i++) { ((uint16_t*)Buffer)[i] = color; }
    }

	void VESA::Clear(uint8_t color)
    {
        if (ModeInfoBlock.Depth != 8) { return; }
        for (size_t i = 0; i < Width * Height; i++) { ((uint8_t*)Buffer)[i] = color; }
    }

    void VESA::SetPixel(int16_t x, int16_t y, uint32_t color)
    {
        if (x >= ModeInfoBlock.Width || y >= ModeInfoBlock.Height)  { return; }
        if (ModeInfoBlock.Depth != 32) return;
        
        // store the color in the allocated buffer
        ((uint32_t*)Buffer)[x + (y * ModeInfoBlock.Width)] = color;
    }
    void VESA::SetPixel(int16_t x, int16_t y, uint16_t color)
    {
        if (x >= ModeInfoBlock.Width || y >= ModeInfoBlock.Height)  { return; }
        if (ModeInfoBlock.Depth != 16) return;

        // store the color in the allocated buffer
        ((uint16_t*)Buffer)[x + (y * ModeInfoBlock.Width)] = color;
    }
    void VESA::SetPixel(int16_t x, int16_t y, uint8_t color)
    {
        if (x >= ModeInfoBlock.Width || y >= ModeInfoBlock.Height)  { return; }
        if (ModeInfoBlock.Depth != 16) return;

        // store the color in the allocated buffer
        ((uint8_t*)Buffer)[x + (y * ModeInfoBlock.Width)] = color;
    }

    uint32_t VESA::GetPixel32(int16_t x, int16_t y)
    {
        if (x >= ModeInfoBlock.Width || y >= ModeInfoBlock.Height)  { return 0; }
        if (ModeInfoBlock.Depth != 32) { return 0; }

        // return the color stored into the buffer
        return ((uint32_t*)Buffer)[x + (y * ModeInfoBlock.Width)];
    }
    uint16_t VESA::GetPixel16(int16_t x, int16_t y)
    {
        if (x >= ModeInfoBlock.Width || y >= ModeInfoBlock.Height)  { return 0; }
        if (ModeInfoBlock.Depth != 16) { return 0; }

        // return the color stored into the buffer
        return ((uint16_t*)Buffer)[x + (y * ModeInfoBlock.Width)];
    }
    uint8_t VESA::GetPixel8(int16_t x, int16_t y)
    {
        if (x >= ModeInfoBlock.Width || y >= ModeInfoBlock.Height)  { return 0; }
        if (ModeInfoBlock.Depth != 8) { return 0; }

        // return the color stored into the buffer
        return ((uint8_t*)Buffer)[x + (y * ModeInfoBlock.Width)];
    }
    void VESA::Render()
    {
        // copy data from buffer to video address
        mem_copy((uint8_t*)Buffer, (uint8_t*)ModeInfoBlock.PhysicalBase, (ModeInfoBlock.Height * ModeInfoBlock.Pitch));
    }

    uint32_t VESA::GetWidth() { return Width; }
	uint32_t VESA::GetHeight() { return Height; }
}