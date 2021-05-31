#!/bin/bash

set -xe

dmesg -C
insmod patch_syscall.ko
./test.o
dmesg
dmesg -C
./bench.o
dmesg
rmmod patch_syscall.ko
