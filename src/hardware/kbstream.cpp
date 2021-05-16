#include "hardware/kbstream.hpp"
#include <core/kernel.hpp>

namespace HAL
{
    // constructor - default
    KeyboardStream::KeyboardStream()  { Enabled = true; }

    // update
    void KeyboardStream::Update()
    {
        if (Text == nullptr || !Enabled) { return; }
        System::KernelIO::Keyboard.Buffer = Text;
    }
}