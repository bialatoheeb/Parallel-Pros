#include "headerFuncs.h"

void getL(int num_ranks, int my_rank, void * vnodeL, void * vLinfo, void * vL){
  long double  *L = (long double *) vL;
  long double  *Linfo = (long double *) vLinfo;
  long double  *nodeL = (long double *) vnodeL;
  
  int i,j,k=0;
  MPI_Allgather(nodeL, num_ranks+2,MPI_LONG_DOUBLE, Linfo,num_ranks+2,MPI_LONG_DOUBLE, MPI_COMM_WORLD); 
  
  if (my_rank == 0){
    for (i=0; i< ((num_ranks+2)*num_ranks); i++)
      printf("L[%u]: %0.14Lf\n", i,Linfo[i]);
    
    

  }
  for (i=0; ((num_ranks+2)*num_ranks); i++){
    if (i%(num_ranks+2) == 0){
      for (j=i;j<i+num_ranks; j++){
  	if (my_rank == 0)
  	  printf("L[%u]: %0.14Lf\n", i,Linfo[j]);
  	L[k] = Linfo[j];
  	k++;
  	if (k >= num_ranks*num_ranks)
  	  break;
      }
    }
    if (k >= num_ranks*num_ranks)
      break;
  }
  printf("\n===========\n\n");
  if (my_rank == 0){
    for (i=0; i< (num_ranks*num_ranks); i++)
      printf("L[%u]: %0.14Lf\n", i,L[i]);
  }
}
