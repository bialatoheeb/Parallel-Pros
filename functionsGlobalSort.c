#include "headerFuncs.h"

struct data_struct * globalSort(void * varray, int *num, int colIndex){
  struct data_struct* array  = (struct data_struct *) varray;
  int i;
  do_sort(array, *num, colIndex);
  // BALANCE
  int* allCounts = (int *) malloc(num_ranks*num_ranks*sizeof(int));
  getallCount(*num, colIndex, array, allCounts); 
  //printCount(allCounts);
  int total_recv_counts;  
  struct data_struct *recv_array = AllToAllSend(array, &total_recv_counts, allCounts);
  *num =total_recv_counts;
  //array = recv_array;
  do_sort(recv_array, *num, colIndex);
  
  return recv_array;
  
  //====================================
  //
  //            TESTING
  //
  //==================================
  //if (my_rank == 0)
  //  printf("\n\n\n");

  //int temp = (int)*num/2;
  //for(i=0; i < num_ranks; i++){
  //  MPI_Barrier(MPI_COMM_WORLD);
  //  if (i == my_rank){
  //    printf("Rank %3d: %d\n", my_rank, *num);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, array[0].num, array[0].xyz[0], array[0].xyz[1], array[0].xyz[2]);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, array[temp].num, array[temp].xyz[0], array[temp].xyz[1], array[temp].xyz[2]);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, array[*num-1].num, array[*num-1].xyz[0], array[*num-1].xyz[1], array[*num-1].xyz[2]);
  //  }
  //}




  //verify(recv_array[0].xyz[colIndex], recv_array[total_recv_counts-1].xyz[colIndex]);
  //
  //avgTime[5] = startTime[5]/num_ranks;
  ////Print the first middle and last on each node
  //if (my_global_rank == 2){
  //int temp = (int)total_recv_counts/2;
  //for(i=0; i < num_ranks; i++){
  //  //MPI_Barrier(MPI_COMM_WORLD);
  //  if (i == my_rank){
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[0].num, recv_array[0].xyz[0], recv_array[0].xyz[1], recv_array[0].xyz[2]);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[temp].num, recv_array[temp].xyz[0], recv_array[temp].xyz[1], recv_array[temp].xyz[2]);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[total_recv_counts-1].num, recv_array[total_recv_counts-1].xyz[0], recv_array[total_recv_counts-1].xyz[1], recv_array[total_recv_counts-1].xyz[2]);
  //  }
  //  MPI_Barrier(MPI_COMM_WORLD);
  //}
  //}
  
  //}
//////      for (i=0; i < total_recv_counts; i++)
//////      printf("%8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", recv_array[i].num, recv_array[i].xyz[0], recv_array[i].xyz[1], recv_array[i].xyz[2]);
  ////  }
  ////}
  //
  //endTime[0] = timestamp() - startTime[0];
  //MPI_Reduce(&endTime[0], &startTime[0], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  //avgTime[0] = startTime[0]/num_ranks;
  ////for (i=0;i++;i<6)
  ////if (my_rank == 0)
  ////  printf("%u;%u;%f;%f;%f;%f;%f;%f\n", datapoints,num_ranks,startTime[1],startTime[2],startTime[3],startTime[4],startTime[5],startTime[0]);
  //if (my_rank == 0  && timePrint == 1)
  //  printf("%f;%f;%f;%f\n",avgTime[3], avgTime[4],avgTime[5],avgTime[0]);

}
