rm -R 'bin'
mkdir 'bin'
mkdir 'bin/objs'

# create iso
mkdir -p 'bin/isodir/boot/grub'
cp 'bin/kernel.bin' 'bin/isodir/boot/kernel.bin'
cp 'include/boot/grub.cfg' 'bin/isodir/boot/grub/grub.cfg'
grub-mkrescue -o  'AerOS.iso' 'bin/isodir'

# run 
qemu-system-i386 -m 256M -vga std -cdrom 'AerOS.iso' -serial stdio -boot d