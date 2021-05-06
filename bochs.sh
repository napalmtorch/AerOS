#!/bin/bash
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
xterm -e tail -f serial.out > out.log 2> err.log & 
bochs -q -f bochsrc.txt > out.log 2> err.log &
fi
