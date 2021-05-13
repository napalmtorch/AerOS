#include <hardware/interrupt/isr.hpp>
#include <hardware/ports.hpp>
extern "C" {
ISRType InterruptHandlers[256];

void RegisterInterruptHandler(uint8_t n,ISRType Handler)
{
	InterruptHandlers[n] = Handler ;
}


void
ISRHandler(RegistersType Regs)
{
	if (InterruptHandlers[Regs.IntNumber] != 0){
		ISRType Handler = InterruptHandlers[Regs.IntNumber];
		Handler(Regs);
	}
}

void
IRQHandler(RegistersType Regs)
{
	if (InterruptHandlers[Regs.IntNumber] != 0) {
			ISRType Handler = InterruptHandlers[Regs.IntNumber];
			Handler(Regs);
	}
	if (Regs.IntNumber >= 40) {
		outb(0xa0,0x20);
	}
	outb(0x20,0x20);
}
}