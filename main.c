#include "headerFuncs.h"

int main(int argc, char* argv[]) {
  if (argc < 2){
    printf("NEED 3 args: number of lines\n");
    exit(0);
  }
  
  int num = atoi(argv[1]);
  int i, j, k, l, colIndex = 0;
  
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  //READ
  char fname[71];
  sprintf(fname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", my_rank+1);
  create_array_datatype();

  readFromFile(fname,num, array);

  //sort
  do_sort(array, num, colIndex);
 

  // BALANCE
  int* allCounts = (int *) malloc(num_ranks*num_ranks*sizeof(int));
  int* totalCount = (int *) malloc(num_ranks*sizeof(int));
  getallCount(num, colIndex, array, allCounts); 
  if (my_rank == 0){
    printf("%5s|", " ");
    for (i=0; i<num_ranks; i++){
      printf("%9u|", i);
      totalCount[i] = 0;
    }
    printf("\n");
    k = 0;
    for (i=0; i<num_ranks; i++){
      printf("%5u|",i);
      for (j=0; j<num_ranks; j++){
	
	printf("%9u|",allCounts[k]);
	totalCount[j] += allCounts[k];
	k++;
      }
      printf("\n");
      
    }
    printf("%5s|", "total");
    for (i=0; i<num_ranks; i++){
      printf("%9u|", totalCount[i]);
    }
    printf("\n================\n\n");
  }
  int total_recv_counts;  
  // All to all 
  struct data_struct *recv_array  = AllToAllSend(array, &total_recv_counts, allCounts);

  // MPI_Isend and recv
  //struct data_struct *recv_array  = AllToAllIsend(array, &total_recv_counts, allCounts);

  //sort again
  do_sort(recv_array, total_recv_counts, colIndex);
  
  //Print the first middle and last on each node
  int temp = (int)total_recv_counts/2;
  for(i=0; i < num_ranks; i++){
    MPI_Barrier(MPI_COMM_WORLD);
    if (i == my_rank){
      printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[0].num, recv_array[0].xyz[0], recv_array[0].xyz[1], recv_array[0].xyz[2]);
      printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[temp].num, recv_array[temp].xyz[0], recv_array[temp].xyz[1], recv_array[temp].xyz[2]);
      printf("Rank %3d: %8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", my_rank, recv_array[total_recv_counts-1].num, recv_array[total_recv_counts-1].xyz[0], recv_array[total_recv_counts-1].xyz[1], recv_array[total_recv_counts-1].xyz[2]);
  
//      for (i=0; i < total_recv_counts; i++)
//      printf("%8Lu\t%0.17Lf\t%0.17Lf\t%0.17Lf\n", recv_array[i].num, recv_array[i].xyz[0], recv_array[i].xyz[1], recv_array[i].xyz[2]);
    }
  }
  
  MPI_Finalize();
  return 0;
}
