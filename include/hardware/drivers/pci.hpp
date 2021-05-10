#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/ports.hpp"
#include "hardware/memory.hpp"

namespace HAL
{
    struct _PCIDriver;

    typedef struct
    {
        uint32_t Vendor;
        uint32_t Device;
        uint32_t Function;
        struct _PCIDriver* Driver;
    } PCIDevice;
    
    typedef struct
    {
        uint32_t Vendor;
        uint32_t Device;
        uint32_t Function;
    } PCIDeviceID;
    
    typedef struct _PCIDriver
    {
        PCIDeviceID* Table;
        char *Name;
        uint8_t (*init_one)(PCIDevice*);
        uint8_t (*init_driver)(void);
        uint8_t (*exit_driver)(void);
    } PCIDriver;

    typedef enum
    {
        PCI_VENDOR_INTEL                = 0x8086,
        PCI_VENDOR_INTEL_ALT            = 0x8087,
        PCI_VENDOR_VMWARE               = 0x15AD,
        PCI_VENDOR_VMWARE_ALT           = 0x0E0F,
        PCI_VENDOR_DELL                 = 0x1028,
        PCI_VENDOR_DELL_ALT             = 0x413C,
        PCI_VENDOR_ATI                  = 0x1002,
        PCI_VENDOR_AMD                  = 0x1022,
        PCI_VENDOR_VIA                  = 0x2109,
        PCI_VENDOR_BENQ                 = 0x04A5,
        PCI_VENDOR_ACER                 = 0x5986,
        PCI_VENDOR_HP                   = 0x03F0,
        PCI_VENDOR_HP_ALT               = 0x103C,
        PCI_VENDOR_HP_ENT               = 0x1590,
        PCI_VENDOR_ASUS                 = 0x1043,
        PCI_VENDOR_ASUS_ALT             = 0x0B05,
        PCI_VENDOR_INNOTEK              = 0x80EE,
        PCI_VENDOR_REALTEK              = 0x10EC,
        PCI_VENDOR_REAKLTEK_ALT         = 0x0BDA,
        PCI_VENDOR_ENSONIQ              = 0x1274,
        PCI_VENDOR_BROADCOM             = 0x14E4,
    } PCI_VENDOR;

    class PCIBusController
    {
        public:
            void Initialize();
            void Probe();
            void List();
            void AddDevice(PCIDevice* device);
            char* GetVendorName(uint16_t vendor, uint16_t id);
            uint16_t ReadWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset);
            void WriteWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t data);
            uint16_t GetVendorID(uint16_t bus, uint16_t device, uint16_t func);
            uint16_t GetDeviceID(uint16_t bus, uint16_t device, uint16_t function);
            uint16_t GetClassID(uint16_t bus, uint16_t device, uint16_t function);
            uint16_t GetSubClassID(uint16_t bus, uint16_t device, uint16_t function);
        private:
            // devices
            PCIDevice **Devices = nullptr;
            uint32_t DeviceCount = 0;
    };
}