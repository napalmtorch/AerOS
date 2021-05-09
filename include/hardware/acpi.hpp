#pragma once

extern "C" {

int initAcpi();
void acpiPowerOff(void);
}
namespace HAL
{
    class ACPI
    {
        public:
        int ACPIInit();
        void Shutdown();
        void LegacyShutdown();
        void Reboot();
    };
}
