#pragma once
#include "lib/types.h"
#include "core/debug.hpp"

extern "C"
{
    void mem_init();

    size_t size(void* data);

    void* mem_alloc(size_t size);
    void mem_free(void* ptr);

    uint32_t mem_get_total_mb();
    uint32_t mem_get_total();

    void mem_copy(uint8_t* src, uint8_t* dest, uint32_t len);
    void mem_swap(uint8_t* src, uint8_t* dest, uint32_t len);
    void mem_fill(uint8_t* dest, uint8_t val, uint32_t len);
    void mem_lshift(uint8_t* bottom, uint8_t* top);

    void mem_write8(uint8_t* dest, uint8_t data);
    void mem_write16(uint8_t* dest, uint16_t data);
    void mem_write32(uint8_t* dest, uint32_t data);

    uint8_t mem_read8(uint8_t* dest);
    uint16_t mem_read16(uint8_t* dest);
    uint32_t mem_read32(uint8_t* dest);
}

// allocate overloads
extern void *operator new(size_t size);
extern void *operator new[](size_t size);

// delete overloads
extern void operator delete(void *p);
extern void operator delete(void *p, size_t size);
extern void operator delete[](void *p);
extern void operator delete[](void *p, size_t size);

namespace HAL
{
    class MemoryManager
    {
        public:
            void Initialize();
            void* Allocate(size_t size);
            void Free(void* ptr);
    };
}