#include <hardware/mm/kheap.hpp>
#include <hardware/paging.hpp>
#include <hardware/mm/heap_ifce.hpp>
#include <hardware/interrupt/isr.hpp>

#define NULL 0

extern uint32_t kernel_end;
uint32_t PlacementAddress = (uint32_t)&kernel_end + (1 * 1024 * 1024);

bool HeapInited = false;

void
HeapInit()
{
	heap_init((get_free_pages_t)GetFreePages,NULL);
	HeapInited = true;
}

uint32_t
KmallocInternal(uint32_t Size,int32_t Align,uint32_t* Physical)
{
	if ( Align == 1 && (PlacementAddress & 0xFFFFF000) ){
		PlacementAddress &= 0xFFFFF000;
		PlacementAddress += 0x1000;
	}
	if ( Physical ) *Physical = PlacementAddress;
	uint32_t ret = PlacementAddress;
	PlacementAddress += Size;
	return ret;
}	

uint32_t
KmallocAligned(uint32_t Size)
{
	return KmallocInternal(Size,1,0);
}

uint32_t
KmallocPhysical(uint32_t Size,uint32_t* Physical)
{
	return KmallocInternal(Size,0,Physical);
}

uint32_t
KmallocAlignedPhysical(uint32_t Size ,uint32_t* Physical)
{
	return KmallocInternal(Size,1,Physical);
}

uint32_t
Kmalloc(uint32_t Size)
{
	if (!HeapInited) return KmallocInternal(Size,0,0);
	else return (uint32_t)heap_malloc(Size);
}

void
KFree(void* ToFree)
{
	return heap_free(ToFree);
}
