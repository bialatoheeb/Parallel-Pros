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
      printf("L[%u]: %0.14Lf\n", j,L[j]);
      j++;
      
    }
    //if (L[num_ranks] < array[i].x)
    //  L[num_ranks] = array[i].x;
    
  }
  printf("L[%u]: %0.14Lf\n", num_ranks,L[j]);
  printf("L[%u]: %0.14Lf\n", num_ranks+1,L[j+1]);
      
  printf("  num: %u\n", num);
  printf("    D: %u\n", D);
  
  printf("Lower Bounds and Max\n=======================\n\n");
  for (i=0;i<num_ranks+1;i++)
    printf("%0.15Lf\t", L[i]);
  printf("\n");

}
