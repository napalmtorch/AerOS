rm -R 'bin'
mkdir 'bin'
mkdir 'bin/objs'

# bootloader
as --32 'src/boot/boot.asm' -o 'bin/boot.o'
nasm -felf32 'src/boot/realmode.asm' -o 'bin/objs/rm.o'
nasm -felf32 'src/boot/gdt.asm' -o 'bin/objs/gdt.o'
nasm -felf32 'include/hardware/interrupt/irqs.asm' -o 'bin/objs/irqs.o'

# core
for file in src/core/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -Iinclude -c src/core/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done
#i686-elf-g++ -Iinclude -c 'src/core/entry.cpp' -o 'bin/objs/entry.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/core/kernel.cpp' -o 'bin/objs/kernel.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/core/debug.cpp' -o 'bin/objs/debug.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter

# graphics
for file in src/graphics/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -Iinclude -c src/graphics/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done
#i686-elf-g++ -Iinclude -c 'src/graphics/font.cpp' -o 'bin/objs/font.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter
#i686-elf-g++ -Iinclude -c 'src/graphics/colors.cpp' -o 'bin/objs/colors.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter
#i686-elf-g++ -Iinclude -c 'src/graphics/canvas.cpp' -o 'bin/objs/canvas.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter

# hardware
for file in src/hardware/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -Iinclude -c src/hardware/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done
#i686-elf-g++ -Iinclude -c 'src/hardware/ports.cpp' -o 'bin/objs/ports.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/multiboot.cpp' -o 'bin/objs/mb.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/memory.cpp' -o 'bin/objs/mem.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter
#i686-elf-g++ -Iinclude -c 'src/hardware/terminal.cpp' -o 'bin/objs/term.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter
#i686-elf-g++ -Iinclude -c 'src/hardware/fat.cpp' -o 'bin/objs/fat.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter

# drivers
for file in src/hardware/drivers/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -Iinclude -c src/hardware/drivers/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done
#i686-elf-g++ -Iinclude -c 'src/hardware/drivers/serial.cpp' -o 'bin/objs/serial.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/drivers/ata.cpp' -o 'bin/objs/ata.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/drivers/rtc.cpp' -o 'bin/objs/rtc.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/drivers/vga.cpp' -o 'bin/objs/vga.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/drivers/pci.cpp' -o 'bin/objs/pci.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter
#i686-elf-g++ -Iinclude -c 'src/hardware/drivers/keyboard.cpp' -o 'bin/objs/kb.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable -Wno-unused-parameter

# interrupt
for file in src/hardware/interrupt/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -Iinclude -c src/hardware/interrupt/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done
#i686-elf-g++ -Iinclude -c 'src/hardware/interrupt/idt.cpp' -o 'bin/objs/idt.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/interrupt/isr.cpp' -o 'bin/objs/isr.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
#i686-elf-g++ -Iinclude -c 'src/hardware/interrupt/interrupt.cpp' -o 'bin/objs/int.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable

# lib
for file in src/lib/*.cpp 
do
infile=$(basename $file)
outfile="$(echo $infile | sed 's/cpp/o/g')"
i686-elf-g++ -Iinclude -c src/lib/$infile -o "bin/objs/$outfile" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable
done
#i686-elf-g++ -Iinclude -c 'src/lib/string.cpp' -o 'bin/objs/string.o' -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-write-strings -Wno-unused-variable

# linker
cd 'bin/objs'
i686-elf-gcc -T '../../include/boot/linker.ld' -o '../kernel.bin' -ffreestanding -O2 -nostdlib *.o '../boot.o' -lgcc
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
qemu-system-i386 -m 256M -vga std -hda disk.img -cdrom 'AerOS.iso' -serial stdio -boot d
fi