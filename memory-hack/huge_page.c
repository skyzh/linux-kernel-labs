#include <sys/mman.h>
#include <stdio.h>
#include <memory.h>

void listvma_and_findpage(void *addr)
{
    system("dmesg -C");
    FILE *fp = fopen("/proc/mtest", "w");
    fprintf(fp, "listvma\n");
    fclose(fp);
    system("dmesg");

    char cmd[1024] = "";
    sprintf(cmd, "cat /proc/%d/maps", getpid());
    system(cmd);

    sprintf(cmd, "findpage %p", addr);
    system("dmesg -C");

    fp = fopen("/proc/mtest", "w");
    fprintf(fp, "%s\n", cmd);
    fclose(fp);
    system("dmesg");
}

int main(int argc, char *argv[])
{
    char *mem;
    size_t size = (2UL * 1024 * 1024); // 2MB huge page

    mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (mem == MAP_FAILED)
    {
        return 1;
    }

    printf("huge page allocated at %p with size %lx\n", mem, size);

    memset(mem, 0, size);

    listvma_and_findpage(mem); // echo "listvma" to "/proc/mtest" in current process

    munmap(mem, size);
    return 0;
}
