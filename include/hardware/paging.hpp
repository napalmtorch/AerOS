#pragma once

#include <hardware/interrupt/isr.hpp>
extern "C" {
typedef struct Page
{
	uint32_t Present : 1;
	uint32_t ReadWrite : 1;
	uint32_t User : 1;
	uint32_t Accessed : 1;
	uint32_t Dirty : 1;
	uint32_t Unused : 7;
	uint32_t Frame : 20;
} PageType;

typedef struct PageTable
{
	PageType Pages[1024];
} PageTableType;

typedef struct PageDirectory
{
	PageTableType* Tables[1024];
	uint32_t TablesPhysical[1024];
	uint32_t PhysicalAddress;
} PageDirectoryType;

void InitialisePaging();
void SwitchPageDirectory(PageDirectoryType* NewDirectory); 
PageType* GetPage(uint32_t Address,int Make,PageDirectoryType* Directory);
void PageFault(RegistersType Registers);
void MapPage(uint32_t Address);
void* GetFreePages(uint32_t HowMany);
void ReturnPages(void* Address,uint32_t HowMany);
void* virt2phys(uint32_t addr);

}
	
