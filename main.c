#include "headerFuncs.h"

void emptyRankFiles(){
  char fname[20];
  sprintf(fname,"/home/gst2d/COMS7900/nodes%03u.txt", my_global_rank);
  FILE *myfile = fopen(fname, "w");
  fclose(myfile);
}

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
  struct node headNode, *localHead;
  
  startTime[0] = timestamp();
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  my_global_rank = my_rank;
  //emptyRankFiles();
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
  readFromFileAllRead(datapoints, array );
  // printFile(10, array);
  //if (my_global_rank == 0){
  //  printFile(10, array);
  //  printf("before global Tree (main)\n");
  //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", num-1, array[num-1].xyz[0], array[num-1].xyz[1], array[num-1].xyz[2]);
  //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", 0, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
  //}
  
  //array = globalSort(array, &num, colIndex);
  localHead = buildTreeGlobal(array, num, &headNode, -1);
  printf("AFTER GLOBAL\n");
  if (my_global_rank == 0)
    buildTree(array, localHead->num_below, localHead, -1);
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
