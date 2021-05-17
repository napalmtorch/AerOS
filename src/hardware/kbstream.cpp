#include "hardware/kbstream.hpp"
#include <core/kernel.hpp>

namespace HAL
{
    // constructor - default
    KeyboardStream::KeyboardStream()  { Enabled = true; KeyDown = false; }

    KeyboardStream::KeyboardStream(void* window)
    {
        Enabled = true;
        KeyDown = false;
        Window = window;
    }

    // update
    void KeyboardStream::Update()
    {
        if (!Enabled && System::KernelIO::Keyboard.Buffer == Text) { System::KernelIO::Keyboard.Buffer = nullptr; }

        // skip if null or disabled
        if (Text == nullptr || !Enabled) { return; }

        // set buffer pointer
        if (System::KernelIO::Keyboard.Buffer != Text) { System::KernelIO::Keyboard.Buffer = Text; }

        if (Window != nullptr)
        {
            System::GUI::Window* window = ((System::GUI::Window*)Window);
            if (!window->Flags->Active && System::KernelIO::Keyboard.Buffer == Text) { System::KernelIO::Keyboard.Buffer = nullptr; }
        }
    }
}