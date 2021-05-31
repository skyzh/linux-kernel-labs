#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/uaccess.h>
#include <linux/highmem.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex Chi");
MODULE_DESCRIPTION("A module which supports raw memory operations");
MODULE_VERSION("0.01");

static struct proc_dir_entry *entry;

static void m_list_vma(void)
{
    struct vm_area_struct *vma = NULL;
    struct task_struct *task = current;
    struct mm_struct *mm = get_task_mm(task);
    char permission_flags[8] = {0};

    if (mm)
    {
        down_read(&mm->mmap_sem);
        for (vma = mm->mmap; vma != NULL; vma = vma->vm_next)
        {
            permission_flags[0] = vma->vm_flags & VM_READ ? 'r' : '-';
            permission_flags[1] = vma->vm_flags & VM_WRITE ? 'w' : '-';
            permission_flags[2] = vma->vm_flags & VM_EXEC ? 'x' : '-';
            permission_flags[3] = vma->vm_flags & VM_SHARED ? 's' : '-';
            printk(KERN_INFO "%lx %lx %s\n", vma->vm_start, vma->vm_end, permission_flags);
        }
        up_read(&mm->mmap_sem);
    }
}

static void m_find_vma(unsigned long long addr)
{
    /* A generic way to find vma.
       We use Linux interface to map a user page to kernel,
       then get the physical address translation. */
    struct task_struct *task = current;
    struct mm_struct *mm = get_task_mm(task);
    struct page *pages[1];

    if (mm)
    {
        down_read(&mm->mmap_sem);
        /* pin exactly one page */
        if (get_user_pages(addr, 1, 0, pages) < 1)
        {
            printk(KERN_WARNING "translation not found\n");
            up_read(&mm->mmap_sem);
            return;
        }
        printk(KERN_INFO "%llx -> %llx\n", addr, virt_to_phys(page_address(pages[0])));
        put_page(pages[0]);
        up_read(&mm->mmap_sem);
    }
}

static void m_write_val(unsigned long long *addr, unsigned long long val)
{
    unsigned long long written_val;
    unsigned long long original_val;
    if (get_user(original_val, addr) < 0)
    {
        goto fail;
    }
    if (put_user(val, addr) < 0)
    {
        goto fail;
    }
    if (get_user(written_val, addr) < 0)
    {
        goto fail;
    }
    printk(KERN_INFO "write %llx to %px\n", written_val, addr);
    if (put_user(original_val, addr) < 0)
    {
        goto fail;
    }
    printk(KERN_INFO "recover %llx to %px\n", original_val, addr);
    return;
fail:
    printk(KERN_WARNING "failed to write value\n");
}

static ssize_t entry_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    return 0;
}

static ssize_t entry_write(struct file *file, const char *ubuf, size_t count, loff_t *ppos)
{
    char user_data[128] = {0};
    char command[128] = {0};
    unsigned long long arg1 = 0;
    unsigned long long arg2 = 0;

    if (count >= 128)
    {
        return -EFAULT;
    }
    if (copy_from_user(user_data, ubuf, count))
    {
        return -EFAULT;
    }
    user_data[count] = '\0';
    sscanf(user_data, "%s %llx %llx", command, &arg1, &arg2);

    if (strcmp(command, "listvma") == 0)
    {
        printk(KERN_INFO "Recv request: listvma\n");
        m_list_vma();
    }
    else if (strcmp(command, "findpage") == 0)
    {
        printk(KERN_INFO "Recv request: findpage %px\n", (void *)arg1);
        m_find_vma(arg1);
    }
    else if (strcmp(command, "writeval") == 0)
    {
        printk(KERN_INFO "Recv request: writeval %px %llx\n", (void *)arg1, arg2);
        m_write_val((unsigned long long *)arg1, arg2);
    }
    else
    {
        printk(KERN_INFO "Unsupported command: %s", user_data);
    }

    return count;
}

static struct file_operations entry_ops =
    {
        .read = entry_read,
        .write = entry_write};

static int __init mem_module_init(void)
{
    entry = proc_create("mtest", 0644, NULL, &entry_ops);
    if (!entry)
    {
        return -EFAULT;
    }
    return 0;
}

static void __exit mem_module_exit(void)
{
    proc_remove(entry);
}

module_init(mem_module_init);
module_exit(mem_module_exit);
