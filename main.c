#include "headerFuncs.h"
//#include "mpi.h"

int main(int argc, char* argv[]){ 
  
  int num;
  int i, j, total=0, K=0, colIndex =0;
  int num_ranks=2;
  int my_rank;
  
  
  if (argc != 3){
    printf("Needs two argument and to be run with 2 cores.\nUsage <mpirun -n 2 test \"num\" infile>\nExiting now\n");
    exit(0);
  }

  // Number of elements to work on
  // getArraySize(const char* fname, int* size);
  // for testing
  num = atoi(argv[1]); //number of elements on this node
  total = num; //number of elements on all nodes
  
  // declare array of struct
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));

  // read from file
  readFromFile(argv[2], num, array);
  printFile(num, array);

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
  printFile(num, array);
  
  // vars for lower bound
  long double* nodeL = (long double *) malloc((num_ranks+2)*sizeof(long double));
  getNodeL(num_ranks, num, colIndex, nodeL, array);
  
}
