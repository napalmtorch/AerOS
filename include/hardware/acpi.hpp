#pragma once


struct LAPIC {
    uint8_t  type;
    uint8_t  length;
    uint8_t  cpu;
    uint8_t  id;
    uint32_t flags;
};
struct IOAPIC {
    uint8_t   type;
    uint8_t   length;
    uint8_t   id;
    uint8_t   reserved;
    uint32_t  addr_base;
    uint32_t  intr_base;
};
struct override_t {
    uint8_t   type;
    uint8_t   length;
    uint8_t   bus_source;
    uint8_t   irq_source;
    uint32_t  global_intr;
    uint16_t  flags;
} __attribute__((packed));
struct nmi_t {
    uint8_t   type;
    uint8_t   length;
    uint8_t   cpu;
    uint16_t  flags;
    uint8_t   lint;
} __attribute__((packed));

extern "C" {

extern LAPIC lapics[256];

int initAcpi();
void acpiPowerOff(void);
}
namespace HAL
{
    struct MADT
    {
        uint8_t numcore=0;          // number of cores detected
        uint32_t lapic_ptr=0;       // pointer to the Local APIC MMIO registers
        uint32_t ioapic_ptr=0;      // pointer to the IO APIC MMIO registers
        uint8_t* lapic_ids = NULL; // CPU core Local APIC IDs
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
