#include "hardware/kbstream.hpp"

namespace HAL
{
    KeyboardStream::KeyboardStream() { }

    // constructor with pointer
    KeyboardStream::KeyboardStream(char* output) : Text(output) { }
}