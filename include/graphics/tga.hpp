#pragma once
#include <lib/types.h>
#include <hardware/memory.hpp>
namespace Graphics
{
    class TGA
    {
        public:
        unsigned int ParseTga(unsigned char *file, int size);
    };
}