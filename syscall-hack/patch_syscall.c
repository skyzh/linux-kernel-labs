#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex Chi");
MODULE_DESCRIPTION("A simple module which patches syscall");
MODULE_VERSION("0.01");

/* We need to passthrough cr0 changes, otherwise kernel will complain */
/* Ref: https://stackoverflow.com/questions/58512430/how-to-write-to-protected-pages-in-the-linux-kernel */
inline void passthrough_write_cr0(unsigned long cr0)
{
    asm volatile("mov %0,%%cr0"
                 : "+r"(cr0));
}

/* We need to remove memory protect in x86 */
/* Ref: https://infosecwriteups.com/linux-kernel-module-rootkit-syscall-table-hijacking-8f1bc0bd099c */

#define unprotect_memory()                                                      \
    (                                                                           \
        {                                                                       \
            orig_cr0 = read_cr0();                                              \
            passthrough_write_cr0(orig_cr0 &(~0x10000)); /* Set WP flag to 0 */ \
        });

#define protect_memory()                                        \
    ({                                                          \
        passthrough_write_cr0(orig_cr0); /* Set WP flag to 1 */ \
    });

/* sys_clone function pointer type definition */
typedef long(sys_clone_sig)(unsigned long, unsigned long, int __user *,
                            int __user *, unsigned long);

asmlinkage sys_clone_sig *original_sys_clone = NULL;

unsigned long *original_syscall_table = NULL;
static long orig_cr0 = 0;

unsigned long *obtain_syscall_table_bf(void)
{
    /* get sys_call_table by kernel symbols */
    unsigned long i = kallsyms_lookup_name("sys_call_table");
    if (i == 0)
    {
        return NULL;
    }
    return (unsigned long *)i;
}

asmlinkage int hooked_sys_clone(unsigned long x1,
                                unsigned long x2,
                                int __user *x3,
                                int __user *x4,
                                unsigned long x5)
{
    int ret_val = original_sys_clone(x1, x2, x3, x4, x5);
    printk(KERN_INFO "hacked by %d (%s), ret val = %d\n", current->pid, current->comm, ret_val);
    return ret_val;
}

static int __init patch_syscall_init(void)
{

    original_syscall_table = obtain_syscall_table_bf();
    printk(KERN_INFO "syscall table: %px\n", original_syscall_table);
    unprotect_memory();
    original_sys_clone = (sys_clone_sig *)original_syscall_table[__NR_clone];
    original_syscall_table[__NR_clone] = (unsigned long)hooked_sys_clone;
    printk(KERN_INFO "replace %lx with %lx\n",
           (unsigned long)original_sys_clone,
           (unsigned long)hooked_sys_clone);
    protect_memory();
    return 0;
}
static void __exit patch_syscall_exit(void)
{
    unprotect_memory();
    original_syscall_table[__NR_clone] = (unsigned long)original_sys_clone;
    protect_memory();
}

module_init(patch_syscall_init);
module_exit(patch_syscall_exit);
