#include "headerFuncs.h"

void getNodeL(const int num_ranks, const int num, const int colIndex, void* vL, void* varray){
  struct data_struct  *array = (struct data_struct *) varray;
  long double  *L = (long double *) vL;
  int i, j=0, D;
  D = (int)(num/num_ranks);
  j = 0;
  L[num_ranks+1] = num;
  L[num_ranks] = array[num-1].xyz[colIndex]; //array[0].x;
  for(i=0;i<num;i++){
    if (i%D == 0 && j<num_ranks){
      L[j] = array[i].xyz[colIndex];
      j++;
      
    }
    //if (L[num_ranks] < array[i].x)
    //  L[num_ranks] = array[i].x;
    
  }
  
}

void printNodeL(const int num_ranks, void* vL){
  long double  *L = (long double *) vL;
  int i;
  for (i=0;i<num_ranks+2;i++)
    printf("L[%u]: %0.14Lf\n", i,L[i]);
  
  printf("\n");

}
