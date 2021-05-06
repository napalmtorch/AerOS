#!/bin/bash
FILE=serial.out
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
if [ -f "$FILE" ]; then
    rm serial.out
    touch serial.out
    exit
else
touch serial.out
fi    
xterm -e tail -f serial.out > out.log 2> err.log & 
bochs -q -f bochsrc.txt
fi
