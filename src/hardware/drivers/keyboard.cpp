#include "hardware/drivers/keyboard.hpp"
#include "core/kernel.hpp"



// ---------------------------------------- UPPER CASE ---------------------------------------- //

// upper case scancode indexed ascii values 
const char ASCIIUpper_US[58] = 
{
    '?', '?', '!', '@', '#', '$', '%', '^',     
    '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
    'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G', 
    'H', 'J', 'K', 'L', ':', '"', '~', '?', '|', 'Z', 'X', 'C', 'V', 
    'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '
};

// ---------------------------------------- LOWER CASE ---------------------------------------- //

// lower case scancode indexed ascii values
const char US_ASCIILower_US[58] = 
{
    '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y', 
    'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
    'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '
};

// 

void kb_callback(registers_t regs)
{
    // get scancode
    System::KernelIO::Keyboard.ReadScanCode();

    // map pressed keys
    for (size_t i = 0; i < KB_SCANCODE_MAX; i++)
    {
        // found pressed key
        if (i == System::KernelIO::Keyboard.GetScanCode())
        // key down
        { 
            System::KernelIO::Keyboard.GetPressedKeys()[i] = true; 
        }
        // key up
        else 
        {
            System::KernelIO::Keyboard.GetPressedKeys()[i] = false;
        }
    }

    // handle keybaord input
    if (System::KernelIO::Keyboard.BufferEnabled) { System::KernelIO::Keyboard.HandleInput(); }
}

namespace HAL
{
    KeyboardLayout::KeyboardLayout()
    {
        Type = KB_LAYOUT_US;
        strcat(Name, "United States");
        strcat(UpperCase, "??!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>???? ");
        strcat(LowerCase, "??!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>???? ");
    }

    KeyboardLayout::KeyboardLayout(KB_LAYOUT type, char* name, char* upper, char* lower)
    {
        Type = type;
        strcat(Name, name);
        strcat(UpperCase, upper);
        strcat(LowerCase, lower);
    }

    // initialize ps/2 keyboard controller
    void PS2Keyboard::Initialize()
    {
        // initialize port
        Port = IOPort(0x60);

        // register interrupt request
        HAL::CPU::RegisterIRQ(IRQ1, (isr_t)kb_callback);

        KeyboardLayoutUS = { KB_LAYOUT_US, "United States", "??!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>???? ", "" };
        KeyboardLayoutDE = { KB_LAYOUT_DE, "German",        "??!\"§$%&/()=?`??QWERTZUIOP?*??ASDFGHJKL??'?>YXCVBNM;:???? ", "" }; 
    }

    // handle keyboard string input
    void PS2Keyboard::HandleInput()
    {
        // backspace
        if (ScanCode == (uint8_t)HAL::Keys::BACKSPACE)
        {
            if (strlen(Buffer) > 0)
            {
                strdel(Buffer);
                System::KernelIO::Terminal.Delete();
            }
        }
        // handle shift
        else if (ScanCode == (uint8_t)HAL::Keys::LSHIFT || ScanCode == (uint8_t)HAL::Keys::RSHIFT) { ShiftDown = true; }
        else if (ScanCode == 0xAA) { ShiftDown = false; }
        // handle caps lock
        else if (ScanCode == (uint8_t)HAL::Keys::CAPSLOCK) { CapsLockDown = !CapsLockDown; }
        // handle enter
        else if (ScanCode == (uint8_t)HAL::Keys::ENTER)
        {
            System::KernelIO::Terminal.WriteChar('\n');
            if (Event_OnEnterPressed != 0) { Event_OnEnterPressed(Buffer); }
            Buffer[0] = '\0';
        }
        // handle characters
        else
        {
            char letter = 0;
            bool caps = ((ShiftDown && !CapsLockDown) || (!ShiftDown && CapsLockDown));

            // symbols
            if (ScanCode == (uint8_t)HAL::Keys::BACKTICK) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::BACKTICK) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::MINUS) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::EQUALS) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::LBRACKET) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::RBRACKET) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::COLON) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::QUOTE) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::BACKSLASH) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::COMMA) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::PERIOD) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::SLASH) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            // numerals
            else if (ScanCode == (uint8_t)HAL::Keys::D1) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D2) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D3) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D4) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D5) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D6) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D7) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D8) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D9) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D0) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            // characters
            else if (ScanCode == (uint8_t)HAL::Keys::A) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::B) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::C) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::D) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::E) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::F) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::G) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::H) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::I) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::J) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::K) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::L) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::M) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::N) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::O) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::P) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::Q) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::R) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::S) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::T) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::U) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::V) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::W) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::X) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::Y) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            else if (ScanCode == (uint8_t)HAL::Keys::Z) { if (caps) { letter = ASCIIUpper_US[(int)ScanCode]; } else { letter = US_ASCIILower_US[(int)ScanCode]; } }
            // space
            else if (ScanCode == (uint8_t)HAL::Keys::SPACE) { letter = ' '; }

            // print character
            if (letter > 0)
            {
                stradd(Buffer, letter);
                System::KernelIO::Terminal.WriteChar(letter);
            }
        }
    }

    // read scancode from port
    void PS2Keyboard::ReadScanCode() { ScanCode = Port.ReadInt8(); }

    // return current scancode
    uint32_t PS2Keyboard::GetScanCode() { return ScanCode; }

    // check if key is down
    bool PS2Keyboard::IsKeyDown(Keys key)
    {
        return PressedKeys[(int)key];
    }

    // check if key is up
    bool PS2Keyboard::IsKeyUp(Keys key)
    {
        return !PressedKeys[(int)key];
    }

    // check if any key is down
    bool PS2Keyboard::IsAnyKeyDown() { return ScanCode > 0; }

    // check if all keys are up
    bool PS2Keyboard::IsAllKeysUp()
    {
        for (int i = 0; i < KB_SCANCODE_MAX; i++)
        {
            if (PressedKeys[i] == true) { return false; }
        }
        return true;
    }

    // get pressed keys array
    bool* PS2Keyboard::GetPressedKeys() { return PressedKeys; }

    // check if caps lock is pressed
    bool PS2Keyboard::GetCapsLockDown() { return IsKeyDown(HAL::Keys::CAPSLOCK); }

    // check if either shift key is down
    bool PS2Keyboard::GetShiftDown() { return IsKeyDown(HAL::Keys::LSHIFT) || IsKeyDown(HAL::Keys::RSHIFT); }

    // check if control key is down
    bool PS2Keyboard::GetControlDown() { return false; } // not yet implemented

    // check if alt key is down
    bool PS2Keyboard::GetAltDown() { return false; } // not yet implemented

    // check if enter key is down
    bool PS2Keyboard::GetEnterDown() { return IsKeyDown(HAL::Keys::ENTER); }
}