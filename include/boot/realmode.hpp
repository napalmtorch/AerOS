#pragma once
struct regs16_t 
{
	unsigned short di, si, bp, sp, bx, dx, cx, ax;
	unsigned short gs, fs, es, ds, eflags;
} __attribute__((packed));

extern "C" void int32(unsigned char interrupt, regs16_t* regs);