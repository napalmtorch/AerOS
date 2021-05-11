#!/bin/bash
if [ "$EUID" -ne 0 ] 
    then
    echo "Please run with sudo or as root"
    exit
    else
if [ "$1" == "mount" ]; then
mkdir /mnt/tmp
mount disks/fat32.img /mnt/tmp
echo "Displaying Contents of /mnt/tmp"
ls -al /mnt/tmp
elif [ "$1" == "unmount" ]; then
umount /mnt/tmp
rm -Rf /mnt/tmp
else
echo "Provide 'mount' or 'unmount' as argument"
fi
fi
