rm -R 'bin'
mkdir 'bin'
mkdir 'bin/objs'

# bootloader
as --32 'src/boot/boot.asm' -o 'bin/boot.o'
nasm -felf32 'src/boot/realmode.asm' -o 'bin/objs/rm.o'
nasm -felf32 'src/boot/gdt.asm' -o 'bin/objs/gdt.o'
# nasm -felf32 'include/hardware/interrupt/irqs.asm' -o 'bin/objs/irqs.o'

# c++ files
i686-elf-g++ -Iinclude -c 'src/core/kernel.cpp' -o 'bin/objs/kernel.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable

# linker
cd 'bin/objs'
i686-elf-gcc -T '../../include/boot/linker.ld' -o '../kernel.bin' -ffreestanding -O2 -nostdlib *.o '../boot.o' -lgcc
cd '../../'

# create iso
mkdir -p 'bin/isodir/boot/grub'
cp 'bin/kernel.bin' 'bin/isodir/boot/kernel.bin'
cp 'include/boot/grub.cfg' 'bin/isodir/boot/grub/grub.cfg'
grub-mkrescue -o  'AerOS.iso' 'bin/isodir'

# run 
qemu-system-i386 -m 256M -vga std -cdrom 'AerOS.iso' -serial stdio -boot d

# lol laptop chip but probably still has better single core performance than my CPU right now lmfao
# not sure about multicore tho xDD Keep in mind I also have 16gb ram and a gtx 1650, 500gb ssd, 2tb hdd, 1tb hdd xDD
# you have 4/8 cores, i have 4/4 i no have hyper threading :( xD lmao xD tecnically MAN KEEP CALM xD
# Intel® Core™ i5-6500 CPU @ 3.20GHz × 4 
# AMD ryzen 5 3500U yep lmao it is a x8 tho
# lmao