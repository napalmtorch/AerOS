// ------------------------------------------------------------------------------------------------
// pci/driver.h
// ------------------------------------------------------------------------------------------------

#pragma once

#include <lib/types.h>

// ------------------------------------------------------------------------------------------------
#define PCI_MAKE_ID(bus, dev, func)     ((bus) << 16) | ((dev) << 11) | ((func) << 8)

// I/O Ports
#define PCI_CONFIG_ADDR                 0xcf8
#define PCI_CONFIG_DATA                 0xcfC

// Header Type
#define PCI_TYPE_MULTIFUNC              0x80
#define PCI_TYPE_GENERIC                0x00
#define PCI_TYPE_PCI_BRIDGE             0x01
#define PCI_TYPE_CARDBUS_BRIDGE         0x02

// PCI Configuration Registers
#define PCI_CONFIG_VENDOR_ID            0x00
#define PCI_CONFIG_DEVICE_ID            0x02
#define PCI_CONFIG_COMMAND              0x04
#define PCI_CONFIG_STATUS               0x06
#define PCI_CONFIG_REVISION_ID          0x08
#define PCI_CONFIG_PROG_INTF            0x09
#define PCI_CONFIG_SUBCLASS             0x0a
#define PCI_CONFIG_CLASS_CODE           0x0b
#define PCI_CONFIG_CACHELINE_SIZE       0x0c
#define PCI_CONFIG_LATENCY              0x0d
#define PCI_CONFIG_HEADER_TYPE          0x0e
#define PCI_CONFIG_BIST                 0x0f

// Type 0x00 (Generic) Configuration Registers
#define PCI_CONFIG_BAR0                 0x10
#define PCI_CONFIG_BAR1                 0x14
#define PCI_CONFIG_BAR2                 0x18
#define PCI_CONFIG_BAR3                 0x1c
#define PCI_CONFIG_BAR4                 0x20
#define PCI_CONFIG_BAR5                 0x24
#define PCI_CONFIG_CARDBUS_CIS          0x28
#define PCI_CONFIG_SUBSYSTEM_VENDOR_ID  0x2c
#define PCI_CONFIG_SUBSYSTEM_DEVICE_ID  0x2e
#define PCI_CONFIG_EXPANSION_ROM        0x30
#define PCI_CONFIG_CAPABILITIES         0x34
#define PCI_CONFIG_INTERRUPT_LINE       0x3c
#define PCI_CONFIG_INTERRUPT_PIN        0x3d
#define PCI_CONFIG_MIN_GRANT            0x3e
#define PCI_CONFIG_MAX_LATENCY          0x3f

// ------------------------------------------------------------------------------------------------
// PCI BAR

#define PCI_BAR_IO                      0x01
#define PCI_BAR_LOWMEM                  0x02
#define PCI_BAR_64                      0x04
#define PCI_BAR_PREFETCH                0x08

typedef struct PciBar
{
    union
    {
        void *address;
        uint16_t port;
    } u;
    uint64_t size;
    uint flags;
} PciBar;

// ------------------------------------------------------------------------------------------------
typedef struct PciDeviceInfo
{
    uint16_t vendorId;
    uint16_t deviceId;
    uint8_t classCode;
    uint8_t subclass;
    uint8_t progIntf;
} PciDeviceInfo;

typedef struct PciDriver
{
    void (*init)(uint id, PciDeviceInfo *info);
} PciDriver;

// ------------------------------------------------------------------------------------------------
extern const PciDriver g_pciDriverTable[];

uint8_t PciRead8(uint id, uint reg);
uint16_t PciRead16(uint id, uint reg);
uint32_t PciRead32(uint id, uint reg);

void PciWrite8(uint id, uint reg, uint8_t data);
void PciWrite16(uint id, uint reg, uint16_t data);
void PciWrite32(uint id, uint reg, uint32_t data);

void PciGetBar(PciBar *bar, uint id, uint index);
