#include "core/kernel.hpp"
#include <hardware/mm/descriptor_tables.hpp>

extern "C" void syscall_event(uint32_t* regs)
{
    System::KernelIO::Kernel.OnSystemCall();
}
extern "C" void kernel_main(void)
{   InitDescriptorTables();
    System::KernelIO::Kernel.Initialize();
    while (true) { System::KernelIO::Kernel.Run(); }
}
