#!/bin/bash
# bootloader
as --32 'src/boot/boot.asm' -o 'bin/boot.o'
nasm -felf32 'src/boot/realmode.asm' -o 'bin/objs/rm.o'
nasm -felf32 'src/boot/gdt.asm' -o 'bin/objs/gdt.o'
nasm -felf32 'include/hardware/interrupt/irqs.asm' -o 'bin/objs/irqs.o'

for file in src/core/*.cpp
do
 files=${file//"src/core/"/"bin/objs/"}
 ext=${files//".cpp"/".o"}
      i686-elf-g++ -Iinclude -c $file -o $ext -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

for file in src/graphics/*.cpp
do
 files=${file//"src/graphics/"/"bin/objs/"}
 ext=${files//".cpp"/".o"}
      i686-elf-g++ -Iinclude -c $file -o $ext -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

for file in src/hardware/*.cpp
do
 files=${file//"src/hardware/"/"bin/objs/"}
 ext=${files//".cpp"/".o"}
      i686-elf-g++ -Iinclude -c $file -o $ext -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

for file in src/lib/*.cpp
do
 files=${file//"src/core/"/"bin/objs/"}
 ext=${files//".cpp"/".o"}
      i686-elf-g++ -Iinclude -c $file -o $ext -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

cd 'bin/objs'
i686-elf-gcc -T '../../include/boot/linker.ld' -o '../kernel.bin' -ffreestanding -O2 -nostdlib *.o '../boot.o' -lgcc
cd '../../'