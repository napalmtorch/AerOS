#include "core/kernel.hpp"

extern "C" void kernel_main(void)
{
    System::KernelIO::Kernel.Initialize();
    while (1) { System::KernelIO::Kernel.Run(); }
}

