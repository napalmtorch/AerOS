#pragma once

extern "C" {

int initAcpi();
void acpiPowerOff(void);
}
namespace HAL
{
    struct MADT
    {
        uint8_t lapic_ids[256]={0}; // CPU core Local APIC IDs
        uint8_t numcore=0;          // number of cores detected
        uint32_t lapic_ptr=0;       // pointer to the Local APIC MMIO registers
        uint32_t ioapic_ptr=0;      // pointer to the IO APIC MMIO registers
    };
    class ACPI
    {
        public:
        int ACPIInit();
        void Shutdown();
        void LegacyShutdown();
        void Reboot();
        MADT* GetAPICInfo();
    };
}
