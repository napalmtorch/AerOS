.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM
    .skip 20


.section .text
.extern kernel_main
.extern gdt_enable
.global loader
.global kernel_stack
.global kern_mem
.global long_mem
.global _stop
loader:
    mov %esp, kernel_stack
    mov %ebx, mbootdat
    call gdt_enable


_stop:
    cli
    hlt
    jmp _stop

.section .bss
.global mbootdat
kernel_stack:
.space 2*1024*1024
long_mem:
.global kernel_begin
kernel_begin:
kern_mem:

.section .kend
mbootdat:
.space 36
.global kernel_end
kernel_end:
.section .text
