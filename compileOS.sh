#!/bin/bash
# Rheeca Guion and Chetana Musunuru, 2023

# bootloader
nasm bootload.asm
dd if=/dev/zero of=diskc.img bs=512 count=1000
dd if=bootload of=diskc.img bs=512 count=1 conv=notrunc

# kernel
bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o

# loadfile
gcc -o loadFile loadFile.c
./loadFile kernel
./loadFile message.txt

# test programs
as86 -o userlib.o userlib.asm

bcc -ansi -c -o tstpr1.o tstpr1.c
ld86 -d -o tstpr1 tstpr1.o userlib.o
./loadFile tstpr1

bcc -ansi -c -o tstpr2.o tstpr2.c
ld86 -d -o tstpr2 tstpr2.o userlib.o
./loadFile tstpr2

bcc -ansi -c -o number.o number.c
ld86 -d -o number number.o userlib.o
./loadFile number

bcc -ansi -c -o letter.o letter.c
ld86 -d -o letter letter.o userlib.o
./loadFile letter

# shell
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o userlib.o
./loadFile shell
