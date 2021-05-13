#pragma once
extern "C" {

#include <lib/types.h>

typedef struct Registers
{
	uint32_t ds;
	uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
	uint32_t IntNumber,ErrorCode;
	uint32_t eip,cs,eflags,useresp,ss;
} RegistersType;

typedef void (*ISRType) (RegistersType);
void RegisterInterruptHandler(uint8_t,ISRType);

}
