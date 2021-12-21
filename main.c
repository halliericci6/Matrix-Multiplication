#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>      
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include "matrix.h"

Matrix* loadMatrix(char* fName)
{
   FILE* fd = fopen(fName,"r");
   if (fd==NULL) return NULL;
   Matrix* m = readMatrix(fd);
   fclose(fd);
   return m;
}

int main(int argc,char* argv[])
{
   if (argc < 4) {
      printf("usage: mult <file1> <file2> <#workers>\n");
      exit(1);
   }
   Matrix* a = loadMatrix(argv[1]);
   Matrix* b = loadMatrix(argv[2]);
   int nbW = atoi(argv[3]);
   if (a==NULL || b==NULL) {
      printf("Wrong filenames. Couldn't load matrices [%s,%s]\n",argv[1],argv[2]);
      exit(2);
   }
Matrix* newm= makeMatrix(a->r, b->c);
Matrix* res=multMatrix(a, b, newm);
int r=a->r;
int c=b->c;
const char* zone="/memzone11";
int mapsz=sizeMatrix(a->r, b->c);
int pg=(mapsz/4096)+1;
int md=shm_open(zone, O_RDWR|O_CREAT, S_IRWXU);
int x=ftruncate(md, pg*4096);
void *ptr=mmap(NULL, pg*4096, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FILE, md, 0);
sem_t* s=sem_open("/sem1", O_CREAT, 0666, 0);
memset(ptr, 0, mapsz);
Matrix* into=makeMatrixMap(ptr, a->r, b->c);
parMultMatrix(nbW,s, a, b, into);
printMatrix(into);
freeMatrix(into);
munmap(into, 4096);
sem_close(s);
return 0;
}
