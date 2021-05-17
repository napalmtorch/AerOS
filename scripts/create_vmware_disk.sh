#!/bin/bash
BASE_DISK=disks/fat32.img
VMWARE_DISK=disks/fat32.vmdk
if ! command -v qemu-img &> /dev/null
then
    echo "qemu-img could not be found, make sure it is installed"
else
if [[ -f "$BASE_DISK" ]]; 
then
    if [[ -f "$VMWARE_DISK" ]];
    then
    rm -Rf $VMWARE_DISK
    fi
 qemu-img convert -f raw -O vmdk disks/fat32.img disks/fat32.vmdk
 fi
 fi