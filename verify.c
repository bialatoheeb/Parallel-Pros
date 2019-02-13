#include "headerFuncs.h"

void verify(long double min, long double max){
  int i, bool;
  long double min_max[2] = {min, max};
  long double * temp = (long double *) malloc(2 * num_ranks * sizeof(long double));
  MPI_Allgather(min_max, 2, MPI_LONG_DOUBLE, temp, 2, MPI_LONG_DOUBLE, MPI_COMM_WORLD);
 
  bool = 1;
  if (my_rank == 0){
    for (i = 1; i < num_ranks-1; i+=2){
      if (temp[i] >= temp[i+1]){
	bool = 0;
	break;
      }
    }
  }
  if (bool == 0){
    if (my_rank == 0)
      printf("Last element on node %d is >= first element on node %d\n", i/2, i/2 + 1);
    MPI_Finalize();
    exit(0);
	
  }
 
}
