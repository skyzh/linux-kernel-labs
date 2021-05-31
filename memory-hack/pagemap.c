#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PTE_SIZE 8
#define GET_BIT(X, Y) ((X & ((unsigned long)1 << Y)) >> Y)
#define GET_PFN(X) (X & 0x7FFFFFFFFFFFFF)

int main(int argc, char **argv)
{
   unsigned long vaddr;
   int pid = atoi(argv[1]);
   char pagemap_path[1024];

   sscanf(argv[2], "%lx", &vaddr);
   printf("Request %d @ %lx\n", pid, vaddr);
   sprintf(pagemap_path, "/proc/%d/pagemap", pid);

   FILE *fd = fopen(pagemap_path, "rb");
   unsigned long file_offset = vaddr / getpagesize() * PTE_SIZE;
   fseek(fd, file_offset, SEEK_SET);

   unsigned long pte[1] = {0};
   fread(&pte, PTE_SIZE, 1, fd);
   printf("PTE: %lx\n", pte[0]);

   if (GET_BIT(pte[0], 63))
   {
      printf("PFN: %lx\n", GET_PFN(pte[0]));
      printf("Addr: %lx\n", GET_PFN(pte[0]) * getpagesize());
   }
   return 0;
}
