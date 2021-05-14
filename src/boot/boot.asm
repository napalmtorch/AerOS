global loader                           ; making entry point visible to linker
global mbootdat 
extern kernel_main

; setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
VIDEOINFO equ 1<<2
VIDEOMODE equ 1
VIDEOWIDTH equ 2320
VIDEOHEIGHT equ 240
VIDEODEPTH equ 32
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required
 
section .text
section .multiboot  
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd VIDEOMODE
    dd VIDEOWIDTH
    dd VIDEOHEIGHT
    dd VIDEODEPTH
 
; reserve initial kernel stack space
STACKSIZE equ 0x8000                    ; that's 32k.

loader:
    mov  esp, stack + STACKSIZE         ; set up the stack
    push eax                            ; Multiboot magic number
    push ebx                           ; Multiboot info structure
 
    call kernel_main                          ; call kernel proper
 
    cli
.hang:
    hlt                                 ; halt machine should kernel return
    jmp  .hang
 
section .bss
 
align 4
stack:
    resb STACKSIZE                      ; reserve 32k stack on a doubleword boundary
