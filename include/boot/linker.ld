ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{

  kernel_start = .;

  . = 1M;

  .dis BLOCK(4K) : ALIGN(4K)
  {
	_ = .;
  }
  
  .text BLOCK(4K) : ALIGN(4K)
  {
    *(.multiboot)
    *(.text*)
  }
  .rodata BLOCK(4K) : ALIGN(4K){
    *(.rodata)
  }
  .data BLOCK(4K) : ALIGN(4K)
  {
    start_ctors = .;
    KEEP(*( .init_array ));
    KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
    end_ctors = .;

    *(.data)
  }

  .bss BLOCK(4K) : ALIGN(4K)
  {
    *(.bss)
  }
  
  .kend BLOCK(4K) : ALIGN(4K)
  {
    *(.kend)
  }
  endKernel = ALIGN(4K);
  /DISCARD/ : { *(.fini_array*) *(.comment) *(.dis) }
}