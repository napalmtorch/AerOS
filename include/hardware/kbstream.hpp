#pragma once
#include <lib/types.h>
#include <hardware/drivers/keyboard.hpp>

namespace HAL
{
    class KeyboardStream
    {
        public:
            KeyboardStream();
            KeyboardStream(void* window);
            void Update();

        public:
            void* Window;
            char* Text;
            bool Enabled;
            bool ShiftDown;
            bool CapsLockDown;
            bool KeyDown;
    };
}