#include "headerFuncs.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include "mpi.h"

int main(int argc, char* argv[]) {
  if (argc < 2){
    printf("NEED 3 args: number of lines\n");
    exit(0);
  }
  
  int num = atoi(argv[1]);
  int i, j, k, l, colIndex =0;
  
  //int num_ranks, my_rank;
  
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  //READ
  char fname[71];//filePathLength + 18];
  //char fid[18];
  sprintf(fname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", my_rank+1);
  create_array_datatype();

  readFromFile(fname,num, array);
  //if (my_rank == 0){
  //  for (i=0;i<num;i++)
  //    printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
  //}

  //sort
  if (colIndex == 0)
    qsort(array, num, sizeof(struct data_struct), compare_x);
  else if (colIndex == 1)
    qsort(array, num, sizeof(struct data_struct), compare_y);
  else if (colIndex == 2)
    qsort(array, num, sizeof(struct data_struct), compare_z);
  else{
    printf("colIndex is between 0 and 2\n");
    exit(0);
  }
  
  //if (my_rank == 0){
  //  for (i=0;i<num;i++)
  //    printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
  //}

  // BALANCE
  long int* allCounts = (long int *) malloc(num_ranks*num_ranks*sizeof(long int));
  getallCount(num, colIndex, array, allCounts); 
  //if (my_rank == 0){
  //  for (i=0;i<num_ranks*num_ranks;i++)
  //    printf("%u: %Lu\n", i, allCounts[i]);
  //}

  
  // Send all Values
  struct data_struct* recv_array;  // To be allocated after getting count
  AllToAllSend(array, recv_array, allCounts);
  
  //if (colIndex == 0)
  //  qsort(recv_array, num, sizeof(struct data_struct), compare_x);
  //else if (colIndex == 1)
  //  qsort(recv_array, num, sizeof(struct data_struct), compare_y);
  //else if (colIndex == 2)
  //  qsort(recv_array, num, sizeof(struct data_struct), compare_z);
  //else{
  //  printf("colIndex is between 0 and 2\n");
  //  exit(0);
  //}

  //for (i=0;i<num;i++)
  //  printf("Node: %u\n%Lu\t%Lf\t%Lf\tLf\n", my_rank, recv_array.num, recv_array.xyz[0], recv_array.xyz[1], recv_array.xyz[2]);


  MPI_Finalize();
  return 0;
}
