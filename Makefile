
all:
	./build.sh --no-qemu
clean:
	rm -Rf bin/isodir/boot/kernel.bin
	rm -Rf bin/objs/*.o
	rm AerOS.iso
	rm *.log
	rm *.out
iso:
	mkdir -p 'bin/isodir/boot/grub'
	cp 'bin/kernel.bin' 'bin/isodir/boot/kernel.bin'
	cp 'include/boot/grub.cfg' 'bin/isodir/boot/grub/grub.cfg'
	grub-mkrescue -o  'AerOS.iso' 'bin/isodir' -V 'AerOS'
qemu:
	qemu-system-i386 -m 256M -vga std -hda disk.img -cdrom 'AerOS.iso' -serial stdio -boot d

qemu-kernel:
	qemu-system-i386 -m 256M -vga std -hda disk.img -kernel bin/kernel.bin -serial stdio -boot d

bochs:
	$(shell ./bochs.sh)
win:
	.\win_build.bat

fullbochs: clean all iso bochs

fullqemu: clean all iso qemu