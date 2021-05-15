#include "hardware/kbstream.hpp"
#include <core/kernel.hpp>

namespace HAL
{
    // constructor - default
    KeyboardStream::KeyboardStream() { }

    // constructor with pointer
    KeyboardStream::KeyboardStream(char* output) : Text(output) { Enabled = true; }

    // update
    void KeyboardStream::Update()
    {
        /*
        if (Text == nullptr || !Enabled) { return; }

        // backspace
        if (System::KernelIO::Keyboard.GetScanCode() == (uint8_t)HAL::Keys::BACKSPACE)
        {
            if (strlen(Text) > 0)
            {
                strdel(Text);
            }
        }
        // handle shift
        else if (ScanCode == (uint8_t)HAL::Keys::LSHIFT || ScanCode == (uint8_t)HAL::Keys::RSHIFT) { ShiftDown = true; }
        else if (ScanCode == 0xAA || ScanCode == 0xB6) { ShiftDown = false; }
        // handle caps lock
        else if (ScanCode == (uint8_t)HAL::Keys::CAPSLOCK) { CapsLockDown = !CapsLockDown; }
        // handle enter
        else if (ScanCode == (uint8_t)HAL::Keys::ENTER)
        {
            stradd(Text)
        }
        // handle characters
        else
        {
            char letter = 0;
            bool caps = ((ShiftDown && !CapsLockDown) || (!ShiftDown && CapsLockDown));
        }
        */
    }
}