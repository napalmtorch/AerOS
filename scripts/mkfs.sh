#!/bin/bash
    if [ "$EUID" -ne 0 ] 
    then
    echo "Please run with sudo or as root"
    exit
    else
    fat_create_dir /mnt/tmp
    mount disks/fat32.img /mnt/tmp/
    cp -Rfv base/* /mnt/tmp/
    umount /mnt/tmp/
    rm -Rf /mnt/tmp
    exit
    fi