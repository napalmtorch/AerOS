#include <hardware/memory.hpp>
#include <core/kernel.hpp>
#include <core/debug.hpp>

// private declarations
rat_entry_t* get_neighbour(rat_entry_t* entry);
rat_entry_t* create_entry(uint32_t offset, uint32_t size, uint8_t state);
bool         delete_entry(rat_entry_t* entry);
rat_entry_t* get_free_entry(uint32_t size);
rat_entry_t* get_entry(int32_t index);

// rat table
uint32_t     rat_table;
uint32_t     rat_table_size;
uint32_t     rat_max_entries;
uint32_t     rat_pos;
uint32_t     rat_count;

// reserved properties
uint32_t reserved_base;
uint32_t reserved_size;
uint32_t reserved_pos;
uint32_t reserved_used;

bool dynamic_mode = false;

// methods
void mem_init(bool dynamic)
{
    // set mode
    dynamic_mode = dynamic;

    // dynamic mode
    if (dynamic)
    {
        // setup rat
        rat_table       = kernel_end_real + (4 * 1024 * 1024);
        rat_max_entries = 32768;
        rat_table_size  = rat_max_entries * sizeof(rat_entry_t);
        rat_pos         = 0;
        rat_count       = 0;

        // clear table
        mem_fill((uint8_t*)rat_table, 0, rat_table_size);

        // setup reserved region
        reserved_base = rat_table + rat_table_size;
        reserved_size = ((mem_get_total_mb() - 1) * 1024 * 1024) - rat_table_size;
        reserved_pos  = 0;
        reserved_used = 0;

        rat_entry_t* e = create_entry(reserved_base, reserved_size, MEM_STATE_FREE);
    }
    // never look back mode
    else
    {
        reserved_base = kernel_end_real + (4 * 1024 * 1024);
        reserved_size = (mem_get_total_mb() - 11) * 1024 * 1024;
        reserved_pos = 0;
    }
}

void mem_alloc_msg(uint32_t offset, uint32_t size)
{
    // msg
    char temp[24];
    System::KernelIO::Write("[");
    System::KernelIO::Write("MALLOC", COL4_GREEN);
    System::KernelIO::Write("]");

    System::KernelIO::Write(" offset");
    System::KernelIO::Write(" = 0x");
    strhex32(offset, temp);
    System::KernelIO::Write(temp);

    System::KernelIO::Write("    size", COL4_CYAN);
    System::KernelIO::Write(" = ");
    strdec(size, temp);
    System::KernelIO::WriteLine(temp);
}

void* mem_alloc(size_t size)
{
    if (size == 0) { System::KernelIO::ThrowWarning("Tried to allocate 0 bytes"); return nullptr; }

    // dynamic allocation mode
    if (dynamic_mode)
    {
        rat_entry_t* entry = get_free_entry(size + 2);

        mem_alloc_msg(entry->offset, entry->size);

        return (void*)entry->offset;
    }
    // never look back mode
    else
    {
        // calculate
        uint32_t addr = reserved_base + reserved_pos;
            
        // msg
        mem_alloc_msg(addr, size + 1);

        // increment and return
        reserved_pos  += size + 1;
        reserved_used += size + 1;
        return (void*)addr;
    }
}

void* mem_alloc_silent(size_t size)
{
    if (size == 0) { System::KernelIO::ThrowWarning("Tried to allocate 0 bytes"); return nullptr; }

    // dynamic allocation mode
    if (dynamic_mode)
    {
        rat_entry_t* entry = get_free_entry(size + 2);

        return (void*)entry->offset;
    }
    // never look back mode
    else
    {
        // calculate
        uint32_t addr = reserved_base + reserved_pos;

        // increment and return
        reserved_pos  += size + 1;
        reserved_used += size + 1;
        return (void*)addr;
    }
}

void mem_free_msg(uint32_t offset, uint32_t size)
{
    // msg
    char temp[24];
    System::KernelIO::Write("[");
    System::KernelIO::Write(" FREE ", COL4_YELLOW);
    System::KernelIO::Write("]");

    System::KernelIO::Write(" offset");
    System::KernelIO::Write(" = 0x");
    strhex32(offset, temp);
    System::KernelIO::Write(temp);

    System::KernelIO::Write("    size", COL4_CYAN);
    System::KernelIO::Write(" = ");
    strdec(size, temp);
    System::KernelIO::WriteLine(temp);
}

void mem_free(void* ptr)
{
    if (ptr == nullptr) { System::KernelIO::ThrowWarning("Tried to free nullptr"); return; }

    if (dynamic_mode)
    {
        // locate entry in table
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            // found match
            if (entry->offset == (uint32_t)ptr)
            {
                if (entry->size == 0) { System::KernelIO::ThrowWarning("Tried to free 0 bytes"); return; }
                mem_fill((uint8_t*)entry->offset, 0, entry->size);
                entry->state = MEM_STATE_FREE;
                mem_free_msg((uint32_t)ptr, entry->size);
                mem_combine_free_entries();
                return;
            }
        }
    }
}

void mem_free_silent(void* ptr)
{
    if (ptr == nullptr) { System::KernelIO::ThrowWarning("Tried to free nullptr"); return; }

    if (dynamic_mode)
    {
        // locate entry in table
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);

            // found match
            if (entry->offset == (uint32_t)ptr)
            {
                if (entry->size == 0) { System::KernelIO::ThrowWarning("Tried to free 0 bytes"); return; }
                mem_fill((uint8_t*)entry->offset, 0, entry->size);
                entry->state = MEM_STATE_FREE;
                mem_combine_free_entries();
                return;
            }
        }
    }
}

rat_entry_t* get_biggest_entry()
{
    rat_entry_t* biggest = nullptr;
    for (size_t i = 0; i < rat_max_entries; i++)
    {
        rat_entry_t* entry = get_entry(i);
        if (biggest == nullptr) { biggest = entry; }
        if (entry->size > biggest->size)
        {
            biggest = entry;
        }
    }
    return biggest;
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
    if (entry == nullptr)   { System::KernelIO::ThrowError("Tried to delete null entry"); return false; }
    if (entry->offset == 0) { System::KernelIO::ThrowError("Tried to delete entry with null offset"); return false; }
    if (entry->size == 0)   { System::KernelIO::ThrowError("Tried to delete entry with size of 0"); return false; }

    // locate entry in table
    for (size_t i = 0; i < rat_max_entries; i++)
    {
        rat_entry_t* temp = get_entry(i);

        if (temp->offset == entry->offset)
        {
            // check if size is valid
            if (entry->size != temp->size) { System::KernelIO::ThrowError("Size mismatch while locating entry for deletion"); return false; }

            // clear dataKernelIO
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
    //System::Debugger::ThrowError("Unable to locate free entry");
    return nullptr;
}

uint32_t mem_get_size_of(void* ptr)
{
   
}

// print ram allocation table
void mem_print_rat()
{
    
}

// get rat entry by index
rat_entry_t* get_entry(int32_t index)
{
    return (rat_entry_t*)(rat_table + (index * sizeof(rat_entry_t)));
}

// get total amount of memory in megabytes
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

// get total amount of memory in bytes
uint32_t mem_get_total() { return mem_get_total_mb() * 1024 * 1024; }
uint32_t mem_get_total_reserved() { return reserved_size; }
uint32_t mem_get_used()
{
    if (dynamic_mode)
    {
        reserved_used = 0;
        for (size_t i = 0; i < rat_max_entries; i++)
        {
            rat_entry_t* entry = get_entry(i);
            if (entry->offset > 0 && entry->size > 0 && entry->state == MEM_STATE_USED)
            {
                reserved_used += entry->size;
            }
        }
    }
    return reserved_used;
}

// memory operations
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

void mem_swap(uint8_t* src, uint8_t* dest, uint32_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (*(dest + i) != *(src + i)) { *(dest + i) = *(src + i); }
    }
}

void mem_fill(uint8_t* dest, uint32_t val, uint32_t len)
{   
    uint32_t num_dwords = len/4;
    uint32_t num_bytes = len%4;
    uint32_t *dest32 = (uint32_t*)dest;
    uint8_t *dest8 = ((uint8_t*)dest)+num_dwords*4;
    uint8_t val8 = (uint8_t)val;
    uint32_t val32 = val|(val<<8)|(val<<16)|(val<<24);
    uint32_t i;

    for (i=0;i<num_dwords;i++) { dest32[i] = val32; }
    for (i=0;i<num_bytes;i++) { dest8[i] = val8; }
}

int mem_compare(const void* a, const void* b, size_t len) 
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