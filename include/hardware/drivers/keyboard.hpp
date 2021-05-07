#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/ports.hpp"
#include "hardware/interrupt/isr.hpp"
#include "hardware/cpu.hpp"
#include "hardware/terminal.hpp"

#define KB_SCANCODE_MAX 256

namespace HAL
{
    // ps/2 keyboard scancode enumerator
    enum class Keys
    {
        NONE,
        ESCAPE,
        D1,
        D2,
        D3,
        D4,
        D5,
        D6,
        D7,
        D8,
        D9,
        D0,
        MINUS,
        EQUALS,
        BACKSPACE,
        TAB,
        Q,
        W,
        E,
        R,
        T,
        Y,
        U,
        I,
        O,
        P,
        LBRACKET,
        RBRACKET,
        ENTER,
        LCTRL,
        A,
        S,
        D,
        F,
        G,
        H,
        J,
        K,
        L,
        COLON,
        QUOTE,
        BACKTICK,
        LSHIFT,
        BACKSLASH,
        Z,
        X,
        C,
        V,
        B,
        N,
        M,
        COMMA,
        PERIOD,
        SLASH,
        RSHIFT,
        KP_MULTIPLY,
        LALT,
        SPACE,
        CAPSLOCK,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        NUMLOCK,
        SCROLL,
        KP_D7,
        KP_D8,
        KP_D9,
        KP_MINUS,
        KP_D4,
        KP_D5,
        KP_D6,
        KP_PLUS,
        KP_D1,
        KP_D2,
        KP_D3,
        KP_D0,
        KP_PERIOD,
        F11 = 0x57,
        F12,
    };

    class PS2Keyboard
    {
        public:
            // keyboard buffer
            bool BufferEnabled;
            char Buffer[256];
            void (*Event_OnEnterPressed)(char*);

            // initialize ps2 keyboard controller
            void Initialize();

            // handle input
            void HandleInput();

            // read scancode from port
            void ReadScanCode();

            // return current scancode
            uint32_t GetScanCode();

            // check if key is down
            bool IsKeyDown(Keys key);

            // check if key is up
            bool IsKeyUp(Keys key);

            // check if any key is down
            bool IsAnyKeyDown();

            // check if all keys are up
            bool IsAllKeysUp();
            
            // get pressed keys array
            bool* GetPressedKeys();

            // check if caps lock is pressed
            bool GetCapsLockDown();

            // check if either shift key is down
            bool GetShiftDown();

            // check if control key is down
            bool GetControlDown();

            // check if alt key is down
            bool GetAltDown();

            // check if enter key is down
            bool GetEnterDown();

        private:
            // array of currently pressed keys
            bool PressedKeys[KB_SCANCODE_MAX];
            uint8_t ScanCode;
            bool ShiftDown, CapsLockDown;

            // port
            IOPort Port;
    };
}