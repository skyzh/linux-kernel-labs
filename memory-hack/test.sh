#!/bin/bash

dmesg -C
insmod module_memory.ko
echo listvma > /proc/mtest
echo "--- Kernel Module Result ---"
dmesg
echo "--- User Space Result ---"
cat /proc/$$/maps

TRY_ADDR=$(dmesg | ./parse.py)
echo Find mapping of $TRY_ADDR
dmesg -C
echo findpage $TRY_ADDR > /proc/mtest
echo "--- Kernel Module Result ---"
dmesg
echo "--- User Space Result ---"
./pagemap $$ $TRY_ADDR

dmesg -C
echo Write val to $TRY_ADDR
echo writeval $TRY_ADDR ffffffff > /proc/mtest
echo "--- Kernel Module Result ---"
dmesg

rmmod module_memory.ko
