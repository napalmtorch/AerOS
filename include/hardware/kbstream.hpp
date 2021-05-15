#pragma once
#include <lib/types.h>
#include <hardware/drivers/keyboard.hpp>

namespace HAL
{
    class KeyboardStream
    {
        public:
            KeyboardStream();
            KeyboardStream(char* output);
            void Update();
            
        public:
            char* Text;
    };
}