#include "hardware/kbstream.hpp"
#include <core/kernel.hpp>

namespace HAL
{
    // constructor - default
    KeyboardStream::KeyboardStream()  { Enabled = true; KeyDown = false; }

    // update
    void KeyboardStream::Update()
    {
        if (Text == nullptr || !Enabled) { return; }

        if (!KeyDown && System::KernelIO::Keyboard.IsAnyKeyDown())
        {
            uint8_t code = System::KernelIO::Keyboard.GetScanCode();

            if (System::KernelIO::Keyboard.IsKeyUp(HAL::Keys::LSHIFT) && System::KernelIO::Keyboard.IsKeyUp(HAL::Keys::RSHIFT)) { ShiftDown = false; }

            // backspace
            if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::BACKSPACE))
            {
                if (strlen(Text) > 0)
                {
                    strdel(Text);
                }
            }
            // handle shift
            else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::LSHIFT) || System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::RSHIFT)) { ShiftDown = true; }
            // handle caps lock
            else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::CAPSLOCK)) { CapsLockDown = !CapsLockDown; }
            // handle enter
            else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::ENTER))
            {
                stradd(Text, '\n');
            }
            else
            {
                char letter = 0;
                bool caps = ((ShiftDown && !CapsLockDown) || (!ShiftDown && CapsLockDown));

                // symbols
                if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::BACKTICK))           { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::MINUS))         { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::EQUALS))        { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::LBRACKET))      { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::RBRACKET))      { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::COLON))         { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::QUOTE))         { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::BACKSLASH))     { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::COMMA))         { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::PERIOD))        { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::SLASH))         { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                // numerals
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D1)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D2)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D3)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D4)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D5)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D6)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D7)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D8)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D9)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D0)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                // characters
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::A)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::B)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::C)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::D)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::E)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::F)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::G)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::H)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::I)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::J)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::K)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::L)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::M)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::N)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::O)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::P)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::Q)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::R)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::S)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::T)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::U)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::V)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::W)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::X)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::Y)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::Z)) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
                // space
                else if (System::KernelIO::Keyboard.IsKeyDown(HAL::Keys::SPACE)) { letter = ' '; }

                // print character
                if (letter > 0)
                {
                    stradd(Text, letter);
                }
            }

            KeyDown = true;
        }

        if (System::KernelIO::Keyboard.IsAllKeysUp()) { KeyDown = false; }
    }
}