#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"
MPI_Comm MPI_LOCAL_COMM, MPI_TEMP_COMM, dup_comm_world;
MPI_Group world_group;
int num_ranks, global_num_ranks;
int my_rank, my_global_rank;
int main(int argc, char* argv[]) {
  int i, j, k, l;
  MPI_Status mystat;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  global_num_ranks = num_ranks;
  my_global_rank = my_rank;
  j = MPI_Comm_dup(MPI_COMM_WORLD, &MPI_LOCAL_COMM);
  
  //j = MPI_Comm_dup(MPI_COMM_WORLD, &dup_comm_world); //&MPI_LOCAL_COMM);
  //MPI_Comm_group( dup_comm_world, &world_group );
  //MPI_Comm_create( dup_comm_world, world_group, &MPI_LOCAL_COMM );
    
  MPI_Barrier(MPI_LOCAL_COMM);
  
  MPI_Finalize();
  return 0;
  
}
