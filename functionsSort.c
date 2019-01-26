#include "headerFuncs.h"

void getNodeL(int num, int colIndex, void* vL, void* varray){
  struct data_struct  *array = (struct data_struct *) varray;
  long double  *L = (long double *) vL;
  int i, j, D;
  j = 0;
  printf("num_ranks = %d\n", num_ranks);
  D = (int)(num/num_ranks);
  j = 0;
  L[num_ranks+1] = num;
  L[num_ranks] = array[num-1].xyz[colIndex]; //array[0].x;
  for(i=0; i<num; i++){
    if (i%D == 0 && j<num_ranks){
      L[j] = array[i].xyz[colIndex];
      j++;
    }
    //if (L[num_ranks] < array[i].x)
    //  L[num_ranks] = array[i].x;
    
  }
  
}



void printNodeL(void* vL){
  long double  *L = (long double *) vL;
  int i;
  for (i=0; i<num_ranks+2; i++)
    printf("L[%d]: %0.14Lu\n", i, L[i]);
  printf("\n");

}
