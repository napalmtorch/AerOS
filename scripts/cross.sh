#!/bin/bash
export PREFIX="$PWD/toolchain"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export CUR_PATH="$PWD/temp"
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
check_or_install_cross_compiler()
{
    if command -v i686-elf-g++ &> /dev/null
    then
    global export PATH="$PREFIX/bin:$PATH"
    echo $PATH
    echo -e "${GREEN}Starting Compile${NC}"
    else
    if [ -d "$PREFIX/bin" ]; then
    echo -e "${RED}Crosscompiler already compiled , skipping${NC}"
    else
    read -p "No Toolchain detected, would you like to install it? (y/N) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
    install_requirements
    build_binutils
    build_gcc
    cleanup
    else
    echo "Skipping CC compile"
    fi
    fi
    fi
}

cleanup()
{
    echo -e "${GREEN} Cleaning up....${NC}"
    rm -Rf ./temp
}

install_requirements()
{
    echo -e "${GREEN}Checking and Installing Requirements${NC}"
    if [  -n "$(uname -a | grep Ubuntu)" ]; then
        sudo apt-get install -y git subversion build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libcloog-isl-dev libisl-dev qemu-kvm
    elif [ -f "/etc/arch-release" ]; then
        sudo pacman -S --noconfirm base-devel gmp libmpc mpfr qemu qemu-arch-extra
    else
        echo -e "${RED}Distribution currently not supported by the AerOS Crosscompiler Script!${NC}"
    fi
}
build_binutils()
{
  echo -e "${GREEN}Cloning and Compiling Binutils${NC}"
  mkdir ${CUR_PATH} && cd ${CUR_PATH} && git clone git://sourceware.org/git/binutils-gdb.git
  mkdir build-binutils
  cd build-binutils && ../binutils-gdb/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
  make -j$(nproc) && make install
}
build_gcc()
{
    echo -e "${GREEN}Cloning and Compiling GCC${NC}"
  if [ -d "${CUR_PATH}" ]; then
    cd ${CUR_PATH} && git clone git://gcc.gnu.org/git/gcc.git
    else
    mkdir ${CUR_PATH} && cd ${CUR_PATH} && git clone git://gcc.gnu.org/git/gcc.git
    fi
    mkdir build-gcc
    cd build-gcc && ../gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
    make && all-gcc -j$(nproc) && make all-target-libgcc -j$(nproc) && make install-gcc && make install-target-libgcc
}
check_or_install_cross_compiler