#include "headerFuncs.h"

int main(int argc, char* argv[]){ 
  
  int num;
  int i, j, total=0, K=0, colIndex =0;
  
  
  if (argc != 3){
    printf("Needs two argument and to be run with 2 cores.\nUsage <mpirun -n 2 test \"num\" infile>\nExiting now\n");
    exit(0);
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
     
  create_array_datatype();

  num = atoi(argv[1]); //number of elements on this node
  
  // declare array of struct
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  struct data_struct* recv_array;  // To be allocated after getting count

  // read from file
  readFromFile(my_rank, num, array);

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
  long int* allCounts = (long int *) malloc(num_ranks*num_ranks*sizeof(long int));
  getallCount(num, colIndex, array, allCounts); 
  AllToAllSend(array, recv_array, allCounts);
  
  //MPI_Type_free(&array_type);
  free(array);
  MPI_Finalize();
}
