# Lab 3 - Memory Hack

In this lab, we use `/proc/mtest` to add 3 functionalities to kernel.

* Get VMA of a process
* Find virt-phys translation of VMA in kernel
* Write a value to given virtual address

Which is done both in kernel space and user space, with THP support.

This module is tested on Linux 4.15.0.
