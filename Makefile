
all:
	./scripts/build.sh --no-qemu
clean:

	rm -Rf bin/isodir

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

iso:
	mkdir -p bin/isodir/boot
	cp bin/kernel.bin bin/isodir/boot/
	grub-mkrescue -d /usr/lib/grub/i386-pc --compress=xz -o AerOS.iso cdrom 'bin/isodir/boot/kernel.bin' -V 'AerOS'
	
qemu:
	gnome-terminal -- /usr/bin/qemu-system-i386 -m 256M -vga std -drive file=disks/disk.img,format=raw -cdrom 'AerOS.iso' -serial stdio -boot d -soundhw all -device e1000 -enable-kvm -cpu host -name "AerOS Development Copy"

qemu-kernel:
	qemu-system-i386 -m 256M -vga std -drive file=disks/disk.img,format=raw -kernel bin/kernel.bin -serial stdio -boot d -soundhw all -enable-kvm -cpu host -name "AerOS Development Copy"

bochs:
	 ./scripts/bochs.sh
bochs-gnome:
	./scripts/bochs.sh --gnome-terminal

bochsn:
	./scripts/bochs_nodebug.sh

bochsn-gnome:
	./scripts/bochs_nodebug.sh --gnome-terminal

lazy-will: clean all iso bochsn

win:
	.\win_build.bat

fullbochs: clean all iso bochs

fullqemu: clean all iso qemu

fullqemuk: clean all qemu-kernel