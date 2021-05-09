#pragma once

#include <lib/types.h>
extern "C" {

#define PAGE_WRITE_THROUGH              0x08    // Page-level write-through
#define PAGE_CACHE_DISABLE              0x10    // Page-level cache-disable

// ------------------------------------------------------------------------------------------------
void VMInit();

void *VMAlloc(uint size);
void *VMAllocAlign(uint size, uint align);

void VMMapPages(void *addr, uint size, uint flags);
}
namespace HAL
{
    class VMM
    {
        public:
            void Initialize();
            void DumpVMMap();
            void* VMAlloc(uint size);
    };
}
