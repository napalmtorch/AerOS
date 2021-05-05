#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/ports.hpp"

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

    enum PCIVendors
    {
        Intel               = 0x8086,
        IntelCorp           = 0x8087,
        VMWare              = 0x15AD,
        VMWareInc           = 0x0E0F,
        Dell                = 0x1028,
        DellCorporation     = 0x413C,
        ATI                 = 0x1002,
        AMD                 = 0x1022,
        VIALabs             = 0x2109,
        BenQ                = 0x04A5,
        AcerInc             = 0x5986,
        HPInc               = 0x03F0,
        HPCompany           = 0x103C,
        HPEnterprise        = 0x1590,
        ASUS                = 0x1043,
        ASUSInc             = 0x0B05,
        InnoTek             = 0x80EE,
        Realtek             = 0x10EC,
        RealtekCorp         = 0x0BDA,
    };

    class PCIBusController
    {
        public:
            void Initialize();
            void Probe();
            void AddDevice(PCIDevice device);
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