#include "headerFuncs.h"

struct data_struct * globalSort(void * varray, int *num, int colIndex, int *globalNum){
  struct data_struct* array  = (struct data_struct *) varray;
  int i;
  do_sort(array, *num, colIndex);
  
  // BALANCE
  int* allCounts = (int *) malloc(num_ranks*num_ranks*sizeof(int));
  getallCount(*num, colIndex, array, allCounts); 
  
  int total_recv_counts;  
  struct data_struct *recv_array = AllToAllSend(array, &total_recv_counts, allCounts);
  *num =total_recv_counts;
  do_sort(recv_array, *num, colIndex);
  *globalNum = 0;
  for (i=0; i<num_ranks*num_ranks;i++)
    *globalNum+=allCounts[i];
  return recv_array;
}
