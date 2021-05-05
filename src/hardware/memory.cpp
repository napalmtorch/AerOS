#include "hardware/memory.hpp"

extern "C"
{
    uint32_t free_pos;
    uint32_t max_pos;

    void mem_init()
    {
        // safely start 10 megabytes into memory
        free_pos = 10 * 1024 * 1024;

        // maximum size is installed memory minus 12 megabytes
        max_pos = (mem_get_total_mb() - 12) * 1024 * 1024;
    }

    void* mem_alloc(size_t size)
    {
        // check if enough memory is available
        if (free_pos + size >= max_pos) { debug_throw_message(MSG_TYPE_ERROR, "System out of memory"); return; }
        uint32_t addr = free_pos;
        free_pos += size;
        return (void*)addr;
    }

    void mem_free(void* ptr)
    {
        
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

    }

    // swap range of memory
    void mem_swap(uint8_t* src, uint8_t* dest, uint32_t len)
    {

    }

    // fill range of memory
    void mem_fill(uint8_t* dest, uint8_t vall, uint32_t len)
    {

    }
    
    // shift range of memory left by one
    void mem_lshift(uint8_t* bottom, uint8_t* top)
    {

    }

    // write 8-bit value to memory
    void mem_write8(uint8_t* dest, uint8_t data) { }

    // write 16-bit value to memory
    void mem_write16(uint8_t* dest, uint16_t data) { }

    // write 32-bit value to memory
    void mem_write32(uint8_t* dest, uint32_t data) { }

    // read 8-bit value from memory
    uint8_t mem_read8(uint8_t* dest) { }

    // read 16-bit value from memory
    uint16_t mem_read16(uint8_t* dest) { }

    // read 32-bit value from memory
    uint32_t mem_read32(uint8_t* dest) { }
}