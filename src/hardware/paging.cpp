#include <lib/types.h>
#include <hardware/paging.hpp>
extern "C"
{
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
extern void loadPageDirectory(unsigned int*);
extern void penis() {};
void SetUpPages()
{
    //set each entry to not present
    int i;
    for(i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x00000002;
    }
    // holds the physical address where we want to start mapping these pages to.
    // in this case, we want to map these pages to the very beginning of memory.
}
void FillPages()
{
        unsigned int i;
    
    //we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
    }
    page_directory[0] = ((unsigned int)first_page_table) | 3;
}
void InitPaging()
{
loadPageDirectory(page_directory);
penis();
}
}