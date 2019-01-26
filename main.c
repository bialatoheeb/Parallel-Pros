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
  total = num; //number of elements on all nodes
  
  // declare array of struct
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  struct data_struct* recv_array;  // To be allocated after getting count

  // read from file
  readFromFile(argv[2], num, array);

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
  //printFile(num, array);
  printf("myrank = %d of %d\n", my_rank, num_ranks);
   
  // vars for lower bound
  long double* nodeL = (long double *) malloc((num_ranks+2)*sizeof(long double));
  
  getNodeL(num, colIndex, nodeL, array);
  printNodeL(nodeL);
 
  //ALL TO ALL TO GET L
  long double* Linfo = (long double *) malloc(((num_ranks+2)*num_ranks)*sizeof(long double));
  long double* L = (long double *) malloc(((num_ranks)*num_ranks)*sizeof(long double));
  int * total_counts = (int *) malloc(num_ranks * num_ranks*sizeof(int));

  getL(nodeL, Linfo, L);

  // Change to the function Getcount to send/recv to/from all nodes
  // Form here
  for (i=0;i < (num_ranks*num_ranks); i++)
    total_counts[i] = 50;
  // To here
  AllToAllSend(array, recv_array, total_counts);

  MPI_Type_free(&array_type);
  free(array);
  MPI_Finalize();
}
