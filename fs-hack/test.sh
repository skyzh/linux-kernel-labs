#!/bin/bash

dmesg -C
insmod romfs.ko
dmesg
rmmod romfs.ko