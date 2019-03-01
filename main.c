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
  
  //array = globalSort(array, &num, colIndex);
  buildTreeGlobal(array, num, &headNode, -1);
  //buildTree(array, num, &headNode, -1);
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
  
  
  MPI_Finalize();
  return 0;
}
