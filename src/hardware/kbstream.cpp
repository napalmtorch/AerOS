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
        if (Text == nullptr || !Enabled) { return; }

        uint8_t ScanCode = System::KernelIO::Keyboard.GetScanCode();

        // backspace
        if (ScanCode == (uint8_t)HAL::Keys::BACKSPACE)
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
            stradd(Text, '\n');
        }
        // handle characters
        else
        {
            char letter = 0;
            bool caps = ((ShiftDown && !CapsLockDown) || (!ShiftDown && CapsLockDown));

            // symbols
            if (ScanCode == (uint8_t)HAL::Keys::BACKTICK)       { if (caps) { letter = '~'; } else { letter = '`'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::MINUS)     { if (caps) { letter = '_'; } else { letter = '-'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::EQUALS)    { if (caps) { letter = '+'; } else { letter = '='; } }
            else if (ScanCode == (uint8_t)HAL::Keys::LBRACKET)  { if (caps) { letter = '{'; } else { letter = '['; } }
            else if (ScanCode == (uint8_t)HAL::Keys::RBRACKET)  { if (caps) { letter = '}'; } else { letter = ']'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::COLON)     { if (caps) { letter = ':'; } else { letter = ';'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::QUOTE)     { if (caps) { letter = '"'; } else { letter = '\''; } }
            else if (ScanCode == (uint8_t)HAL::Keys::BACKSLASH) { if (caps) { letter = '|'; } else { letter = '\\'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::COMMA)     { if (caps) { letter = '<'; } else { letter = ','; } }
            else if (ScanCode == (uint8_t)HAL::Keys::PERIOD)    { if (caps) { letter = '>'; } else { letter = '.'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::SLASH)     { if (caps) { letter = '?'; } else { letter = '/'; } }
            // numerals
            else if (ScanCode == (uint8_t)HAL::Keys::D1) { if (caps) { letter = '!'; } else { letter = '1'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D2) { if (caps) { letter = '@'; } else { letter = '2'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D3) { if (caps) { letter = '#'; } else { letter = '3'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D4) { if (caps) { letter = '$'; } else { letter = '4'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D5) { if (caps) { letter = '%'; } else { letter = '5'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D6) { if (caps) { letter = '^'; } else { letter = '6'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D7) { if (caps) { letter = '&'; } else { letter = '7'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D8) { if (caps) { letter = '*'; } else { letter = '8'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D9) { if (caps) { letter = '('; } else { letter = '9'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D0) { if (caps) { letter = ')'; } else { letter = '0'; } }
            // characters
            else if (ScanCode == (uint8_t)HAL::Keys::A) { if (caps) { letter = 'A'; } else { letter = 'a'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::B) { if (caps) { letter = 'B'; } else { letter = 'b'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::C) { if (caps) { letter = 'C'; } else { letter = 'c'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D) { if (caps) { letter = 'D'; } else { letter = 'd'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::E) { if (caps) { letter = 'E'; } else { letter = 'e'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::F) { if (caps) { letter = 'F'; } else { letter = 'f'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::G) { if (caps) { letter = 'G'; } else { letter = 'g'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::H) { if (caps) { letter = 'H'; } else { letter = 'h'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::I) { if (caps) { letter = 'I'; } else { letter = 'i'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::J) { if (caps) { letter = 'J'; } else { letter = 'j'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::K) { if (caps) { letter = 'K'; } else { letter = 'k'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::L) { if (caps) { letter = 'L'; } else { letter = 'l'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::M) { if (caps) { letter = 'M'; } else { letter = 'm'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::N) { if (caps) { letter = 'N'; } else { letter = 'n'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::O) { if (caps) { letter = 'O'; } else { letter = 'o'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::P) { if (caps) { letter = 'P'; } else { letter = 'p'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::Q) { if (caps) { letter = 'Q'; } else { letter = 'q'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::R) { if (caps) { letter = 'R'; } else { letter = 'r'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::S) { if (caps) { letter = 'S'; } else { letter = 's'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::T) { if (caps) { letter = 'T'; } else { letter = 't'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::U) { if (caps) { letter = 'U'; } else { letter = 'u'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::V) { if (caps) { letter = 'V'; } else { letter = 'v'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::W) { if (caps) { letter = 'W'; } else { letter = 'w'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::X) { if (caps) { letter = 'X'; } else { letter = 'x'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::Y) { if (caps) { letter = 'Y'; } else { letter = 'y'; } }
            else if (ScanCode == (uint8_t)HAL::Keys::Z) { if (caps) { letter = 'Z'; } else { letter = 'z'; } }
            // space
            else if (ScanCode == (uint8_t)HAL::Keys::SPACE) { letter = ' '; }

            // print character
            if (letter > 0)
            {
                stradd(Text, letter);
            }
        }
    }
}