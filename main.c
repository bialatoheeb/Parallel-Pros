#include "headerFuncs.h"

int main(int argc, char* argv[]) {
  if (argc < 2){
    printf("NEED 1 args: number of data points\n");
    exit(0);
  }
  timePrint = 0;
  int datapoints = atoi(argv[1]);
  int num;
  int i, j, k, l, colIndex = 0;
  double startTime[6], endTime[6], avgTime[6];
  struct node headNode;
  
  startTime[0] = timestamp();
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  j = MPI_Comm_dup(MPI_COMM_WORLD, &MPI_LOCAL_COMM);
  //if (my_rank == 0)
  //  printf("C long double %u\n MPI long long  double %u\n C double %u\n MPI double %u\n", sizeof(long double), MPI_LONG_DOUBLE, sizeof(double), sizeof(MPI_DOUBLE));
  //exit(0);
  
  //if (my_rank == 0)
  //  printf("\n\n====================\n\n%u, %u\n\n", datapoints, num_ranks);
  
  if (my_rank == num_ranks-1){
    num = (int)datapoints/num_ranks + datapoints%num_ranks;
  }else{

    num = (int)datapoints/num_ranks;
  }
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  //READ
  create_array_datatype();
  //readFromFile(fname, num, array);
  startTime[1] = timestamp();
  readFromFileAllRead(datapoints, array );
  endTime[1] = timestamp() - startTime[1];
  MPI_Reduce(&endTime[1], &startTime[1], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  avgTime[1] = startTime[1]/num_ranks;
  //printFile(10, array);
  buildTreeGlobal(array, num, &headNode, -1);
  
  MPI_Finalize();
  return 0;
  
  //readFile1(datapoints, array );
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
  startTime[2] = timestamp();
  do_sort(array, num, colIndex);
  endTime[2] = timestamp() - startTime[2];
  MPI_Reduce(&endTime[2], &startTime[2], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  avgTime[2] = startTime[2]/num_ranks;
  if (my_rank == 0 && timePrint == 1)
    printf("%u;%u;%f;%f;", datapoints,num_ranks,avgTime[1],avgTime[2]);
  
  // BALANCE
  int* allCounts = (int *) malloc(num_ranks*num_ranks*sizeof(int));
  startTime[3] = timestamp();
  getallCount(num, colIndex, array, allCounts); 
  endTime[3] = timestamp() - startTime[3];
  MPI_Reduce(&endTime[3], &startTime[3], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  avgTime[3] = startTime[3]/num_ranks;
  //printf("rank: %u; num: %u\n", my_rank,num);
  
  printCount(allCounts);
  int total_recv_counts;  
  // All to all 
  startTime[4] = timestamp();
  struct data_struct *recv_array  = AllToAllSend(array, &total_recv_counts, allCounts);
  endTime[4] = timestamp() - startTime[4];
  MPI_Reduce(&endTime[4], &startTime[4], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  avgTime[4] = startTime[4]/num_ranks;
  // MPI_Isend and recv
  //startTime =timestamp();
  //struct data_struct *recv_array  = AllToAllIsend(array, &total_recv_counts, allCounts);
  //endTime = timestamp() - startTime;
  //MPI_Reduce(&endTime, &startTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  //if (my_rank == 0)
  //  printf("Time: %f\n", startTime);
  //sort again
  startTime[5] = timestamp();
  do_sort(recv_array, total_recv_counts, colIndex);
  endTime[5] = timestamp() - startTime[5];
  MPI_Reduce(&endTime[5], &startTime[5], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
  verify(recv_array[0].xyz[colIndex], recv_array[total_recv_counts-1].xyz[colIndex]);

  avgTime[5] = startTime[5]/num_ranks;
  //Print the first middle and last on each node
  int temp = (int)total_recv_counts/2;
  for(i=0; i < num_ranks; i++){
    MPI_Barrier(MPI_COMM_WORLD);
    if (i == my_rank){
      printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[0].num, recv_array[0].xyz[0], recv_array[0].xyz[1], recv_array[0].xyz[2]);
      printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[temp].num, recv_array[temp].xyz[0], recv_array[temp].xyz[1], recv_array[temp].xyz[2]);
      printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[total_recv_counts-1].num, recv_array[total_recv_counts-1].xyz[0], recv_array[total_recv_counts-1].xyz[1], recv_array[total_recv_counts-1].xyz[2]);
    }
  }
////      for (i=0; i < total_recv_counts; i++)
////      printf("%8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", recv_array[i].num, recv_array[i].xyz[0], recv_array[i].xyz[1], recv_array[i].xyz[2]);
  //  }
  //}

  endTime[0] = timestamp() - startTime[0];
  MPI_Reduce(&endTime[0], &startTime[0], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  avgTime[0] = startTime[0]/num_ranks;
  //for (i=0;i++;i<6)
  //if (my_rank == 0)
  //  printf("%u;%u;%f;%f;%f;%f;%f;%f\n", datapoints,num_ranks,startTime[1],startTime[2],startTime[3],startTime[4],startTime[5],startTime[0]);
  if (my_rank == 0  && timePrint == 1)
    printf("%f;%f;%f;%f\n",avgTime[3], avgTime[4],avgTime[5],avgTime[0]);
  
  MPI_Finalize();
  return 0;
}
