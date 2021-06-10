# Lab 5: Syscall Hack

In fact, we do not even need a syscall to do this. A one-line
eBPF script could do everything for us. You may also write it
into a C program. But here we just make it simple by using a
one-liner.

```
sudo bpftrace -e 'tracepoint:syscalls:sys_enter_clone { printf("clone hacked by %s (%d)\n", comm, pid); }'
```

## Result

```
Attaching 1 probe...
clone hacked 583326 node
clone hacked 586581 sh
clone hacked 586581 sh
clone hacked 583326 node
clone hacked 586584 sh
clone hacked 586584 sh
```
