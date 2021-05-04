#include "core/kernel.hpp"

extern "C" void kernel_main(void)
{
    System::Kernel::Initialize();
    while (1) { System::Kernel::Run(); }
}