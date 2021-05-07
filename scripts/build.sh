#!/bin/sh
if [ -d "bin" ]; then
rm -Rf 'bin'
mkdir -p 'bin/objs'
fi
# bootloader
as --32 'src/boot/boot.asm' -o 'bin/objs/boot.o'
nasm -felf32 'src/boot/realmode.asm' -o 'bin/objs/rm.o'
nasm -felf32 'src/boot/gdt.asm' -o 'bin/objs/gdt.o'
nasm -felf32 'include/hardware/interrupt/irqs.asm' -o 'bin/objs/irqs.o'

# core
for file in src/core/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -w -Iinclude -c src/core/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

# graphics
for file in src/graphics/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -w -Iinclude -c src/graphics/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

# hardware
for file in src/hardware/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -w -Iinclude -c src/hardware/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

# drivers
for file in src/hardware/drivers/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -w -Iinclude -c src/hardware/drivers/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

# interrupt
for file in src/hardware/interrupt/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -w -Iinclude -c src/hardware/interrupt/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

# lib
for file in src/lib/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -w -Iinclude -c src/lib/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done

# linker
cd 'bin/objs'
i686-elf-gcc -w -T '../../include/boot/linker.ld' -o '../kernel.bin' -ffreestanding -O2 -nostdlib *.o -lgcc
cd '../../'

options=$(getopt -l "no-qemu" -o "hv:Vrd" -a -- "$@")
eval set -- "$options"

while true
do
    case $1 in
            --no-qemu) 
            qemu="no"
            exit 0
            ;;
            --)
            shift
            break;;
    esac
    shift
done

if [ -n "${qemu+set}" ]; then
exit 0
else
# create iso
mkdir -p 'bin/isodir/boot/grub'
cp 'bin/kernel.bin' 'bin/isodir/boot/kernel.bin'
cp 'include/boot/grub.cfg' 'bin/isodir/boot/grub/grub.cfg'
grub-mkrescue -o  'AerOS.iso' 'bin/isodir' -V 'AerOS'

# run 
qemu-system-i386 -cpu host -m 256M -vga std -hda 'disks/disk.img' -cdrom 'AerOS.iso' -serial stdio -boot d -soundhw all -device e1000 -enable-kvm
fi