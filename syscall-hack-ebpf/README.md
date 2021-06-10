# Lab 5: Syscall Hack

In fact, we do not even need a kernel module to do this. A one-line
eBPF script could do everything for us.

```
sudo bpftrace -e 'tracepoint:syscalls:sys_enter_clone { printf("clone hacked by %s (%d)\n", comm, pid); }'
```

Or

```
sudo ./hack.py
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
