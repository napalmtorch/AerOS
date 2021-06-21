#!/bin/bash

#AerOS Prerequirement installer
#this script will install packages which are needed for compiling AerOS
#and are not already covered from the toolchain build process

UBUNTU_MANAGER=apt-get
ARCH_MANAGER=pacman
ARCH_PARAMS="-S --noconfirm"
UBUNTU_PARAMS="install -y"
UBUNTU_PACKAGES=""
ARCH_PACKAGES=""
install_requirements()
{
    if [  -n "$(uname -a | grep Ubuntu)" ]; then
    echo "Checking packages"
    if ! command -v xorriso &> /dev/null
    then
    UBUNTU_PACKAGES+="xorriso "
    fi
    if ! command -v qemu-system-i386 &> /dev/null
    then
    UBUNTU_PACKAGES+="qemu-kvm "
    fi
    if ! command -v nasm &> /dev/null
    then
    UBUNTU_PACKAGES+="nasm "
    fi
    if ! command -v grub-mkrescue &> /dev/null
    then
    UBUNTU_PACKAGES+="grub "
    fi
    if ! command -v make &> /dev/null
    then
    UBUNTU_PACKAGES+="build-essential make "
    fi
    if ! command -v doxygen &> /dev/null
    then
    UBUNTU_PACKAGES+="doxygen "
    fi
    if [ -z "$UBUNTU_PACKAGES" ]
    then
        echo "Congrats! There are no prerequisites that need to be installed!"
    else
    echo "We will now run sudo $UBUNTU_MANAGER install -y $UBUNTU_PACKAGES"
        read -p "Do you want to continue? " -n 1 -r
    echo 
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
    sudo $UBUNTU_MANAGER $UBUNTU_PARAMS $UBUNTU_PACKAGES
    else
    echo "Aborting install on your Request!"
    fi
    fi
    elif [  -n "$(uname -a | grep arch)" ]; then
    echo "Checking Packages"
    if ! command -v xorriso &> /dev/null
    then
    ARCH_PACKAGES+="xorriso "
    fi
    if ! command -v nasm &> /dev/null
    then
    ARCH_PACKAGES+=" nasm "
    fi
    if ! command -v grub-mkrescue &> /dev/null
    then
    ARCH_PACKAGES+=" grub "
    fi
    if ! command -v make &> /dev/null
    then
    ARCH_PACKAGES+=" make"
    fi
    if ! command -v doxygen &> /dev/null
    then
    ARCH_PACKAGES+="doxygen "
    fi
    if ! command -v qemu-system-i386 &> /dev/null
    then
    ARCH_PACKAGES+="qemu qemu-arch-extra "
    fi
    if [ -z "$ARCH_PACKAGES" ]
    then
        echo "Congrats! no prerequirements need to be installed!"
    else
    echo "We will now run sudo $ARCH_MANAGER -S --noconfirm $ARCH_PACKAGES"
        read -p "Do you want to continue? " -n 1 -r
    echo 
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
    sudo $ARCH_MANAGER $ARCH_PARAMS $ARCH_PACKAGES
    else
    echo "Aborting install on your Request!"
    fi
    fi
    else
    echo "Your distribution is currently not supported"
    fi
    echo -e "Dont forget to run \e[32mmake toolchain \e[39mto build the toolchain if you have not already done so :)"
}
install_requirements