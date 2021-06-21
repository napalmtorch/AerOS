#!/bin/bash
export BASE_DIR=Toolchain/temp
export PREFIX="$PWD/Toolchain/Cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
install_requirements()
{
echo "Attempting to install requirements"
if [  -n "$(uname -a | grep Ubuntu)" ]; then
sudo apt-get -y install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
elif [  -n "$(uname -a | grep arch)" ]; then
sudo pacman -S --noconfirm base-devel gmp libmpc mpfr
else
echo "Your distribution is currently not supported"
fi
prep_dirs
}

clone_binutils()
{
if [ -d "binutils-gdb" ]; then
echo "Already cloned Binutils"
else
echo "Cloning Binutils" 
git clone git://sourceware.org/git/binutils-gdb.git
fi
clone_gcc
}

clone_gcc()
{
if [ -d "gcc" ]; then
echo "Already cloned GCC"
else
echo "Cloning GCC"
git clone git://gcc.gnu.org/git/gcc.git
fi
build_binutils
}

prep_dirs()
{
    if [ -d $BASE_DIR ]; then
        cd $BASE_DIR && pwd
    else
        mkdir $BASE_DIR && cd $BASE_DIR && pwd
    fi
        clone_binutils
}

build_binutils()
{
echo "Building Binutils"
if [ -d "build_binutils" ]; then
cd build_binutils
else
mkdir build_binutils && cd build_binutils
fi
if [ ! -f "$PWD/Toolchain/Cross/bin/i686-elf-as" ]; then
../binutils-gdb/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install
fi
build_gcc
}

build_gcc()
{
echo "Building GCC"
if [ -d "../build_gcc" ]; then
cd ../build_gcc
else
mkdir ../build_gcc && cd ../build_gcc
fi
if [ ! -f "$PWD/Toolchain/Cross/bin/i686-elf-g++" ]; then
../gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
fi
cleanup
}

cleanup()
{
echo "Cleaning up"
rm -Rf $PWD/Toolchain/temp
finish
}

welcome()
{

    echo "Welcome to the AerOS Cross Compiler Setup"
    read -p "Do you wish to build the Cross Compiler? " -n 1 -r
    echo 
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
    install_requirements
    else
        echo "Aborting..... :("
        exit
    fi
}

finish()
{
    echo "We (hopefully) finished compiling the AerOS Cross compiler"
    echo "dont forget to add '$PWD/Toolchain/Cross/bin' to your PATH"
    echo "For example this can be done by running:"
    echo "'export PATH=$PWD/Toolchain/Cross/bin:\$PATH'"
    exit
}

welcome