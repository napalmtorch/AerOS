
all:
	./build.sh --no-qemu
clean:
	for file in $(wildcard bin/isodir/boot/*.bin) ; do \
		rm -Rf $$file ; \
	done

	for file in $(wildcard bin/objs/*.o) ; do \
		rm -Rf $$file ; \
	done

	for file in $(wildcard bin/*.o) ; do \
		rm -Rf $$file ; \
	done

	for file in $(wildcard *.iso) ; do \
		rm $$file ; \
	done

	for file in $(wildcard *.log) ; do \
		rm $$file ; \
	done

	for file in $(wildcard *.log) ; do \
		rm $$file ; \
	done

iso:
	mkdir -p 'bin/isodir/boot/grub'
	cp 'bin/kernel.bin' 'bin/isodir/boot/kernel.bin'
	cp 'include/boot/grub.cfg' 'bin/isodir/boot/grub/grub.cfg'
	grub-mkrescue -o  'AerOS.iso' 'bin/isodir' -V 'AerOS'
qemu:
	qemu-system-i386 -m 256M -vga std -hda disk.img -cdrom 'AerOS.iso' -serial stdio -boot d -soundhw all -device e1000 -enable-kvm

qemu-kernel:
	qemu-system-i386 -m 256M -vga std -hda disk.img -kernel bin/kernel.bin -serial stdio -boot d -soundhw all -enable-kvm

bochs:
	$(shell ./bochs.sh)
bochs-gnome:
	$(shell ./bochs.sh --gnome-terminal)

bochsn:
	./bochs_nodebug.sh

bochsn-gnome:
	./bochs_nodebug.sh --gnome-terminal

lazy-will: clean all iso bochsn

win:
	.\win_build.bat

fullbochs: clean all iso bochs

fullqemu: clean all iso qemu

fullqemuk: clean all iso qemu-kernel