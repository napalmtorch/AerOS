CXX=i686-elf-g++
LD=i686-elf-gcc
CXXFLAGS=-Iinclude -fno-use-cxa-atexit -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -w -Wno-narrowing -Wno-sign-compare -Wno-type-limits -Wno-unused-parameter -Wno-missing-field-initializers
NASM=nasm
ASFLAGS=-felf32
SOURCES=$(shell find . -name '*.cpp')
LINK_SOURCES=$(shell find . -name '*.o')
CPP_FILES_OUT = $(SOURCES:.cpp=.o)
.PHONY: all as clean link iso run bochs nfs

all: as $(CPP_FILES_OUT) link iso nfs clean

as:
	as --32 'src/boot/boot.asm' -o 'src/boot/boot.o'
	nasm -felf32 'src/boot/realmode.asm' -o 'src/boot/rm.o'
	nasm -felf32 'src/boot/gdt.asm' -o 'src/boot/gdt.o'
	nasm -felf32 'include/hardware/interrupt/irqs.asm' -o 'include/hardware/interrupt/irqs.o'

link:
	$(LD) -w -T 'include/boot/linker.ld' -o 'kernel.bin' -ffreestanding -O2 -nostdlib $(LINK_SOURCES) -lgcc

clean:
	-rm -Rf $(shell find . -name '*.o') $(shell find . -name '*.bin') $(shell find . -name '*.iso')  

iso:
	mkdir -p bin/isodir/boot
	cp kernel.bin bin/isodir/boot/
	grub-mkrescue -d /usr/lib/grub/i386-pc --compress=xz -o AerOS.iso cdrom 'bin/isodir/boot/kernel.bin' -V 'AerOS'

nfs:
	qemu-system-i386 -cpu host -m 256M -vga std -hda 'disks/nfs_disk.img' -cdrom 'AerOS.iso' -name 'AerOS Development Copy' -serial stdio -boot d -soundhw all -device e1000 -enable-kvm -rtc base=localtime

run:
	qemu-system-i386 -m 256M -vga std -drive file=disks/fat32.img,format=raw -cdrom 'AerOS.iso' -serial stdio -boot d -rtc base=localtime -soundhw all -device e1000 -enable-kvm -cpu host -name "AerOS Development Copy"

bochs:
	/usr/bin/bochs -q -f ./configs/bochsrc.txt 