#include "core/kernel.hpp"
#include "hardware/interrupt/isr.hpp"

extern "C" void syscall_event(uint32_t* regs)
{
    System::KernelIO::Kernel.OnSystemCall();
}
extern "C" void kernel_main(void)
{
    System::KernelIO::Kernel.Initialize();
    //We need this until the threadpool has started, DONT REMOVE - unless you have a solution
    while (true); 
}
