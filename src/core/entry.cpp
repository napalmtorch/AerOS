#include "core/kernel.hpp"
#include <hardware/mm/descriptor_tables.hpp>
#include <hardware/mm/kheap.hpp>
#include <boot/mboot.hpp>
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

extern "C" void syscall_event(uint32_t* regs)
{
    System::KernelIO::Kernel.OnSystemCall();
}
extern "C" void kernel_main(multiboot_header_t* mbootdat)
{ 
    InitDescriptorTables();
    // initialize ACPI
    System::KernelIO::ACPI.ACPIInit();
    System::KernelIO::Multiboot.Read((multiboot_t*)mbootdat);
    InitialisePaging();
    System::KernelIO::Kernel.Initialize();
    while (true) { System::KernelIO::Kernel.Run(); }
}
