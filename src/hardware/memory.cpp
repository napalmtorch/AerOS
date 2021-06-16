#include "hardware/memory.hpp"
#include <core/kernel.hpp>
#define DEBUG_MEM false
extern "C"
{
    // private declarations
    rat_entry_t* get_neighbour(rat_entry_t* entry);
    rat_entry_t* create_entry(uint32_t offset, uint32_t size, uint8_t state);
    bool         delete_entry(rat_entry_t* entry);
    rat_entry_t* get_free_entry(uint32_t size);
    rat_entry_t* get_entry(int32_t index);
    
    // rat table properties
    uint32_t     rat_table;
    uint32_t     rat_table_size;
    uint32_t     rat_max_entries;
    uint32_t     rat_pos;
    uint32_t     rat_count;

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
            // setup rat
            rat_table       = kernel_end_real;
            rat_max_entries = 32768;
            rat_table_size  = rat_max_entries * sizeof(rat_entry_t);
            rat_pos         = 0;
            rat_count       = 0;

            // clear table
            mem_fill((uint8_t*)rat_table, 0, rat_table_size);

            // setup reserved region
            reserved_start = rat_table + rat_table_size;
            reserved_size = ((mem_get_total_mb() - 1) * 1024 * 1024) - rat_table_size;
            reserved_pos  = 0;
            mem_used = 0;

            // map entire reserved memory region as a free chunk
            rat_entry_t* e = create_entry(reserved_start, reserved_size, MEM_STATE_FREE);

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
            // check if size is valid
            if (size == 0) { debug_throw_message(MSG_TYPE_ERROR, "Tried to allocate 0 bytes"); return nullptr; }

            // get next available free entry
            rat_entry_t* entry = get_free_entry(size + 2);
            if(DEBUG_MEM) {
            // allocation message
            debug_write("["); debug_write_ext("MALLOC", COL4_CYAN); debug_write("] ");
            debug_write_ext("offset", COL4_YELLOW);
            debug_write_hex(" = ", entry->offset);
            debug_write_ext("      size", COL4_YELLOW);
            debug_writeln_dec(" = ", size + 2); }

            // return entry data offset
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

            // allocation message
            //debug_write("ALLOCATION: ");
            //debug_write_hex("offset = ", offset);
            //debug_writeln_dec("      size = ", size + 1);

            // return offset
            return (void*)offset;
        }
    }
    
    size_t mem_sizeof(void* data)
    {
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            // found match
            if (entry->offset == (uint32_t)data)
            {
                // size should never be zero - filtered by 'mem_alloc'
                if (entry->size == 0) { debug_throw_panic("RAT Corruption"); return -1; }
                return (size_t)entry->size;
            }
        }
        return -1;
    }

    // free region of memory
    void mem_free(void* ptr)
    {
        // don't bother trying to free memory in this mode
        if (!dynamic_mode) { return; }
        if (ptr == nullptr) { return; }

        // locate entry in table
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            // found match
            if (entry->offset == (uint32_t)ptr)
            {
                // size should never be zero - filtered by 'mem_alloc'
                if (entry->size == 0) { debug_throw_panic("RAT Corruption"); return; }
                entry->state = MEM_STATE_FREE;
                if(DEBUG_MEM) {
                // allocation message
                debug_write("["); debug_write_ext(" FREE ", COL4_GREEN); debug_write("] ");
                debug_write_ext("offset", COL4_YELLOW);
                debug_write_hex(" = ", entry->offset);
                debug_write_ext("      size", COL4_YELLOW);
                debug_writeln_dec(" = ", entry->size);
                }

                // combine free entries
                mem_combine_free_entries();
                return;
            }
        }
    }

    // combine free entires
    void mem_combine_free_entries()
    {
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            if (entry->offset > 0 && entry->size > 0 && entry->state == MEM_STATE_FREE)
            {
                rat_entry_t* next = get_neighbour(entry);
                if (next != nullptr)
                {
                    entry->size += next->size;
                    entry->state = MEM_STATE_FREE;
                    delete_entry(next);
                }
            }
        }

        uint32_t free_total = 0;
        uint32_t free_offset = 0;
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            if (entry->offset > 0 && entry->size > 0 && entry->state == MEM_STATE_FREE)
            {
                free_total += entry->size;
                if (entry->offset > free_offset) { free_offset = entry->offset; }
                delete_entry(entry);
            }
        }
        create_entry(free_offset, free_total, MEM_STATE_FREE);
    }

    // get neighbouring free entry
    rat_entry_t* get_neighbour(rat_entry_t* entry)
    {
        if (entry == nullptr) { return nullptr; }
        if (entry->offset == 0) { return nullptr; }

        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* temp = get_entry(i);
            if (temp != entry && entry->offset + entry->size == temp->offset && temp->state == MEM_STATE_FREE) { return temp; }
            if (temp != entry && temp->offset + temp->size == entry->offset && temp->state == MEM_STATE_FREE) { return temp; }
        }
        return nullptr;
    }

    // create new rat entry
    rat_entry_t* create_entry(uint32_t offset, uint32_t size, uint8_t state)
    {
        rat_entry_t* entry = get_entry(rat_pos);
        entry->offset = offset;
        entry->size = size;
        entry->state = state;
        rat_pos++;
        rat_count++;
        return entry;
    }

    // delete rat entry
    bool delete_entry(rat_entry_t* entry)
    {
        // invalid entry
        if (entry == nullptr)   { debug_throw_message(MSG_TYPE_ERROR, "Tried to delete null entry"); return false; }
        if (entry->offset == 0) { debug_throw_message(MSG_TYPE_ERROR, "Tried to delete entry with null offset"); return false; }
        if (entry->size == 0)   { debug_throw_message(MSG_TYPE_ERROR, "Tried to delete entry with size of 0"); return false; }

        // locate entry in table
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* temp = get_entry(i);

            if (temp->offset == entry->offset)
            {
                // check if size is valid
                if (entry->size != temp->size) { debug_throw_message(MSG_TYPE_ERROR, "Size mismatch while locating entry for deletion"); return false; }

                // clear data
                temp->offset = 0;
                temp->size = 0;
                temp->state = 0;

                // shift data
                for (size_t j = i; j < rat_count; j++)
                {
                    rat_entry_t* now = get_entry(j);
                    rat_entry_t* next = get_entry(j + 1);
                    now->offset = next->offset;
                    now->size = next->size;
                    now->state = next->state;
                    next->offset = 0;
                    next->size = 0;
                    next->state = 0;
                
                }

                rat_count--;
                rat_pos--;
                return true;
            }
        }
        return false;
    }

    rat_entry_t* get_free_entry(uint32_t size)
    {
        if (size == 0) { return nullptr; }
        size_t i = 0;

        // attempt to find perfectly sized region first
        for (i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            if (entry->offset > 0 && entry->size == size && entry->state == MEM_STATE_FREE)
            {
                entry->state = MEM_STATE_USED;
                return entry;
            }
        }

        // couldn't find perfect match so locate larger chunk
        for (i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            // found available entry
            if (entry->offset > 0 && entry->size >= size && entry->state == MEM_STATE_FREE)
            {
                create_entry(entry->offset + size, entry->size - size, MEM_STATE_FREE);
                entry->size = size;
                entry->state = MEM_STATE_USED;
                return entry;
            }
        }

        // unable to locate free entry
        debug_throw_panic("System out of memory");
        return nullptr;
    }

    // get size of entry by pointer
    uint32_t mem_get_size_of(void* ptr)
    {
        if (ptr == nullptr) { debug_throw_message(MSG_TYPE_ERROR, "Unable to determine size of nullptr"); return 0; }
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);
            if (entry->offset == (uint32_t)ptr) { return entry->size; }
        }
        debug_throw_message(MSG_TYPE_ERROR, "Unable to locate pointer within RAT");
        return 0;
    }


    // get rat entry by index
    rat_entry_t* get_entry(int32_t index)
    {
        return (rat_entry_t*)(rat_table + (index * sizeof(rat_entry_t)));
    }

    // print ram allocation table 
    void mem_print_rat()
    {
        char temp[24];
        uint index = 0;

        term_writeln_ext("Showing RAM allocation table", Graphics::Colors::Green);
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);
            if (entry->offset != 0 && entry->size != 0)
            {
                // index
                term_write_ext("ID: 0x", Graphics::Colors::Green);
                strhex32(index, temp);
                term_write(temp);

                // offset
                term_write_ext("  offset", Graphics::Colors::Cyan);
                strhex32(entry->offset, temp);
                term_write(" = ");
                term_write(temp);

                // state
                term_write_ext("  state", Graphics::Colors::Magenta);
                if (entry->state == MEM_STATE_FREE) { term_write(" = FREE"); } else { term_write(" = USED"); }

                // size
                term_write_ext("  size", Graphics::Colors::Yellow);
                strdec(entry->size, temp);
                term_write(" = ");
                term_writeln(temp);
            }
            index++;
        }
        term_newline();
    }

    // get amount of memory used
    uint32_t mem_get_used()
    {
        uint32_t used = 0;
        for (size_t i = 0; i < rat_count; i++)
        {
            rat_entry_t* entry = (rat_entry_t*)(rat_table + (i * sizeof(rat_entry_t)));
            if (entry->state == MEM_STATE_USED) { used += entry->size; }
        }
        return used;
    }

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
        uint32_t buffer_size = len;
        uint32_t destt       = (uint32_t)dest;
        uint32_t srcc        = (uint32_t)src;
        uint32_t num_dwords = buffer_size / 4;
        uint32_t num_bytes  = buffer_size % 4;
        uint32_t *dest32    = (uint32_t*)destt;
        uint32_t *src32     = (uint32_t*)srcc;
        uint8_t *dest8      = ((uint8_t*)destt) + num_dwords * 4;
        uint8_t *src8       = ((uint8_t*)srcc) + num_dwords * 4;
        uint32_t i;

        for (i = 0; i < num_dwords; i++) { dest32[i] = src32[i]; }
        for (i = 0; i < num_bytes; i++) { dest8[i] = src8[i]; }
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

    // print ram allocation table - compatible with gui
    void MemoryManager::PrintRAT(bool serial)
    {
        if (!serial)
        {
            char temp[24];
            uint index = 0;

            System::KernelIO::Terminal.WriteLine("Showing RAM allocation table", COL4_GREEN);
            for (size_t i = 0; i < rat_max_entries; i++)
            {
                rat_entry_t* entry = get_entry(i);
                if (entry->offset != 0 && entry->size != 0)
                {
                    // index
                    System::KernelIO::Terminal.Write("ID: 0x", COL4_GREEN);
                    strhex32(index, temp);
                    System::KernelIO::Terminal.Write(temp);

                    // offset
                    System::KernelIO::Terminal.Write("  offset", COL4_CYAN);
                    strhex32(entry->offset, temp);
                    System::KernelIO::Terminal.Write(" = ");
                    System::KernelIO::Terminal.Write(temp);

                    // state
                    System::KernelIO::Terminal.Write("  state", COL4_MAGENTA);
                    if (entry->state == MEM_STATE_FREE) 
                    { System::KernelIO::Terminal.Write(" = FREE"); } else { System::KernelIO::Terminal.Write(" = USED"); }

                    // size
                    System::KernelIO::Terminal.Write("  size", COL4_YELLOW);
                    strdec(entry->size, temp);
                    System::KernelIO::Terminal.Write(" = ");
                    System::KernelIO::Terminal.WriteLine(temp);
                }
                index++;
            }
            System::KernelIO::Terminal.NewLine();
        }
        else
        {
            char temp[24];
            uint index = 0;

            serial_writeln_ext("Showing RAM allocation table", COL4_GREEN);
            for (size_t i = 0; i < rat_max_entries; i++)
            {
                rat_entry_t* entry = get_entry(i);
                if (entry->offset != 0 && entry->size != 0)
                {
                    // index
                    serial_write_ext("ID: 0x", COL4_GREEN);
                    strhex32(index, temp);
                    serial_write(temp);

                    // offset
                    serial_write_ext("  offset", COL4_CYAN);
                    strhex32(entry->offset, temp);
                    serial_write(" = ");
                    serial_write(temp);

                    // state
                    serial_write_ext("  state", COL4_MAGENTA);
                    if (entry->state == MEM_STATE_FREE) 
                    { serial_write(" = FREE"); } else { serial_write(" = USED"); }

                    // size
                    serial_write_ext("  size", COL4_YELLOW);
                    strdec(entry->size, temp);
                    serial_write(" = ");
                    serial_writeln(temp);
                }
                index++;
            }
            System::KernelIO::Terminal.NewLine();
        }
    }
}