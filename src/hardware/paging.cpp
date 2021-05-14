#include <hardware/paging.hpp>
#include <lib/types.h>
#include <hardware/mm/kheap.hpp>
#include <hardware/mm/memory_page.hpp>
#include <core/debug.hpp>
#define NULL 0

typedef struct UnusedPage
{
	uint32_t HowMany;
	struct UnusedPage* Next;
} UnusedPageType;

PageDirectoryType* KernelDirectory;
PageDirectoryType* CurrentDirectory = 0;

uint32_t* Frames;
uint32_t NFrames;

UnusedPageType* UnusedPages = NULL;

extern uint32_t PlacementAddress;
static uint32_t FreePage;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void SetFrame(uint32_t FrameAddress)
{
	uint32_t Frame = FrameAddress/0x1000;
	uint32_t Index = INDEX_FROM_BIT(Frame);
	uint32_t Offset = OFFSET_FROM_BIT(Frame);
	Frames[Index] |= (0x1 << Offset);
}

static void ClearFrame(uint32_t FrameAddress)
{
	uint32_t Frame = FrameAddress/0x1000;
	uint32_t Index = INDEX_FROM_BIT(Frame);
	uint32_t Offset = OFFSET_FROM_BIT(Frame);
	Frames[Index] &= ~(0x1 << Offset);
}

static uint32_t TestFrame(uint32_t FrameAddress)
{
	uint32_t Frame = FrameAddress/0x1000;
	uint32_t Index = INDEX_FROM_BIT(Frame);
	uint32_t Offset = OFFSET_FROM_BIT(Frame);
	return Frames[Index] & (0x1 << Offset);
}

static uint32_t FirstFrame()
{
	for ( uint32_t i = 0 ; i < INDEX_FROM_BIT(NFrames) ; i++ ){
		if ( Frames[i] != 0xFFFFFFFF ) {
			for ( uint32_t j = 0 ; j < 32 ; j++ ) {
				if ( !(Frames[i]&(0x1<<j)) ){
					return i*4*8+j;
				}
			}
		}
		
	}
}

void AllocFrame(PageType* Page,int32_t IsKernel,int32_t IsWriteable)
{
	if ( Page -> Frame == 0 ) {
		uint32_t Index = FirstFrame();
		if ( Index == (uint32_t)-1 )
			debug_throw_panic("No free frames");
		SetFrame(Index*0x1000);
		Page->Present = 1;
		Page->ReadWrite = (IsWriteable)?1:0;
		Page->User = (IsKernel)?0:1;
		Page->Frame = Index;
	}
}

void FreeFrame(PageType* Page)
{
	uint32_t Frame;
	if (Frame=Page->Frame) {
		ClearFrame(Frame);
		Page->Frame = 0;
	}
}

void InitialisePaging()
{
	uint32_t MemEndPage = 0x10000000;
	NFrames = MemEndPage / 0x1000;
	Frames = (uint32_t*)Kmalloc(INDEX_FROM_BIT(NFrames));
	memset((uint8_t*)Frames,0,INDEX_FROM_BIT(NFrames));
	KernelDirectory = (PageDirectoryType*)KmallocAligned(sizeof(PageDirectoryType));
	memset((uint8_t*)KernelDirectory,0,sizeof(PageDirectoryType));
	CurrentDirectory = KernelDirectory;
	for ( int i = 0 ; i < PlacementAddress ; i += 0x1000 )
    {
        //debug_writeln_hex("Allocating Frame:",(uint32_t)GetPage(i,1,KernelDirectory));
		AllocFrame(GetPage(i,1,KernelDirectory),0,1);
    }
    
	for ( int i = 0xBF000000 ; i < 0xC0100000 ; i += 0x1000 )
    {
      //  debug_writeln_hex("Allocating Frame 2:",(uint32_t)GetPage(i,1,KernelDirectory));
		AllocFrame(GetPage(i,1,KernelDirectory),0,1);
    }
    debug_bochs_break();
	RegisterInterruptHandler(14,PageFault);
	SwitchPageDirectory(KernelDirectory);
	/* Set Free Page */
	FreePage = 0xC0000000;
}

void SwitchPageDirectory(PageDirectoryType* Directory)
{
    //Enable Paging
	CurrentDirectory = Directory;
	asm volatile("mov %0,%%cr3"::"r"(&(Directory->TablesPhysical)));
	uint32_t cr0;
	asm volatile("mov %%cr0,%0":"=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0,%%cr0"::"r"(cr0));
    debug_writeln_hex("CR0: ", cr0);
    debug_writeln_dec("Physical Tables: ",(uint32_t)&(Directory->TablesPhysical));
    debug_writeln_hex("Physical Tables Hex: ",(uint32_t)&(Directory->TablesPhysical));
    debug_bochs_break(); 
}

PageType* GetPage(uint32_t Address,int32_t Make,PageDirectoryType* Directory)
{
	Address /= 0x1000;
	uint32_t TableIndex = Address / 1024;
	if ( Directory->Tables[TableIndex] )
		return &Directory->Tables[TableIndex]->Pages[Address%1024];
	else if(Make) {
		uint32_t Temp;
		Directory->Tables[TableIndex] =
			(PageTableType*)KmallocAlignedPhysical(sizeof(PageTableType),&Temp);
		memset((uint8_t*)Directory->Tables[TableIndex],0,0x1000);
		Directory->TablesPhysical[TableIndex] = Temp | 0x7;
		return &Directory->Tables[TableIndex]->Pages[Address%1024];
	} else return 0;
}

void PageFault(RegistersType Registers)
{
	uint32_t FaultingAddress;
	asm volatile("mov %%cr2,%0" : "=r" (FaultingAddress));
	int Present = (Registers.ErrorCode & 0x1);
	int ReadWrite = Registers.ErrorCode & 0x2;
	int User = Registers.ErrorCode & 0x4;
	int Reserved = Registers.ErrorCode & 0x8;
	int ID = Registers.ErrorCode & 10;
	debug_writeln_hex("Page Fault: ( ",FaultingAddress);
	if ( Present ) debug_writeln("Present ");
	if ( ReadWrite ) debug_writeln_hex("Read-only ",FaultingAddress);
	if ( User ) debug_writeln("User");
	if ( Reserved ) debug_writeln_dec("Reserved",FaultingAddress);
	PageType* Page = GetPage(FaultingAddress,1,KernelDirectory);
	AllocFrame(Page,1,1);
    debug_bochs_break();
//debug_throw_panic("Page Fault");
return;
}

void
MapPage(uint32_t Address)
{
	AllocFrame(GetPage(Address,1,CurrentDirectory),1,1);
}

void*
FindFreePages(uint32_t HowMany)
{
	UnusedPageType* ret = UnusedPages;
	while(ret->HowMany<HowMany&&ret->Next!=NULL)
		ret = ret->Next;
	return ret;
}

void*
GetFreePages(uint32_t HowMany)
{
	UnusedPageType* ret;
	if ((ret = (UnusedPageType*)FindFreePages(HowMany))!=NULL)
	{
		/* Delete From Unused Page List */
		/* Find Previous One */
		UnusedPageType* Previous = (UnusedPageType*)FreePage;
		while(Previous->Next!=ret)
			Previous = Previous->Next;
		Previous->Next = ret->Next;
		return ret;
	}
	ret = (UnusedPageType*)FreePage;
	FreePage += HowMany*0x1000;
    debug_writeln_hex("Ret:",(uint32_t)ret);
	return ret;
}

void
ReturnPages(void* Address,uint32_t HowMany)
{
	UnusedPageType* ToInsert = (UnusedPageType*)Address;
	ToInsert->HowMany = HowMany;
	UnusedPageType* Previous = (UnusedPageType*)FreePage;
	while ( Previous->Next->HowMany < HowMany && Previous->Next!=NULL )
		Previous = Previous->Next;
	ToInsert->Next = Previous->Next;
	Previous->Next = ToInsert;
}
