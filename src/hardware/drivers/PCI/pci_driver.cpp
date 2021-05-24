#include <hardware/ports.hpp>
#include <hardware/drivers/PCI/pci_driver.hpp>
// ------------------------------------------------------------------------------------------------
const PciDriver g_pciDriverTable[] =
{
   // { EthIntelInit },
    { 0 },
};

// ------------------------------------------------------------------------------------------------
uint8_t PciRead8(uint id, uint reg)
{
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, addr);
    return inb(PCI_CONFIG_DATA + (reg & 0x03));
}

// ------------------------------------------------------------------------------------------------
uint16_t PciRead16(uint id, uint reg)
{
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, addr);
    return inw(PCI_CONFIG_DATA + (reg & 0x02));
}

// ------------------------------------------------------------------------------------------------
uint32_t PciRead32(uint id, uint reg)
{
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, addr);
    return inl(PCI_CONFIG_DATA);
}

// ------------------------------------------------------------------------------------------------
void PciWrite8(uint id, uint reg, uint8_t data)
{
    uint32_t address = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, address);
    outb(PCI_CONFIG_DATA + (reg & 0x03), data);
}

// ------------------------------------------------------------------------------------------------
void PciWrite16(uint id, uint reg, uint16_t data)
{
    uint32_t address = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, address);
    outw(PCI_CONFIG_DATA + (reg & 0x02), data);
}

// ------------------------------------------------------------------------------------------------
void PciWrite32(uint id, uint reg, uint32_t data)
{
    uint32_t address = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, address);
    outl(PCI_CONFIG_DATA, data);
}

// ------------------------------------------------------------------------------------------------
static void PciReadBar(uint id, uint index, uint32_t *address, uint32_t *mask)
{
    uint reg = PCI_CONFIG_BAR0 + index * sizeof(uint32_t);

    // Get address
    *address = PciRead32(id, reg);

    // Find out size of the bar
    PciWrite32(id, reg, 0xffffffff);
    *mask = PciRead32(id, reg);

    // Restore adddress
    PciWrite32(id, reg, *address);
}

// ------------------------------------------------------------------------------------------------
void PciGetBar(PciBar *bar, uint id, uint index)
{
    // Read pci bar register
    uint32_t addressLow;
    uint32_t maskLow;
    PciReadBar(id, index, &addressLow, &maskLow);

    if (addressLow & PCI_BAR_64)
    {
        // 64-bit mmio
        uint32_t addressHigh;
        uint32_t maskHigh;
        PciReadBar(id, index + 1, &addressHigh, &maskHigh);

        bar->u.address = (void *)(((uintptr_t)addressHigh << 32) | (addressLow & ~0xf));
        bar->size = ~(((uint64_t)maskHigh << 32) | (maskLow & ~0xf)) + 1;
        bar->flags = addressLow & 0xf;
    }
    else if (addressLow & PCI_BAR_IO)
    {
        // i/o register
        bar->u.port = (uint16_t)(addressLow & ~0x3);
        bar->size = (uint16_t)(~(maskLow & ~0x3) + 1);
        bar->flags = addressLow & 0x3;
    }
    else
    {
        // 32-bit mmio
        bar->u.address = (void *)(uintptr_t)(addressLow & ~0xf);
        bar->size = ~(maskLow & ~0xf) + 1;
        bar->flags = addressLow & 0xf;
    }
}
