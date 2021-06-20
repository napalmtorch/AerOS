# A Operating System build by some Friends, from scratch!

[![Build Status](http://144.76.18.59:8080/buildStatus/icon?job=AerOS)](http://144.76.18.59:8080/job/AerOS/lastSuccessfulBuild/)

if you want to give this a try you can download the last build [here](http://144.76.18.59:8080/job/AerOS/lastSuccessfulBuild/artifact/AerOS.iso)

however to use the filesystem you will need to use our custom .img file which is located [here](https://github.com/napalmtorch/AerOS/blob/main/disks/nfs_disk.img)
since we use a custom Filesystem.

currently AerOS only works on Qemu (and some real hardware but not many)
for launch parameters see [here](https://github.com/napalmtorch/AerOS/blob/ff4d54f197e1df895a1fd6077357afd01a2af3f8/Makefile#L32)


For more launch options check our document [here](https://shorturl.at/iBGX4)

# Compiling Yourself
the main requirement you will need to have installed is `make`
after cloning/downloading the repository the following command will take care of everything for you:
just type `make`
what this will do is the following:
if you run ubuntu or arch and do not have a crosscompiler installed it will prompt you to install all requirements
then it clones the last binutils and gcc from the official repositories, you can see this in scripts/cross.sh
next it will compile the cross compiler for you, this might take some time, so grab a coffee and watch some netflix meanwhile ;)
if however you already have i686-elf-g++ installed and in your PATH we will use that to compile AerOS for you :)
this is a one time operation, once done it will compile AerOS and start it for you.

if you already compiled AerOS and would like to just run it you can simply type `make nfs`
to clean the AerOS build and recompile it type `make clean`
