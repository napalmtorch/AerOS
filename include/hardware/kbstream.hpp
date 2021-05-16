#pragma once
#include <lib/types.h>
#include <hardware/drivers/keyboard.hpp>

namespace HAL
{
    class KeyboardStream
    {
        public:
            KeyboardStream();
            void Update();

        public:
            char* Text;
            bool Enabled;
            bool ShiftDown;
            bool CapsLockDown;
    };
}