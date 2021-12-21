#include "matrix.h"
#include <unistd.h>
#include <ctype.h>

size_t sizeMatrix(int r,int c)
{
    size_t matsz=r*c*4+sizeof(struct Matrix);
    return matsz;
}

Matrix* makeMatrix(int r,int c)
{
    Matrix* mat= (Matrix*)malloc(sizeMatrix(r, c));
    mat->r=r;
    mat->c=c;
    return mat;
}

Matrix* makeMatrixMap(void* zone,int r,int c)
{
Matrix*m=zone;
m->r=r;
m->c=c;
return m;
  
}

int readValue(FILE* fd)
{
   int v = 0;
   char ch;
   int neg=1;
   while (((ch = getc_unlocked(fd)) != EOF) && isspace(ch)); // skip WS.    
   while (!isspace(ch)) {
      if (ch=='-')
         neg=-1;
      else
         v = v * 10 + ch - '0';
      ch = getc_unlocked(fd);
   }
   return neg * v;
}

Matrix* readMatrix(FILE* fd)
{
   int r,c,v;
   int nv = fscanf(fd,"%d %d",&r,&c);
   Matrix* m = makeMatrix(r,c);
   flockfile(fd);
   for(int i=0;i < r;i++)
      for(int j=0;j < c;j++) {
         v = readValue(fd);
         M(m,i,j) = v; 
      }
   funlockfile(fd);
   return m;
}


void freeMatrix(Matrix* m)
{

    free(m);
}
void printMatrix(Matrix* m)
{
    int numRows=m->r;
    int numCol=m->c;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numCol;j++){
            int val=M(m, i, j);
            printf("  %d", val);
        }
        printf("\n");
    }
    

    
    
}

Matrix* multMatrix(Matrix* a,Matrix* b,Matrix* into)
{   
    int aRows=a->r;
    int aCol=a->c;
    int bRows=a->r;
    int bCol=b->c;
    int resRows=into->r;
    int resCol=into->c;
    for (int i=0; i<aRows; i++){
        for (int k=0; k<bCol; k++){
            int sum=0;
            for (int m=0; m<bRows; m++){
                int first=M(a, i, m);
                int second=M(b,m, k);
                int val=first*second;
                sum+=val;
            }
            M(into, i, k)=sum;
       }
    }
    
    return into;
}

Matrix* parMultMatrix(int nbW,sem_t* sem,Matrix* a,Matrix* b,Matrix* into)
{ 
    int rowsEach=a->r/nbW;
    int wkList[nbW];
    int startRow=0;
    int rem=a->r%nbW;
for (int n=0; n<nbW; n++){
    wkList[n]=rowsEach;
    if (rem>0)
    {
        wkList[n]++;
    }
    rem--;

}
    
for (int i=0; i<nbW; i++){
    pid_t child=fork();
        if (child==0){
            for(int j=startRow; j<(startRow + wkList[i]); j++){
                for (int k=0; k < b->c; k++){
                    int sum=0;
                    for (int h=0; h< a->c; h++){
                        int first=M(a,j , h);
                        int second=M(b, h, k);
                        sum+=(first*second);
                    }
                M(into, j, k)=sum;
            }
            }
         sem_post(sem);
         exit(0);   
        }
        startRow+= wkList[i];
    }
    for (int m=0; m<nbW; m++){
        sem_wait(sem);
    }
    return into;
}
