#!/bin/bash
file_t=serial.out
DISK=disks/disk.img
DISK_LOCK_FILE=disks/disk.img.lock 
if ! command -v bochs &> /dev/null
then
    echo "bochs could not be found, installing...."
    if [ "$EUID" -ne 0 ] 
    then
    echo "Please run with sudo or as root"
    exit
    else
    apt install bochs bochs-x vgabios bochsbios
    exit
    fi
    exit
else
if ! command -v xterm &> /dev/null
then
echo "xterm not found,trying to install"
if [ "$EUID" -ne 0 ] 
    then
    echo "Please run with sudo or as root"
    exit
    else
    apt install xterm
    fi
fi
if [ ! -e "$DISK" ]; then
dd if=/dev/zero of=disk.img bs=1M count=32
fi
if [ -f "$file_t" ]; then
    rm serial.out
    touch serial.out
else
touch serial.out
fi    
if [[ -f "$DISK_LOCK_FILE" ]]; 
then
echo "Found Lock File"
rm -f disks/disk.img.lock
fi
if [ "$1" == "--gnome-terminal" ]; then
gnome-terminal -- bochs -q -f ../configs/bochsrc.txt &
else
xterm -e /usr/bin/bochs -q -f ./configs/bochsrc.txt 
fi
fi
