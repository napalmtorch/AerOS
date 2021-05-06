#include "hardware/memory.hpp"

extern "C"
{
    // entry of a memory region
    typedef struct entry
    {
        // this allows us to know that the address is an entry
        uint8_t offset;

        // last known entry (if none it is -1)
        void* prev;

        // if the entry was used or not
        bool used;
        
        // next known entry (if none it is set -1)
        void* next;
    } entry_t;
    

    uint32_t free_pos;
    uint32_t max_pos;
    uint32_t start;

    void mem_init()
    {
        // safely start 10 megabytes into memory
        free_pos = 10 * 1024 * 1024;

        // set the start pointer to 10 megabytes
        start = 10 * 1204 * 1024;

        // maximum size is installed memory minus 12 megabytes
        max_pos = (mem_get_total_mb() - 12) * 1024 * 1024;
    }

    void* last = (void*)-1;

    // calculate the size of the memory region, knowing the entry
    size_t size_of_region(entry_t* entry)
    {
        return (uint32_t)entry->next - (uint32_t)entry - sizeof(struct entry);
    }

    // setup a new entry in a not known position
    entry_t* setup_entry(void* at, size_t size)
    {
        entry_t* entry = (entry_t*)at;
        entry->offset = 0xFF;
        entry->next = (entry_t*)(at + sizeof(struct entry) + size);
        entry->prev = last;
        entry->used = false;
        last = (entry_t*)entry;
        return entry;
    }

    entry_t* alloc_entry(entry_t* current, size_t size)
    {
        size_t esize = size_of_region(current);

        // easiest possibility: the entry is the same size as the requested allocation
        if (size == esize)
        {
            return current;
        }
        // the requested size is bigger than the entry
        else if (size > esize)
        {
            // if so, we try to merge entries until we match the requested size, or we find a used entry
            while (size > esize)
            {
                // gather the next region entry
                entry_t* next = (entry_t*)current->next;
                // check if it is an actual entry
                if (next->offset != 0xFF)
                {
                    // if it is not, setup the entry
                    next = setup_entry((void*)next, size-esize);
                    // merge it
                    current->next = next->next;
                    esize = size_of_region(current);
                    // and break the loop
                    break;
                }
                // if the region is used, we just break the loop and return null
                else if (next->used)
                    break;
                // otherwise we merge it
                current->next = next->next;
                // check the new size
                esize = size_of_region(current);
                // and continue the loop
            }
            if (esize >= size)
                return alloc_entry(current, size);
            else
                return nullptr;
        }
        // third scenario: the entry size is bigger than requested
        else if (esize > size)
        {
            // in this case we just shrink the entry
            void* next = ((entry_t*)current->next)->next;
            current->next = (void*)((uint32_t)current->next - (esize - size));
            ((entry_t*)current->next)->offset = 0xFF;
            ((entry_t*)current->next)->next = next;
            ((entry_t*)current->next)->prev = (void*)current;
            ((entry_t*)current->next)->used = false;
        }
    }

    // search next free entry
    entry_t* get_free_entry(void* from, size_t size)
    {
        // check if we reached free_pos
        if ((uint32_t)from >= free_pos)
        {
            // if so allocate the memory section here

            // increase the free position by the size of the allocated memory region
            free_pos = (uint32_t)(from + sizeof(struct entry) + size);
            return setup_entry(from, size);
        }
        // current entry we're working with
        entry_t* current = (entry_t*)from;

        // loop repeated every time the entry is used
        while (current->used)
        {
            // switch to new entry
            current = (entry_t*)current->next;
            // if this isn't an address, it probably means we reached the end of the entry chain
            if (current->offset != 0xFF)
            {
                // so we just allocate a new one here
                return setup_entry((void*)current, size);
            }
        }
        // if we still alive, we just call the alloc_entry, where all the magic happens
        entry_t* e = alloc_entry(current, size);
        if (e == nullptr)
        {
            debug_throw_message(MSG_TYPE::MSG_TYPE_WARNING, "fell into next region manager");
            return get_free_entry(current->next, size);
        }
        return e;
    }

    size_t size(void* data)
    {
        // retrun the size from the entry
        return size_of_region(((entry_t*)((uint32_t)data - sizeof(struct entry))));
    }

    void* mem_alloc(size_t size)
    {
        // check if enough memory is available
        if (free_pos + size >= max_pos) { debug_throw_message(MSG_TYPE_ERROR, "System out of memory"); return nullptr; }
        
        // request first free entry
        entry_t* entry = get_free_entry((void*)start, size);
        
        entry->used = true;

        System::KernelIO::Write("ALLOCATION ", COL4::COL4_YELLOW);
        System::KernelIO::Write("at ");
        char s[6];
        strdec((uint32_t)(uint32_t)entry + sizeof(struct entry), s);
        System::KernelIO::WriteLine(s);

        //return alloc result
        return (void*)((uint32_t)entry + sizeof(struct entry));
    }

    void mem_free(void* ptr)
    {
        System::KernelIO::Write("FREE at ");
        char s[6];
        strdec((uint32_t)ptr, s);
        System::KernelIO::WriteLine(s);

        entry_t* entry = (entry_t*)((uint32_t)ptr - sizeof(struct entry));
        entry->used = false;
    }

    // get amount of installed memory in megabytes
    uint32_t mem_get_total_mb()
    {
        // borrowed kindly from nik300
        register uint64_t* mem;
        uint64_t mem_count, a;
        uint32_t memkb;
        uint8_t	irq1, irq2;

        /* save IRQ's */
        irq1 = inb(0x21);
        irq2 = inb(0xA1);

        /* kill all irq's */
        outb(0x21, 0xFF);
        outb(0xA1, 0xFF);

        mem_count = 0;
        memkb = 0;

        __asm__ __volatile__("wbinvd");

        do 
        {
            memkb++;
            mem_count += 1024 * 1024;
            mem = (uint64_t*)mem_count;

            a = *mem;
            *mem = 0x55AA55AA;

            asm("":::"memory");
            if (*mem != 0x55AA55AA) { mem_count = 0; }
            else 
            {
                *mem = 0xAA55AA55;
                asm("":::"memory");
                if (*mem != 0xAA55AA55)
                    mem_count = 0;
            }

            asm("":::"memory");
            *mem = a;

        } while (memkb < 4096 && mem_count != 0);

        mem = (uint64_t*)0x413;

        outb(0x21, irq1);
        outb(0xA1, irq2);

        return memkb;
    }

    // get amount of installed memory in bytes
    uint32_t mem_get_total() { return mem_get_total_mb() * 1024 * 1024; }

    // copy range of memory
    void mem_copy(uint8_t* src, uint8_t* dest, uint32_t len)
    {
        for (size_t i = 0; i < len; i++) { *(dest + i) = *(src + i); }
    }

    // swap range of memory
    void mem_swap(uint8_t* src, uint8_t* dest, uint32_t len)
    {
        for (size_t i = 0; i < len; i++)
        {
            if (*(dest + i) != *(src + i)) { *(dest + i) = *(src + i); }
        }
    }   

    // fill range of memory
    void mem_fill(uint8_t* dest, uint8_t val, uint32_t len)
    {
        for (size_t i = 0; i < len; i++) { *(dest + i) = val; }
    }
    
    // shift range of memory left by one
    void mem_lshift(uint8_t* bottom, uint8_t* top)
    {
        uint8_t saved = *(bottom - 1);
        for (size_t i = 0; i < (uint32_t)(top - bottom); i++)
        {
            if ((int32_t)i - 1 >= 0) { bottom[i - 1] = bottom[i]; }
        }
        *(top - 1) = 0;
        *(bottom - 1) = saved;
    }

    // write 8-bit value to memory
    void mem_write8(uint8_t* dest, uint8_t data) { *(dest) = data; }

    // write 16-bit value to memory
    void mem_write16(uint8_t* dest, uint16_t data) { uint16_t* ptr = (uint16_t*)dest; ptr[0] = data; }

    // write 32-bit value to memory
    void mem_write32(uint8_t* dest, uint32_t data) { uint32_t* ptr = (uint32_t*)dest; ptr[0] = data; }

    // read 8-bit value from memory
    uint8_t mem_read8(uint8_t* dest) { return dest[0]; }

    // read 16-bit value from memory
    uint16_t mem_read16(uint8_t* dest) { uint16_t* ptr = (uint16_t*)dest; return ptr[0]; }

    // read 32-bit value from memory
    uint32_t mem_read32(uint8_t* dest) { uint32_t* ptr = (uint32_t*)dest; return ptr[0];}
}

// memory management overloads
void *operator new(size_t size) { return mem_alloc(size); }
void *operator new[](size_t size) { return mem_alloc(size); }
void operator delete(void *p) { mem_free(p); }
void operator delete(void *p, size_t size) { mem_free(p); UNUSED(size); }
void operator delete[](void *p) { mem_free(p); }
void operator delete[](void *p, size_t size) { mem_free(p); UNUSED(size); }

namespace HAL
{
    // initialize memory manager
    void MemoryManager::Initialize() { mem_init(); }

    // allocate region of memory
    void* MemoryManager::Allocate(size_t size) { return mem_alloc(size); }

    // free region of memory
    void MemoryManager::Free(void* ptr) { mem_free(ptr); }
}