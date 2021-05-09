#include <hardware/drivers/vesa.hpp>
#include <boot/realmode.hpp>
#include <core/debug.hpp>

#define LNG_PTR(seg, off) ((seg << 4) | off)
#define REAL_PTR(arr) LNG_PTR(arr[1], arr[0])
#define SEG(addr) (((uint32_t)addr >> 4) & 0xF000)
#define OFF(addr) ((uint32_t)addr & 0xFFFF)

void HAL::VESA::populate_vib()
{
    if (vibset) return;
    System::KernelIO::ThrowMessage(MSG_TYPE_SYSTEM, "VESA: Gathering available VESA modes from BIOS...");
    VbeInfoBlock* locvib = (VbeInfoBlock*)0x80000;

    regs16_t regs;
	regs.ax = 0x4f00;
	regs.es = 0x8000;
    regs.di = 0x0;

    int32(0x10, &regs);
    vibset = true;
    
    vib = *locvib;
    System::KernelIO::ThrowMessage(MSG_TYPE_SYSTEM, "VESA: Succeded");
}

static bool isVesaEnabled = false;
bool HAL::VESA::SwitchMode(int16_t width, int16_t height, uint8_t depth)
{
    if (isVesaEnabled)
    {
        delete buffer;
    }
    populate_vib();
    System::KernelIO::ThrowMessage(MSG_TYPE_SYSTEM, "VESA: Starting...");
    regs16_t regs;
    ModeInfoBlock* locmib = (ModeInfoBlock*)(0x80000 + sizeof(VbeInfoBlock) + 512);
    uint16_t* modes = (uint16_t*)REAL_PTR(vib.VideoModePtr);
    uint16_t mode;
    bool set = false;
    System::KernelIO::ThrowMessage(MSG_TYPE_SYSTEM, "VESA: Looking for requested mode...");
    for (int i = 0; modes[i] != 0xFFFF; i++)
    {
        regs.ax = 0x4f01;
		regs.cx = modes[i];
		regs.es = SEG(locmib);
		regs.di = OFF(locmib);
        int32(0x10, &regs);
        if (locmib->Yres == height && locmib->Xres == width && locmib->bpp == depth)
        {
            System::KernelIO::ThrowMessage(MSG_TYPE_OK, "VESA: Found!");
            set = true;
            mode = modes[i];
            break;
        }
    }
    if (!set) { System::KernelIO::ThrowError("VESA: No matching mode was found!"); return false; }
    System::KernelIO::ThrowSystem("VESA: setting up buffer");
    int size = (locmib->Yres * locmib->pitch);
    switch (depth)
    {
        case 8:
            buffer = new uint8_t[size];
            break;
        case 16:
            buffer = new uint16_t[size];
            break;
        case 32:
            buffer = new uint32_t[size];
            break;
        default:
            System::KernelIO::ThrowError("VESA: colordepth not available");
            return false;
    }
    height = locmib->Yres;
    width = locmib->Xres;
    System::KernelIO::ThrowOK("VESA: buffer set up");
    System::KernelIO::ThrowSystem("VESA: enabling mode...");
    regs.ax = 0x4F02;
    regs.bx = mode | 0x4000;
    int32(0x10, &regs);
    System::KernelIO::ThrowSystem("VESA: done.");

    mib = *locmib;
    return true;
}

void HAL::VESA::SetPixel(int16_t x, int16_t y, uint32_t color)
{
    if (x >= mib.Xres) return;
    if (y >= mib.Yres) return;

    if (mib.bpp != 32) return;
    ((uint32_t*)buffer)[mib.Xres*y+x] = color;
}
void HAL::VESA::SetPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x >= mib.Xres) return;
    if (y >= mib.Yres) return;

    if (mib.bpp != 16) return;
    ((uint16_t*)buffer)[mib.Xres*y+x] = color;
}
void HAL::VESA::SetPixel(int16_t x, int16_t y, uint8_t color)
{
    if (x >= mib.Xres) return;
    if (y >= mib.Yres) return;

    if (mib.bpp != 8) return;
    ((uint8_t*)buffer)[mib.Xres*y+x] = color;
}

uint32_t HAL::VESA::GetPixel32(int16_t x, int16_t y)
{
    if (x >= mib.Xres) return 0;
    if (y >= mib.Yres) return 0;

    return ((uint32_t*)buffer)[width*y+x];
}
uint16_t HAL::VESA::GetPixel16(int16_t x, int16_t y)
{
    if (x >= mib.Xres) return 0;
    if (y >= mib.Yres) return 0;

    return ((uint16_t*)buffer)[width*y+x];
}
uint8_t HAL::VESA::GetPixel8(int16_t x, int16_t y)
{
    if (x >= mib.Xres) return 0;
    if (y >= mib.Yres) return 0;

    return ((uint8_t*)buffer)[width*y+x];
}
void HAL::VESA::Render()
{
    mem_copy((uint8_t*)buffer, (uint8_t*)mib.physbase, (mib.Yres * mib.pitch));
}
HAL::VESA::VESA():
    vibset(false),
    height(0),
    width(0)
{
}
HAL::VESA::~VESA()
{
    delete buffer;
}