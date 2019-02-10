#include "headerFuncs.h"

int main(int argc, char* argv[]) {
  if (argc < 2){
    printf("NEED 1 args: number of data points\n");
    exit(0);
  }
  int datapoints = atoi(argv[1]);
  int num;
  int i, j, k, l, colIndex = 0;
  double startTime, endTime;
  startTime =timestamp();
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  //if (my_rank == 0)
  //  printf("C long double %u\n MPI long long  double %u\n C double %u\n MPI double %u\n", sizeof(long double), MPI_LONG_DOUBLE, sizeof(double), sizeof(MPI_DOUBLE));
  //exit(0);

  if (my_rank == num_ranks-1){
    num = (int)datapoints/num_ranks + datapoints%num_ranks;
  }else{

    num = (int)datapoints/num_ranks;
  }
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  //READ
  char fname[71];
  sprintf(fname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", my_rank+1);
  //char fname[6];
  //sprintf(fname,"/home/gst2d/COMS7900/sample/s%u.bin", my_rank);
  create_array_datatype();
  //readFromFile(fname, num, array);
  readFromFileAllRead(datapoints, array );
  endTime = timestamp() - startTime;
  MPI_Reduce(&endTime, &startTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (my_rank == 0)
    printf("Time: %f\n", startTime);
  //readFromFileOneToAll(num, array, datapoints);
  //printFile(10, array);
  //for (i=0;i<num_ranks;i++){
  //  MPI_Barrier(MPI_COMM_WORLD);
  //  if (i == my_rank){
  //    printf("rank: %u; id[0]: %u\n", my_rank, array[0].num);
  //    printf("rank: %u; id[num-1]: %u\n", my_rank, array[num-1].num);
  //    
  //  }
  //}
  //sort
  //do_sort(array, num, colIndex);
  //
  //
  //// BALANCE
  //int* allCounts = (int *) malloc(num_ranks*num_ranks*sizeof(int));
  //getallCount(num, colIndex, array, allCounts); 
  //printf("rank: %u; num: %u\n", my_rank,num);
  
  //printCount(allCounts);
  //int total_recv_counts;  
  // All to all 
  //struct data_struct *recv_array  = AllToAllSend(array, &total_recv_counts, allCounts);
  //
  //// MPI_Isend and recv
  ////struct data_struct *recv_array  = AllToAllIsend(array, &total_recv_counts, allCounts);
  //
  ////sort again
  //do_sort(recv_array, total_recv_counts, colIndex);
  //
  ////Print the first middle and last on each node
  //int temp = (int)total_recv_counts/2;
  //for(i=0; i < num_ranks; i++){
  //  MPI_Barrier(MPI_COMM_WORLD);
  //  if (i == my_rank){
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[0].num, recv_array[0].xyz[0], recv_array[0].xyz[1], recv_array[0].xyz[2]);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[temp].num, recv_array[temp].xyz[0], recv_array[temp].xyz[1], recv_array[temp].xyz[2]);
  //    printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[total_recv_counts-1].num, recv_array[total_recv_counts-1].xyz[0], recv_array[total_recv_counts-1].xyz[1], recv_array[total_recv_counts-1].xyz[2]);
  //
////      for (i=0; i < total_recv_counts; i++)
////      printf("%8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", recv_array[i].num, recv_array[i].xyz[0], recv_array[i].xyz[1], recv_array[i].xyz[2]);
  //  }
  //}
  
  MPI_Finalize();
  return 0;
}
