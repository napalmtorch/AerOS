#pragma once

#include <lib/types.h>
extern "C" {
typedef struct
{
	uint32_t Magic;
	uint32_t Size;
	bool InUse;
} BlockHeader;

//void HeapInit();
uint32_t KmallocInternal(uint32_t Size,int32_t Aligned,uint32_t* Physical);
uint32_t KmallocAligned(uint32_t Size);
uint32_t KmallocPhysical(uint32_t Size,uint32_t* Physical);
uint32_t KmallocAlignedPhysical(uint32_t Size,uint32_t* Physical);
uint32_t Kmalloc(uint32_t Size);
void HeapInit();
void KFree(void* ToFree);
}