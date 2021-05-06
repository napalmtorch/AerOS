#pragma once
#include "lib/types.h"
#include "core/debug.hpp"
#include "hardware/ports.hpp"
#include "hardware/multiboot.hpp"
#include "hardware/terminal.hpp"
#include "hardware/fat.hpp"
#include "hardware/interrupt/idt.hpp"
#include "hardware/interrupt/isr.hpp"
#include "hardware/interrupt/interrupt.hpp"
#include "hardware/drivers/serial.hpp"
#include "hardware/drivers/ata.hpp"
#include "hardware/drivers/rtc.hpp"
#include "hardware/drivers/vga.hpp"
#include "hardware/drivers/pci.hpp"
#include "hardware/drivers/keyboard.hpp"
#include "graphics/font.hpp"
#include "graphics/colors.hpp"

// Kernel IO API by napalmtorch and Nik300

#ifdef __cplusplus
    // main namespace of the System library
    namespace System
    {
        namespace KernelIO
        {
            // kernel core
            class KernelBase
            {
                public:
                    // called as first function before kernel run
                    void Initialize();
                    
                    // kernel core code, runs in a loop
                    void Run();
                    
                    // triggered when a kernel panic is injected
                    void OnPanic();
                    
                    // triggered when a handled interrupt call is finished
                    void OnInterrupt();
                    
                    // triggered when interrupt 0x80 is triggered
                    void OnSystemCall();

                    // triggered when enter key is pressed
                    void OnEnterPressed(char* input);

                    // register command
                    void HandleCommand(char* input);

            };

            // kernel base class
            extern KernelIO::KernelBase Kernel;

            // serial port 
            extern HAL::SerialPort SerialPort;

            // multiboot header
            extern HAL::MultibootHeader Multiboot;

            // pci controller driver
            extern HAL::PCIBusController PCIBus;

            // memory manager
            extern HAL::MemoryManager MemoryManager;

            // vga graphics driver
            extern HAL::VGADriver VGA;

            // ata controller driver
            extern HAL::ATAController ATA;

            // file system
            extern HAL::FATFileSystem FAT;

            // real time clock
            extern HAL::RTCManager RTC;

            // ps2 keyboard controller driver
            extern HAL::PS2Keyboard Keyboard;

            // terminal interface
            extern HAL::TerminalManager Terminal;
        }
    }
#else
    
#endif