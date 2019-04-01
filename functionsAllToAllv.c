#include "headerFuncs.h"

struct data_struct *  AllToAllSend(void * sends, int  *total_recv_counts, void * rankc){

  int* send_counts;
  int* recv_counts;
  int* send_displs;
  int* recv_displs;
  int i;
  struct data_struct*  send_array  = (struct data_struct *) sends;
  int * rank_counts = (int *) rankc;

  send_counts = (int *) calloc(num_ranks, sizeof(int));
  recv_counts = (int *) calloc(num_ranks, sizeof(int));
  recv_displs = (int *) calloc(num_ranks, sizeof(int));
  send_displs = (int *) calloc(num_ranks, sizeof(int));
  
  send_counts[0] = rank_counts[num_ranks*my_rank];
  *total_recv_counts =  recv_counts[0] = rank_counts[my_rank];
  for (i=1; i < num_ranks; i++){
    send_counts[i] = rank_counts[num_ranks*my_rank + i];
    send_displs[i] = send_displs[i-1] + send_counts[i-1];
    recv_counts[i] = rank_counts[i*num_ranks + my_rank];
    recv_displs[i] = recv_displs[i-1] + recv_counts[i-1];
    (*total_recv_counts) += recv_counts[i];
  }

  struct data_struct *recv_array = (struct data_struct *) malloc((*total_recv_counts) * sizeof(struct data_struct));
  MPI_Alltoallv(send_array, send_counts, send_displs, array_type, recv_array, recv_counts, recv_displs, array_type, myCommCollection->localcomm);

  free(send_array);
  return recv_array;
}
