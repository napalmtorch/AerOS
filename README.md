# A Operating System build by some Friends, from scratch!

[![Build Status](http://144.76.18.59:8080/buildStatus/icon?job=AerOS)](http://144.76.18.59:8080/job/AerOS/lastSuccessfulBuild/)

if you want to give this a try you can download the last build [here](http://144.76.18.59:8080/job/AerOS/lastSuccessfulBuild/artifact/AerOS.iso)

however to use the filesystem you will need to use our custom .img file which is located [here](https://github.com/napalmtorch/AerOS/blob/main/disks/nfs_disk.img)
since we use a custom Filesystem.

currently AerOS only works on Qemu (and some real hardware but not many)
for launch parameters see [here](https://github.com/napalmtorch/AerOS/blob/ff4d54f197e1df895a1fd6077357afd01a2af3f8/Makefile#L32)


For more launch options check our document [here](https://shorturl.at/iBGX4)


# Compiling yourself

## We assume a Linux system here, while WSL is known to work you will not be able to get the full performance of AerOS
## AerOS has been tested on Ubuntu and Arch Linux, if you could run it on another Distribution please extend this list in form of a Pull Request

1. First clone or download this repository!
     If you intend to keep up to date with the source you need to have `git` installed
     on a *buntu based System simply run `sudo apt install git`
     on Arch Linux run `sudo pacman -S git`
     then run `git clone https://github.com/napalmtorch/AerOS.git`

2. Now you will need to go into the just cloned/downloaded directory.
     Assuming you kept the default name you can do this by running `cd AerOS`

3. The fun part starts, lets install some requirements!
     We provide a easy install script for *buntu and Arch Systems.
     just run `bash scripts/install_requirements.sh`
     this will install everything that is needed to build the iso and run make.
     if any of the packages we need is already installed on your system we will skip it.

4. Build the Toolchain
     Once step 3 is finished you can run `make toolchain` in the AerOS main folder
     this will take some time depending on your System. 
     This will also install some requirements that are needed to compile the Toolchain!
     Go grab a Coffee or some Tea!

5. Finally done
     now all you need to do is type `make`
     this will compile AerOS and run it for you in QEMU , on boot in the grub menu either choose:
    ⋅⋅1. "AerOS VESA" to boot into GUI mode
    ⋅⋅2. "AerOS Serial output Only" to boot into Terminal mode and print all debug messages to the console

6. Extras!
     if you need a overview of the APIs you can run `make docs`
     this will provide a html output of the available APIs which are stored in Docs/html/ 
     simply open `index.html` from there in your preferred Browser :)