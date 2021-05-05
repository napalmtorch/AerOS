#include "hardware/drivers/ata.hpp"

#define STATUS_BUSY     0x80
#define STATUS_READY    0x40
#define STATUS_DRQ      0x08
#define STATUS_DF       0x20
#define STATUS_ERROR    0x01

namespace HAL
{
    // initialize ata controller driver
    void ATAController::Initialize()
    {

    }

    // read specified amount of sectors to pointer
    void ATAController::ReadSectors(uint8_t* dest, uint32_t lba, uint8_t sectors)
    {
        WaitBusy();
        outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
        outb(0x1F2, sectors);
        outb(0x1F3, (uint8_t)lba);
        outb(0x1F4, (uint8_t)(lba >> 8));
        outb(0x1F5, (uint8_t)(lba >> 16));
        outb(0x1F7, 0x20);

        uint16_t* ptr = (uint16_t*)dest;
        for (size_t i = 0; i < sectors; i++)
        {
            WaitBusy();
            WaitDRQ();
            for (size_t j = 0; j < 256; j++) { ptr[j] = inw(0x1F0); }
            ptr += 256;
        }
    }

    // write data from pointer to sectors
    void ATAController::WriteSectors(uint32_t* src, uint32_t lba, uint8_t sectors)
    {
        WaitBusy();
        outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
        outb(0x1F2, sectors);
        outb(0x1F3, (uint8_t)lba);
        outb(0x1F4, (uint8_t)(lba >> 8));
        outb(0x1F5, (uint8_t)(lba >> 16));
        outb(0x1F7, 0x30);

        for (size_t i = 0; i < sectors; i++)
        {
            WaitBusy();
            WaitDRQ();
            for (size_t j = 0; j < 256; j++) { outl(0x1F0, src[j]); }
        }
    }

    // wait if ata controller is busy
    void ATAController::WaitBusy() { while (inb(0x1F7) & STATUS_BUSY); }

    // wait until ata controller is ready
    void ATAController::WaitDRQ() { while (!(inb(0x1F7) & STATUS_READY)); }

    // check if an error has occurred
    uint8_t ATAController::HasErrorOccurred() { return (uint8_t)(inb(0x1F7) & STATUS_ERROR); }
}