#pragma once
#include "lib/types.h"
#include "core/debug.hpp"

extern "C"
{
    #define MEM_STATE_FREE 0
    #define MEM_STATE_USED 1

    // ram allocation table entry structure
    typedef struct
    {
        // offset of the region of memory
        uint32_t offset;
        // size of the region of memory
        uint32_t size;
        // status flag of region of memory
        uint8_t state;
    } __attribute__((packed)) rat_entry_t;

    void mem_init(bool dynamic);

    size_t size(void* data);

    void* mem_alloc(size_t size);
    void mem_free(void* ptr);

    void mem_print_rat();

    uint32_t mem_get_total_mb();
    uint32_t mem_get_total();
    uint32_t mem_get_used();
    uint32_t mem_get_total_usable();

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
    int memcmp(const void* a, const void* b, size_t len);
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
            void Initialize(bool dynamic);
            void* Allocate(size_t size);
            void Free(void* ptr);
    };
}