#include <hardware/drivers/vesa.hpp>
#include <boot/realmode.hpp>
#include <core/debug.hpp>

void HAL::VESA::populate_vib()
{
    if (vibset) return;
    System::KernelIO::ThrowMessage(MSG_TYPE_SYSTEM, "");
}