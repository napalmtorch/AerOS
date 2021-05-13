#include <hardware/mm/descriptor_tables.hpp>
#include <hardware/mm/memory_page.hpp>
#include <lib/types.h>
#include <hardware/ports.hpp>
#include <hardware/interrupt/isr.hpp>
extern "C" {
extern void GDTFlush(uint32_t);
extern void IDTFlush(uint32_t);

static void InitGDT();
static void GDTSetGate(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);
static void InitIDT();
static void IDTSetGate(uint8_t,uint32_t,uint16_t,uint8_t);

GDTEntryType GDTEntries[5];
GDTPointerType GDTPointer;

IDTEntryType IDTEntries[256];
IDTPointerType IDTPointer;

extern ISRType InterruptHandlers[];

void InitDescriptorTables()
{
	InitGDT();
	InitIDT();
	memset((uint8_t*)&InterruptHandlers,0,sizeof(ISRType)*256);
}

static void InitGDT()
{
	GDTPointer.Limit = (sizeof(GDTEntryType)*5)-1;
	GDTPointer.Base = (uint32_t)&GDTEntries;
	GDTSetGate(0,0,0,0,0);
	GDTSetGate(1,0,0xffffffff,0x9a,0xcf);
	GDTSetGate(2,0,0xffffffff,0x92,0xcf);
	GDTSetGate(3,0,0xffffffff,0xfa,0xcf);
	GDTSetGate(4,0,0xffffffff,0xf2,0xcf);
	GDTFlush((uint32_t)&GDTPointer);
}

static void GDTSetGate(int32_t Number,uint32_t Base,
		       uint32_t Limit,uint8_t Access,uint8_t Gran)
{
	GDTEntries[Number].BaseLow = (Base & 0xffff);
	GDTEntries[Number].BaseMiddle = (Base>>16)&0xff;
	GDTEntries[Number].BaseHigh = (Base>>24)&0xff;
	GDTEntries[Number].LimitLow = (Limit&0xffff);
	GDTEntries[Number].Granularity = (Limit>>16)&0x0f;
	GDTEntries[Number].Granularity |= Gran & 0xf0;
	GDTEntries[Number].Access = Access;
}

static void InitIDT()
{
	IDTPointer.Limit = sizeof(IDTEntryType)*256-1;
	IDTPointer.Base = (uint32_t)&IDTEntries;
	memset((uint8_t*)&IDTEntries,0,sizeof(IDTEntryType)*256);
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
	IDTSetGate(0,(uint32_t)isr0,0x08,0x8e);
	IDTSetGate(1,(uint32_t)isr1,0x08,0x8e);
	IDTSetGate(2,(uint32_t)isr2,0x08,0x8e);
	IDTSetGate(3,(uint32_t)isr3,0x08,0x8e);
	IDTSetGate(4,(uint32_t)isr4,0x08,0x8e);
	IDTSetGate(5,(uint32_t)isr5,0x08,0x8e);
	IDTSetGate(6,(uint32_t)isr6,0x08,0x8e);
	IDTSetGate(7,(uint32_t)isr7,0x08,0x8e);
	IDTSetGate(8,(uint32_t)isr8,0x08,0x8e);
	IDTSetGate(9,(uint32_t)isr9,0x08,0x8e);
	IDTSetGate(10,(uint32_t)isr10,0x08,0x8e);
	IDTSetGate(11,(uint32_t)isr11,0x08,0x8e);
	IDTSetGate(12,(uint32_t)isr12,0x08,0x8e);
	IDTSetGate(13,(uint32_t)isr13,0x08,0x8e);
	IDTSetGate(14,(uint32_t)isr14,0x08,0x8e);
	IDTSetGate(15,(uint32_t)isr15,0x08,0x8e);
	IDTSetGate(16,(uint32_t)isr16,0x08,0x8e);
	IDTSetGate(17,(uint32_t)isr17,0x08,0x8e);
	IDTSetGate(18,(uint32_t)isr18,0x08,0x8e);
	IDTSetGate(19,(uint32_t)isr19,0x08,0x8e);
	IDTSetGate(20,(uint32_t)isr20,0x08,0x8e);
	IDTSetGate(21,(uint32_t)isr21,0x08,0x8e);
	IDTSetGate(22,(uint32_t)isr22,0x08,0x8e);
	IDTSetGate(23,(uint32_t)isr23,0x08,0x8e);
	IDTSetGate(24,(uint32_t)isr24,0x08,0x8e);
	IDTSetGate(25,(uint32_t)isr25,0x08,0x8e);
	IDTSetGate(26,(uint32_t)isr26,0x08,0x8e);
	IDTSetGate(27,(uint32_t)isr27,0x08,0x8e);
	IDTSetGate(28,(uint32_t)isr28,0x08,0x8e);
	IDTSetGate(29,(uint32_t)isr29,0x08,0x8e);
	IDTSetGate(30,(uint32_t)isr30,0x08,0x8e);
	IDTSetGate(31,(uint32_t)isr31,0x08,0x8e);
	IDTSetGate(32,(uint32_t)irq0,0x08,0x8e);
	IDTSetGate(33,(uint32_t)irq1,0x08,0x8e);
	IDTSetGate(34,(uint32_t)irq2,0x08,0x8e);
	IDTSetGate(35,(uint32_t)irq3,0x08,0x8e);
	IDTSetGate(36,(uint32_t)irq4,0x08,0x8e);
	IDTSetGate(37,(uint32_t)irq5,0x08,0x8e);
	IDTSetGate(38,(uint32_t)irq6,0x08,0x8e);
	IDTSetGate(39,(uint32_t)irq7,0x08,0x8e);
	IDTSetGate(40,(uint32_t)irq8,0x08,0x8e);
	IDTSetGate(41,(uint32_t)irq9,0x08,0x8e);
	IDTSetGate(42,(uint32_t)irq10,0x08,0x8e);
	IDTSetGate(43,(uint32_t)irq11,0x08,0x8e);
	IDTSetGate(44,(uint32_t)irq12,0x08,0x8e);
	IDTSetGate(45,(uint32_t)irq13,0x08,0x8e);
	IDTSetGate(46,(uint32_t)irq14,0x08,0x8e);
	IDTSetGate(47,(uint32_t)irq15,0x08,0x8e);
	IDTFlush((uint32_t)&IDTPointer);
}

static void IDTSetGate(uint8_t Number,uint32_t Base,uint16_t Selector,uint8_t Flags)
{
	IDTEntries[Number].BaseLow = Base & 0xffff;
	IDTEntries[Number].BaseHigh = (Base>>16)&0xffff;
	IDTEntries[Number].Selector = Selector;
	IDTEntries[Number].Always0 = 0;
	IDTEntries[Number].Flags = Flags;
}
}
