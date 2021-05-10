#include "hardware/drivers/pci.hpp"
#include "core/kernel.hpp"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

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
    bool initialized = false;
    void PCIBusController::Probe()
    {
        if (initialized) return;
        initialized = true;
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
                    
                    // print id - used to google the actual name xD 
                    // System::KernelIO::WriteLineHex("", id);

                    PCIDevice* device = new PCIDevice();
                    device->Vendor = vendor;
                    device->Device = id;
                    device->Function = function;
                    device->Driver = 0;
                    AddDevice(device);
                }
            }
        }
    }

    void PCIBusController::List()
    {
        System::KernelIO::Terminal.Write("\n");
        System::KernelIO::Terminal.WriteLine("------- PCI Devices -------", COL4_GREEN);
        System::KernelIO::Terminal.Write("\n");
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
                    
                    System::KernelIO::Terminal.Write("- ");
                    char vendorbuf[32];
                    strhex(vendor,vendorbuf);
                    char vendoridbuf[32];
                    strhex(id,vendoridbuf);
                    
                    System::KernelIO::Terminal.Write("DEVICE: ", COL4_MAGENTA);
                    System::KernelIO::Terminal.Write("0x");
                    System::KernelIO::Terminal.Write(vendorbuf);
                    System::KernelIO::Terminal.Write(":0x");
                    System::KernelIO::Terminal.Write(vendoridbuf);
                    System::KernelIO::Terminal.Write(" Name: ", COL4_CYAN);
                    System::KernelIO::Terminal.WriteLine(GetVendorName(vendor, id));
                }
            }
        }
        System::KernelIO::Terminal.Write("\n");
    }

    void PCIBusController::AddDevice(PCIDevice* device)
    {
        Devices[DeviceCount] = device;
        DeviceCount++;
    }
    
    uint16_t PCIBusController::ReadWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
    {
        uint32_t address;
        uint32_t lbus = (uint32_t)bus;
        uint32_t lslot = (uint32_t)slot;
        uint32_t lfunc = (uint32_t)func;
        uint16_t temp = 0;
        address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
        outl(PCI_CONFIG_ADDRESS, address);
        temp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
        return (temp);
    }

    void PCIBusController::WriteWord(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t data)
    {
        uint32_t address = (uint32_t)((bus << 16) | (slot << 1) | (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
        outl(PCI_CONFIG_ADDRESS, address);
        outl(PCI_CONFIG_DATA, data);
    }

    uint16_t PCIBusController::GetVendorID(uint16_t bus, uint16_t device, uint16_t function)
    {
        uint16_t r0 = ReadWord(bus, device, function, 0x0);
        return r0;
    }

    uint16_t PCIBusController::GetDeviceID(uint16_t bus, uint16_t device, uint16_t function)
    {
        uint32_t r0 = ReadWord(bus, device, function, 0x2);
        return r0;
    }

    uint16_t PCIBusController::GetClassID(uint16_t bus, uint16_t device, uint16_t function)
    {
        uint32_t r0 = ReadWord(bus, device, function, 0xA);
        return r0;
    }

    uint16_t PCIBusController::GetSubClassID(uint16_t bus, uint16_t device, uint16_t function)
    {
        uint32_t r0 = ReadWord(bus, device, function, 0xA);
        return (r0 & ~0xFF00);
    }

    char* PCIBusController::GetVendorName(uint16_t vendor, uint16_t id)
    {
        // intel devices
        if (vendor == PCI_VENDOR_INTEL)
        {
            switch (id)
            {
                case 0x1237: { return "Intel 440FX Chipset"; }
                case 0x7000: { return "Intel 82371SB PIIX3 ISA"; }
                case 0x7010: { return "Intel 82371SB PIIX3 IDE"; }
                case 0x7020: { return "Intel 82371SB PIIX3 USB"; }
                case 0x7111: { return "Intel 82371AB/EB/MB PIIX4 IDE"; }
                case 0x7113: { return "Intel 82371AB/EB/MB PIIX4 ACPI"; }
                case 0x100E: { return "Intel 82540EM Ethernet Controller"; }
                case 0x0041: { return "Intel Core PCI Express x16 Root Port"; }
                case 0x0042: { return "Intel Core IGPU Controller"; }
                case 0x0044: { return "Intel Core DRAM Controller"; }
                case 0x0600: { return "Intel RAID Controller"; }
                case 0x061F: { return "Intel 80303 I/O Processor"; }
                case 0x2415: { return "Intel 82801AA AC'97 Audio"; }
                case 0x2668: { return "Intel 82801(ICH6 Family) HD Audio"; }
                case 0x2770: { return "Intel 82945G Memory Controller Hub"; }
                case 0x2771: { return "Intel 82945G PCI Express Root Port"; }
                case 0x2772: { return "Intel 82945G Integrated Graphics"; }
                case 0x2776: { return "Intel 82945G Integrated Graphics"; }
                case 0x27D0: { return "Intel NM10/ICH7 PCI Express Port 1"; }
                case 0x27D2: { return "Intel NM10/ICH7 PCI Express Port 2"; }
                case 0x27C8: { return "Intel NM10/ICH7 USB UHCI Controller 1"; }
                case 0x27C9: { return "Intel NM10/ICH7 USB UHCI Controller 2"; }
                case 0x27CA: { return "Intel NM10/ICH7 USB UHCI Controller 3"; }
                case 0x27CB: { return "Intel NM10/ICH7 USB UHCI Controller 4"; }
                case 0x27CC: { return "Intel NM10/ICH7 USB2 EHCI Controller"; }
                case 0x244E: { return "Intel 82801 PCI Bridge"; }
                case 0x27DE: { return "Intel 82801G/ICH7 AC'97 Audio"; }
                case 0x27B8: { return "Intel 82801GB/ICH7 LPC Interface Bridge"; }
                case 0x27C0: { return "Intel NM10/ICH7 SATA Controller(IDE MODE)"; }
                case 0x27DA: { return "Intel NM10/ICH7 SMBus Controller"; }
                case 0x0F00: { return "Intel Atom Z36xxx/Z37xxx SoC Trans-Reg"; }
                case 0x0F31: { return "Intel Atom Z36xxx/Z37xxx Series Graphics"; }
                case 0x0F23: { return "Intel Atom E3800 Series SATA AHCI"; }
                case 0x0F18: { return "Intel Atom Z36xxx/Z37xxx Trusted Exec."; }
                case 0x0F04: { return "Intel Atom Z36xxx/Z37xxx HD Audio"; }
                case 0x0F48: { return "Intel Atom E3800 Series PCI Express Port 1"; }
                case 0x0F4A: { return "Intel Atom E3800 Series PCI Express Port 2"; }
                case 0x0F34: { return "Intel Atom Z36xxx/Z37xxx USB EHCI"; }
                case 0x0F1C: { return "Intel Atom Z36xxx/Z37xxx Series PCU"; }
                case 0x0F12: { return "Intel Atom E3800 Series SMBus Controller"; }
                default:     { return "Unrecognized Intel Device"; }
            }
        }

        // newer intel devices
        if (vendor == PCI_VENDOR_INTEL_ALT)
        {
            switch (id)
            {
                default: { return "Unrecognized Intel Device"; }
            }
        }

        // vmware
        if (vendor == PCI_VENDOR_VMWARE)
        {
            switch (id)
            {
                case 0x0405: { return "VMWare SVGAII Adapter"; }
                case 0x0710: { return "VMWare SVGA Adapter"; }
                case 0x0770: { return "VMWare USB2 EHCI Controller"; }
                case 0x0720: { return "VMWare VMXNET Ethernet Controller"; }
                case 0x0740: { return "VMWare VM Communication Interface"; }
                case 0x0774: { return "VMWare USB1.1 UHCI Controller"; }
                case 0x0778: { return "VMWare USB3 xHCI 0.96 Controller"; }
                case 0x0779: { return "VMWare USB3 xHCI 1.00 Controller"; }
                case 0x0790: { return "VMWare PCI Bridge"; }
                case 0x07A0: { return "VMWare PCI Express Root Port"; }
                case 0x07B0: { return "VMWare VMXNET3 Ethernet Controller"; }
                case 0x07C0: { return "VMWare PVSCSI SCSI Controller"; }
                case 0x07E0: { return "VMWare SATA AHCI Controller"; }
                case 0x0801: { return "VMWare VM Interface"; }
                case 0x0820: { return "VMWare Paravirtual RDMA Controller"; }
                case 0x0800: { return "VMWare Hypervisor ROM Interface"; }
                case 0x1977: { return "VMWare HD Audio Controller"; }
                default: { return "Unrecognized VMWare Device"; }
            }
        }

        // vmware
        if (vendor == PCI_VENDOR_VMWARE_ALT)
        {
            switch (id)
            {
                default: { return "Unrecognized VMWare Device"; }
            }
        }

        // innotek
        if (vendor == PCI_VENDOR_INNOTEK)
        {
            switch (id)
            {
                case 0xBEEF: { return "VirtualBox Graphics Adapter"; }
                case 0xCAFE: { return "VirtualBox Guest Service"; }
                default: { return "Unrecognized InnoTek Device"; }
            }
        }

        // amd
        if (vendor == PCI_VENDOR_AMD)
        {
            switch (id)
            {
                case 0x2000: { return "AMD 79C970 Ethernet Controller"; }
                default: { return "Unrecognized AMD Device"; }
            }
        }

        // ensoniq
        if (vendor == PCI_VENDOR_ENSONIQ)
        {
            switch (id)
            {
                case 0x5000: { return "Ensoniq ES1370 [AudioPCI]"; }
                default: { return "Unrecognized Ensoniq Device"; }
            }
        }

        // broadcom
        if (vendor == PCI_VENDOR_BROADCOM)
        {
            switch (id)
            {
                case 0x1677: { return "NetXtreme BCM5751 Ethernet PCI-E"; }
                default: { return "Unrecognized Realtek Device"; }
            }
        }

        // realtek
        if (vendor == PCI_VENDOR_REALTEK)
        {
            switch (id)
            {
                case 0x5289: { return "Realtek RTL8411 PCI Express Card Reader"; }
                case 0x8168: { return "Realtek RTL8111/8168/8411 Ethernet PCI-E"; }
                default: { return "Unrecognized Realtek Device"; }
            }
        }

        // atheros
        if (vendor == PCI_VENDOR_ATHEROS)
        {
            switch (id)
            {
                case 0x0036: { return "Atheros AR9485 WiFi Adapter"; }
                default: { "Unrecognized Atheros Device"; }
            }
        }

        // qemu graphics controller
        if (vendor == 0x1234 && id == 0x1111) { return "QEMU VGA Controller"; }

        // unknown
        return "Unrecognized Device";
    }
}