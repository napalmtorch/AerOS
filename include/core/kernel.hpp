#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "lib/vector.hpp"
#include "lib/list.hpp"
#include "core/debug.hpp"
#include "hardware/acpi.hpp"
#include "hardware/cpu.hpp"
#include "hardware/ports.hpp"
#include "hardware/multiboot.hpp"
#include "hardware/terminal.hpp"
#include "hardware/interrupt/idt.hpp"
#include "hardware/interrupt/isr.hpp"
#include "hardware/drivers/peripherals/ps2.hpp"
#include "hardware/cpu.hpp"
#include "hardware/vmm.hpp"
#include "hardware/smbios.hpp"
#include "hardware/drivers/serial.hpp"
#include "hardware/drivers/ata.hpp"
#include "hardware/drivers/rtc.hpp"
#include "hardware/drivers/vga.hpp"
#include "hardware/drivers/PCI/new_pci.hpp"
#include "hardware/drivers/pci.hpp"
#include "hardware/drivers/keyboard.hpp"
#include "hardware/drivers/mouse.hpp"
#include "graphics/font.hpp"
#include "graphics/colors.hpp"
#include "core/shell.hpp"
#include "hardware/fat.hpp"
#include "hardware/nfs.hpp"
#include "gui/xserver.hpp"
#include "hardware/drivers/vesa.hpp"

extern "C" 
{
    extern uint32_t kernel_end;
    extern uint32_t kernel_end_real;
}

// Kernel IO API by napalmtorch and Nik300

void enter_pressed(char* input);


#ifdef __cplusplus
    // main namespace of the System library
    namespace System
    {
        struct StartParameters
        {
            bool Debug;
            bool Serial;
            bool DisableFS;
            bool VGA;
            bool VESA;
            bool SMBIOS; 
        };

        namespace KernelIO
        {
            // kernel core
            class KernelBase
            {
                public:
                    // start parameters
                    StartParameters Parameters;
                    
                    // called as first function before kernel run
                    void Initialize();

                    // start graphics mode
                    void InitializeGUI(VIDEO_DRIVER driver);

                    // parse start parameters
                    void ParseStartParameters();
                    
                    // kernel core code, runs in a loop
                    void Run();
                    
                    // triggered when a kernel panic is injected
                    void OnPanic(char* error);
                    
                    // triggered when a handled interrupt call is finished
                    void OnInterrupt();
                    
                    // triggered when interrupt 0x80 is triggered
                    void OnSystemCall();

                    // triggered when enter key is pressed
                    void OnEnterPressed(char* input);

            };

            // kernel base class
            extern KernelIO::KernelBase Kernel;

            // serial port 
            extern HAL::SerialPort SerialPort;

            // multiboot header
            extern HAL::MultibootHeader Multiboot;

            // pci controller driver
            extern HAL::PCIBusController PCIBus;

            //SMBIOS
            extern BIOS::SMBIOS smbios;

            // memory manager
            extern HAL::MemoryManager MemoryManager;

            // vga graphics driver
            extern HAL::VGADriver VGA;

            // ata controller driver
            extern HAL::ATAController ATA;

            // real time clock
            extern HAL::RTCManager RTC;

            // ps2 keyboard controller driver
            extern HAL::PS2Keyboard Keyboard;

            // ps2 mouse controller driver
            extern HAL::PS2Mouse Mouse;

            // terminal interface
            extern HAL::TerminalManager Terminal;

            // shell
            extern System::ShellHost Shell;

            // window server
            extern System::GUI::XServerHost XServer;

            // acpi
            extern HAL::ACPI ACPI;

            // vmm
            extern HAL::VMM VMM;
            
            // vesa
            extern HAL::VESA VESA;

            // napalm file system
            extern HAL::NapalmFileSystem NapalmFS;
        }
    }
#else
    
#endif