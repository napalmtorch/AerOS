#include "core/kernel.hpp"
#include <hardware/mm/descriptor_tables.hpp>
#include <hardware/mm/kheap.hpp>
#include <hardware/mm/memory_page.hpp>
#include <boot/mboot.hpp>
#include <lib/types.h>
// terminal interface
HAL::TerminalManager Terminal;
        // ps2 keyboard controller driver
        HAL::PS2Keyboard Keyboard;
        HAL::PS2Mouse Mouse;
static void pit_callback(RegistersType regs)
{
    System::KernelIO::Kernel.OnInterrupt();
    UNUSED(regs);
}
extern "C" void ClearScreen()
{
	memset((void*)0xb8000,0,80*25*2);
}

extern "C" void syscall_event(uint32_t* regs)
{
    System::KernelIO::Kernel.OnSystemCall();
}
extern "C" void kernel_main(multiboot_header_t* mbootdat)
{ 
    serial_set_port(SERIAL_PORT_COM1);
    InitDescriptorTables();
    ClearScreen();
    // initialize ACPI
  //  System::KernelIO::VESA.PopulateInfoBlock();
    //System::KernelIO::VESA.SetMode(640, 480, 32);
    //System::KernelIO::ACPI.ACPIInit();
    System::KernelIO::Multiboot.Read((multiboot_t*)mbootdat);
    debug_write_ext("Vesa 1: ",COL4_GREEN);
    debug_writeln_dec("VBE Mode:",System::KernelIO::Multiboot.GetVBEModeInfo());

    System::KernelIO::Mouse.Initialize();
    System::KernelIO::Keyboard.Initialize();
    System::KernelIO::Kernel.Initialize();
    while (true) { System::KernelIO::Kernel.Run(); }
}
