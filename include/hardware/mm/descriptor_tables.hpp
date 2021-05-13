#pragma once

#include <lib/types.h>
extern "C" {
struct GDTEntryStruct 
{
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t BaseMiddle;
	uint8_t Access;
	uint8_t Granularity;
	uint8_t BaseHigh;
} __attribute__((packed));
typedef struct GDTEntryStruct GDTEntryType;

struct GDTPointerStruct
{
	uint16_t Limit;
	uint32_t Base;
} __attribute__((packed));
typedef struct GDTPointerStruct GDTPointerType;

struct IDTEntryStruct
{
	uint16_t BaseLow;
	uint16_t Selector;
	uint8_t Always0;
	uint8_t Flags;
	uint16_t BaseHigh;
} __attribute__((packed));
typedef struct IDTEntryStruct IDTEntryType;

struct IDTPointerStruct
{
	uint16_t Limit;
	uint32_t Base;
} __attribute__((packed));
typedef struct IDTPointerStruct IDTPointerType;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
void InitDescriptorTables();
}
