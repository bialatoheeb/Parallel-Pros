#include "headerFuncs.h"

int main(int argc, char* argv[]) {
  int i, j, k, l, colIndex = 0, *size;
  char comm_name[MPI_MAX_OBJECT_NAME];
  double startTime[6], endTime[6], avgTime[6];
  //MPI_Comm MPI_LOCAL_COMM, MPI_TEMP_COMM;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  j = MPI_Comm_dup(MPI_COMM_WORLD, &MPI_LOCAL_COMM);

  //MPI_Comm_size(MPI_LOCAL_COMM, &num_ranks);
  ////printf("HELLO\n");
  //MPI_Comm_rank(MPI_LOCAL_COMM, &my_rank);
  //MPI_Comm_get_name(MPI_LOCAL_COMM, comm_name, size);
  //MPI_Barrier(MPI_LOCAL_COMM);
  //for (i=0;i<num_ranks;i++){
  //  MPI_Barrier(MPI_LOCAL_COMM);
  //  if (my_rank == i)
  //    printf("%s: %5u %3u\n", comm_name, num_ranks, my_rank);
  //}
  myFunc();
  
  MPI_Finalize();
  return 0;
}

void myFunc(){
  int i = 0, j, *size;
  
  MPI_Comm_size(MPI_LOCAL_COMM, &num_ranks);
  //printf("HELLO\n");
  MPI_Comm_rank(MPI_LOCAL_COMM, &my_rank);
  //MPI_Comm_get_name(MPI_LOCAL_COMM, comm_name, size);
  //for (i=0;i<num_ranks;i++){
  //  MPI_Barrier(*MPI_LOCAL_COMM);
  //  
  printf("%5u %3u\n",num_ranks, my_rank);
  //}
  
  if (num_ranks > 1){
    if (my_rank < num_ranks/2)
      MPI_Comm_split(MPI_LOCAL_COMM,
    		     0,
    		     my_rank,
    		     &MPI_TEMP_COMM);
    else
      MPI_Comm_split(MPI_LOCAL_COMM,
    		     1,
    		     my_rank,
    		     &MPI_TEMP_COMM);
    
    MPI_Comm_free(&MPI_LOCAL_COMM);
    //MPI_LOCAL_COMM = MPI_TEMP_COMM;
    j = MPI_Comm_dup(MPI_TEMP_COMM, &MPI_LOCAL_COMM);
    MPI_Comm_free(&MPI_TEMP_COMM);
    myFunc();
    
  }

}
