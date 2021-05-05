#include "hardware/drivers/pci.hpp"

namespace HAL
{
    void PCIBusController::Initialize()
    {
        
    }

    void PCIBusController::Probe()
    {

    }

    void PCIBusController::AddDevice(PCIDevice device)
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