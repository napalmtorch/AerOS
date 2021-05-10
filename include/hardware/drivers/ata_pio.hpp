
#pragma once
extern "C"
{
#include <lib/types.h>

uint8_t identify();
void ata_pio_read28(uint32_t LBA, uint8_t sectorcount, uint8_t *target);
void ata_pio_read48(uint64_t LBA, uint16_t sectorcount, uint8_t *target);
void ata_pio_write48(uint64_t LBA, uint16_t sectorcount, uint8_t *target);
}