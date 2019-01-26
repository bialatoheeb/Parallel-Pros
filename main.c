#include "headerFuncs.h"
//#include "mpi.h"

int main(int argc, char* argv[]){ 
  
  int num;
  int i, j, total=0, K=0, colIndex =0;
  int num_ranks=2;
  int my_rank;
  int* send_counts;
  int * send_displs;
  int* recv_displs;
  int* recv_counts;
  int* rank_counts;
  int total_recv_counts;
  int rank_to_print = 2;  // Change to 0, 1 or 2
  
  
  if (argc != 3){
    printf("Needs two argument and to be run with 2 cores.\nUsage <mpirun -n 2 test \"num\" infile>\nExiting now\n");
    exit(0);
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
     
  MPI_Status status;
  MPI_Datatype array_type, data_type[2];
  int data_length[2];
  MPI_Aint displ[2], lower_bound, extent;
 
  // Number of elements to work on
  // getArraySize(const char* fname, int* size);
  // for testing
  num = atoi(argv[1]); //number of elements on this node
  total = num; //number of elements on all nodes
  
  // declare array of struct
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));

  // read from file
  readFromFile(argv[2], num, array);
  //printFile(num, array);

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
  
  // vars for lower bound
  long double* nodeL = (long double *) malloc((num_ranks+2)*sizeof(long double));
  getNodeL(num_ranks, num, colIndex, nodeL, array);
  printNodeL(num_ranks, nodeL);
  
  //ALL TO ALL TO GET L
  long double* Linfo = (long double *) malloc(((num_ranks+2)*num_ranks)*sizeof(long double));
  long double* L = (long double *) malloc(((num_ranks)*num_ranks)*sizeof(long double));
  
  getL(num_ranks, my_rank, nodeL, Linfo, L);
  //MPI_Allgather(nodeL, num_ranks+2,MPI_LONG_DOUBLE, Linfo,num_ranks+2,MPI_LONG_DOUBLE, MPI_COMM_WORLD); 
  //
  //if (my_rank == 0){
  //  for (i=0; i< ((num_ranks+2)*num_ranks); i++)
  //    printf("L[%u]: %0.14Lf\n", i,Linfo[i]);
  //  
  //  
  //
  //}
  //int k = 0;
  //for (i=0; ((num_ranks+2)*num_ranks); i++){
  //  if (i%(num_ranks+2) == 0){
  //    for (j=i;j<i+num_ranks; j++){
  //	if (my_rank == 0)
  //	  printf("L[%u]: %0.14Lf\n", i,Linfo[j]);
  //	L[k] = Linfo[j];
  //	k++;
  //	if (k >= num_ranks*num_ranks)
  //	  break;
  //    }
  //  }
  //  if (k >= num_ranks*num_ranks)
  //    break;
  //}
  //printf("\n===========\n\n");
  //if (my_rank == 0){
  //  for (i=0; i< (num_ranks*num_ranks); i++)
  //    printf("L[%u]: %0.14Lf\n", i,L[i]);
  //}
  //MPI_Type_free(&array_type);
  free(array);
  MPI_Finalize();
}
