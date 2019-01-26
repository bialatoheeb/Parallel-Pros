#include "headerFuncs.h"

void AllToAllSend(void * send, void * recv, void * rankc, MPI_Datatype array_type){


  int* send_counts;
  int * send_displs;
  int* recv_displs;
  int* recv_counts;
  int total_recv_counts;
  int i;
  struct data_struct*  send_array  = (struct data_struct *) send;
  struct data_struct*  recv_array  = (struct data_struct *) recv;
  int * rank_counts = (int *) rank_counts;



  send_counts = (int *) calloc(num_ranks, sizeof(int));
  recv_counts = (int *) calloc(num_ranks, sizeof(int));
  recv_displs = (int *) calloc(num_ranks, sizeof(int));
  send_displs = (int *) calloc(num_ranks, sizeof(int));
  

  send_counts[0] = rank_counts[num_ranks*my_rank];
  total_recv_counts =  recv_counts[0] = rank_counts[my_rank];
  for (i=1; i < num_ranks; i++){
    send_counts[i] = rank_counts[num_ranks*my_rank + i];
    send_displs[i] = send_displs[i-1] + send_counts[i-1];
    recv_counts[i] = rank_counts[i*num_ranks + my_rank];
    recv_displs[i] = recv_displs[i-1] + recv_counts[i-1];
      total_recv_counts += recv_counts[i];
  }
  recv_array = malloc(total_recv_counts * sizeof(struct data_struct));

  MPI_Alltoallv(send_array, send_counts, send_displs, array_type, recv_array, recv_counts, recv_displs, array_type, MPI_COMM_WORLD);

  

}
