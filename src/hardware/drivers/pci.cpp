#include "hardware/drivers/pci.hpp"
#include "core/kernel.hpp"

namespace HAL
{
    // initialize pci bus controller driver
    void PCIBusController::Initialize()
    {
        // reset device count
        DeviceCount = 0;

        // initialize device list
        Devices = (PCIDevice**)mem_alloc(32 * sizeof(PCIDevice));

        // initialization mesasge
        System::KernelIO::ThrowOK("Initialized PCI bus controller");

        // probe
        Probe();
    }

    void PCIBusController::Probe()
    {
        char temp[16];
            for (uint32_t bus = 0; bus < 256; bus++)
            {
                for (uint32_t slot = 0; slot < 32; slot++)
                {
                    for (uint32_t function = 0; function < 8; function++)
                    {
                        uint16_t vendor = GetVendorID(bus, slot, function);
                        if (vendor == 0xFFFF) { continue; }
                        uint16_t id = GetDeviceID(bus, slot, function);
                        uint16_t classID = GetClassID(bus, slot, function);

                        System::KernelIO::Write(" - ");
                        System::KernelIO::Write("DEVICE: ", COL4_MAGENTA);
                        System::KernelIO::WriteLine(GetVendorName(vendor, id));

                        PCIDevice* device = (PCIDevice*)System::KernelIO::MemoryManager.Allocate(sizeof(PCIDevice));
                        device->Vendor = vendor;
                        device->Device = id;
                        device->Function = function;
                        device->Driver = 0;
                        AddDevice(device);
                    }
                }
            }
    }

    void PCIBusController::AddDevice(PCIDevice* device)
    {

    }

    char* PCIBusController::GetVendorName(uint16_t vendor, uint16_t id)
    {
        
    }
    
    uint16_t PCIBusController::ReadWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
    {

    }

    void PCIBusController::WriteWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t data)
    {
    
    }

    uint16_t PCIBusController::GetVendorID(uint16_t bus, uint16_t device, uint16_t func)
    {

    }

    uint16_t PCIBusController::GetDeviceID(uint16_t bus, uint16_t device, uint16_t function)
    {

    }

    uint16_t PCIBusController::GetClassID(uint16_t bus, uint16_t device, uint16_t function)
    {

    }

    uint16_t PCIBusController::GetSubClassID(uint16_t bus, uint16_t device, uint16_t function)
    {

    }
}