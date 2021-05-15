#include "hardware/memory.hpp"
#include <core/kernel.hpp>

extern "C"
{
    // private declarations
    rat_entry_t* mem_create_entry(uint32_t offset, uint32_t size, uint8_t state);
    bool         mem_delete_entry(rat_entry_t* entry);
    uint32_t     mem_get_free_offset(uint32_t size);
    
    // ram allocation table properties
    uint32_t table_start;
    uint32_t table_size;
    uint32_t table_pos;
    uint32_t table_count;

    // reserved memory properties
    uint32_t reserved_start;
    uint32_t reserved_size;
    uint32_t reserved_pos;

    // properties
    uint32_t mem_used;
    bool dynamic_mode;

    // initialize memory management system
    void mem_init(bool dynamic)
    {
        // set dynamic mode
        dynamic_mode = dynamic;

        if (dynamic_mode)
        {
            // setup table
            table_start = kernel_end_real + (4 * 1024 * 1024);
            table_size = 4 * 1024 * 1024;
            table_pos = 0;
            table_count = 0;

            // clear table
            mem_fill((uint8_t*)table_start, 0, table_size);

            // setup reserved memory
            reserved_start = table_start + table_size;
            reserved_size = (mem_get_total_mb() * 1024 * 1024) - reserved_start;
            reserved_pos = 0;

            // map entire reserved memory region as a free chunk
            mem_create_entry(reserved_start, reserved_size, MEM_STATE_FREE);

            mem_print_rat();
        }
        else
        {
            // set reserved memory to maximum possibel and never look back
            reserved_start = kernel_end_real + (4 * 1024 * 1024);
            reserved_size = (mem_get_total() - reserved_start);
        }
    }

    // allocate region of memory
    void* mem_alloc(size_t size)
    {
        // dynamic allocation mode
        if (dynamic_mode)
        {
            rat_entry_t* entry = (rat_entry_t*)mem_get_free_offset(size + 1);
            
            // allocation message
            debug_write("ALLOCATION: ");
            debug_write_hex("offset = ", entry->offset);
            debug_writeln_dec("      size = ", size + 1);

            mem_used += size;
            debug_writeln_dec("RAT COUNT: ", table_count);
            return (void*)entry->offset;
        }
        // 'never look back' mode
        else
        {
            // get available offset
            uint32_t offset = reserved_start + reserved_pos;
            
            // increment reserved memory position
            reserved_pos += size + 1;
            // increment memory used
            mem_used += size + 1;

            // allocation message
            //debug_write("ALLOCATION: ");
            //debug_write_hex("offset = ", offset);
            //debug_writeln_dec("      size = ", size + 1);

            // return offset
            return (void*)offset;
        }
    }

    // free region of memory
    void mem_free(void* ptr)
    {
        // don't bother trying to free memory in this mode
        if (!dynamic_mode) { return; }
        if (ptr == nullptr) { return; }


        // locate entry in table
        for (size_t i = 0; i < table_size; i += sizeof(rat_entry_t))
        {
            rat_entry_t* entry = (rat_entry_t*)(table_start + i);

            // located rat entry
            if (entry->offset == (uint32_t)ptr)
            {
                debug_write("FREE: ");
                debug_write_hex("offset = ", entry->offset);
                debug_writeln_dec("      size = ", entry->size);

                // free and return
                entry->state = MEM_STATE_FREE;
                mem_used -= entry->size;
                return;
            }
        }

    }

    uint32_t mem_get_free_offset(uint32_t size)
    {
        rat_entry_t* entry;
        size_t i;

        // attempt to find perfectly sized region
        for (i = 0; i < table_size; i += sizeof(rat_entry_t))
        {
            entry = (rat_entry_t*)(table_start + i);

            // confirm entry is valid
            if (entry->offset != 0)
            {
                // found perfectly sized region
                if (entry->size == size && entry->state == MEM_STATE_FREE)
                {
                    // set entry state to used
                    entry->state = MEM_STATE_USED;
                    // return
                    return (uint32_t)entry;
                }
            }
        }

        // find a regioWhan larger and portion it
        for (i = 0; i < table_size; i += sizeof(rat_entry_t))
        {
            entry = (rat_entry_t*)(table_start + i);

            // confirm entry is valid
            if (entry->offset != 0)
            {
                // found region large enough to portion
                if (entry->size > size && entry->state == MEM_STATE_FREE)
                {
                    mem_create_entry(entry->offset + size, entry->size - size, MEM_STATE_FREE);
                    entry->size = size;
                    entry->state = MEM_STATE_USED;
                    return (uint32_t)entry;
                }
            }
        }

        // return offset - likely invalid if it got this far
        debug_throw_message(MSG_TYPE_ERROR, "Unable to allocate more memory");
        return 0;
    }

    // create ram allocation table entry
    rat_entry_t* mem_create_entry(uint32_t offset, uint32_t size, uint8_t state)
    {
        if (table_pos >= table_size) { debug_throw_message(MSG_TYPE_ERROR, "RAM allocation table is full"); return nullptr; }

        // create new item at next position
        rat_entry_t* entry = (rat_entry_t*)(table_start + table_pos);
        
        // set entry properties
        entry->offset = offset;
        entry->size = size;
        entry->state = state;

        // increment table position
        table_pos += sizeof(rat_entry_t);
        table_count++;

        // return allocation table entry pointer
        return entry;
    }

    // delete ram allocation table entry
    bool mem_delete_entry(rat_entry_t* entry)
    {
        uint32_t entry_offset = (uint32_t)entry;
        bool exists = false;
        if (entry_offset == 0) { debug_throw_message(MSG_TYPE_ERROR, "Tried to delete null rat entry"); return false; }
        for (size_t i = 0; i < table_size; i += sizeof(rat_entry_t))
        {
            // get entry
            rat_entry_t* e = (rat_entry_t*)(table_start + i);

            // located entry in table
            if (e->offset == entry->offset) { exists = true; break; }
        }

        if (exists)
        {
            // clear entry
            entry->offset = 0;
            entry->size = 0;
            entry->state = 0;

            // shift list to the left by 1 entry
            for (size_t i = 0; i < sizeof(rat_entry_t); i++) { mem_lshift((uint8_t*)entry_offset, (uint8_t*)(table_start + (table_count * sizeof(rat_entry_t)))); }

            // decrement table count and return success
            table_count--;
            table_pos -= sizeof(rat_entry_t);
            return true;
        }

        // unable to locate entry in table
        debug_throw_message(MSG_TYPE_ERROR, "Unable to locate rat entry in table");
        return false;
    }

    // print ram allocation table 
    void mem_print_rat()
    {
        char temp[16];
        uint32_t index = 0;
        for (size_t i = 0; i < table_size; i += sizeof(rat_entry_t))
        {
            rat_entry_t* entry = (rat_entry_t*)(table_start + i);

            if (entry->offset != 0)
            {
                // id
                serial_write_ext("ENTRY ", COL4_YELLOW);
                strhex32(index, temp);
                serial_write_ext(temp, COL4_YELLOW);
                serial_write_ext(":    ", COL4_YELLOW);

                // offset
                serial_write_ext("offset = ", COL4_CYAN);
                strhex32(entry->offset, temp);
                serial_write(temp);
                serial_write("    ");
                
                // state
                serial_write_ext("state = ", COL4_CYAN);
                strdec(entry->state, temp);
                serial_write(temp);
                serial_write("  ");

                // size
                serial_write_ext("size = ", COL4_CYAN);
                strdec(entry->size, temp);
                serial_writeln(temp);
            }
            
            index++;
        }
    }

    // get amount of memory used
    uint32_t mem_get_used() { return mem_used; }

    uint32_t mem_get_total_usable() { return reserved_size; }

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

    int memcmp(const void* a, const void* b, size_t len) 
    {
        int r = 0;
        const unsigned char *x = (const unsigned char*)a;
        const unsigned char *y = (const unsigned char*)b;

        while (len--) {
            r = *x - *y;
            if (r)
                return r;
            x++, y++;
        }
        return 0;
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
    void MemoryManager::Initialize(bool dynamic) { mem_init(dynamic); }

    // allocate region of memory
    void* MemoryManager::Allocate(size_t size) { return mem_alloc(size); }

    // free region of memory
    void MemoryManager::Free(void* ptr) { mem_free(ptr); }
}