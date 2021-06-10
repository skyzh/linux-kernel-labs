#!/usr/bin/env python3

# Ref: https://rdelfin.com/blog/post_ebpf_002

from bcc import BPF

BPF_PROGRAM = r"""
#include <linux/sched.h>

int trace_clone(void *ctx) {
    struct task_struct *task;
    task = (struct task_struct *) bpf_get_current_task();
    bpf_trace_printk("clone hacked by %s (%d)\n", task->comm, task->pid);
    return 0;
}
"""


bpf = BPF(text=BPF_PROGRAM)
bpf.attach_kprobe(event=bpf.get_syscall_fnname("clone"), fn_name="trace_clone")
while True:
    try:
        (_, _, _, _, _, msg_b) = bpf.trace_fields()
        msg = msg_b.decode('utf8')
        print(msg)
    except ValueError:
        continue
    except KeyboardInterrupt:
        break
