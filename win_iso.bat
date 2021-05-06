mkdir -p bin\isodir\boot\grub
copy bin\kernel.bin bin\isodir\boot\kernel.bin
copy include\boot\grub.cfg bin\isodir\boot\grub\grub.cfg
wsl grub-mkrescue -o  'AerOS.iso' 'bin/isodir' -V 'AerOS'