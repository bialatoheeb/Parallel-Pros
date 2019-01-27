#include "headerFuncs.h"

void getL(const int num_ranks, const int my_rank, void * vnodeL, void * vLinfo, void * vL){
  long double  *L = (long double *) vL;
  long double  *Linfo = (long double *) vLinfo;
  long double  *nodeL = (long double *) vnodeL;
  
  int i,j,k=0;
  MPI_Allgather(nodeL, num_ranks+2,MPI_LONG_DOUBLE, Linfo,num_ranks+2,MPI_LONG_DOUBLE, MPI_COMM_WORLD); 
  
  for (i=0; ((num_ranks+2)*num_ranks); i++){
    if (i%(num_ranks+2) == 0){
      for (j=i;j<i+num_ranks; j++){
  	L[k] = Linfo[j];
  	k++;
  	if (k >= num_ranks*num_ranks)
  	  break;
      }
    }
    if (k >= num_ranks*num_ranks)
      break;
  }
  
}


void getCounts(const int num, const int colIndex, const void * varray, void * vL, void * vnodeCount, void * vtotalCount, void * vallCounts, int * balanced){
  struct data_struct  *array = (struct data_struct *) varray;
  long double  *L = (long double *) vL;
  long int *nodeCount = (long int *) vnodeCount;
  long int *totalCount = (long int *) vtotalCount;
  long int* allCounts = (long int *) vallCounts; //malloc(num_ranks*num_ranks*sizeof(long int));
  MPI_Status status;
  long int total=0,K;
  int i, j, k;
  j = 0;
  for (i=0;i<num_ranks;i++){
    nodeCount[i] = 0;
    totalCount[i] = 0;
    //for (k=0;k<num_ranks;k++){
    //  allCounts[j] = 0;
    //  j++;
    //}
  }
  
  k=num_ranks-1;
  for (i = num-1; i>= 0; i--){
    if (array[i].xyz[colIndex] >= L[k])
      nodeCount[k]++;
    else{
      while (array[i].xyz[colIndex] < L[k])
	k--;
      nodeCount[k]++;
    }
  }
  //for (i=0;i<num_ranks;i++){
  //  //nodeCount[i] = i + my_rank*10;
  //  printf("rank: %u;nodeCount[%u]: %Lu\n",my_rank, i,nodeCount[i]);
  //}
  
  //MPI_Allgather(nodeCount, num_ranks,MPI_LONG_INT, allCounts,num_ranks,MPI_LONG_INT, MPI_COMM_WORLD);

  for (i=0;i<num_ranks;i++){
    if (my_rank != i){
      for (k=0;k<num_ranks;k++)
	MPI_Send(nodeCount+k, 1, MPI_LONG_INT, i, k, MPI_COMM_WORLD);
    }else{
      for (j=0;j<num_ranks;j++){
	allCounts[j + my_rank*num_ranks] = nodeCount[j];
      }
    }
    totalCount[i] = 0;
  }

  for (i=0; i<num_ranks;i++){
    if (my_rank != i){
      for (k=0;k<num_ranks;k++)
	MPI_Recv(allCounts+i*num_ranks+k, 1, MPI_LONG_INT, i, k, MPI_COMM_WORLD, &status);
      
    }else{
      for (j=0;j<num_ranks;j++){
	allCounts[j + my_rank*num_ranks] = nodeCount[j];
      }
    }
  }
  
  ////if (my_rank == 0){
  //  for (i=0;i<num_ranks*num_ranks;i++){
  //    printf("rank: %u; allCounts[%u]: %Lu\n",my_rank, i,allCounts[i]);
  //  }
  //  //}

    //MPI_Barrier(MPI_COMM_WORLD);
    //printf("===========\n");
    //MPI_Barrier(MPI_COMM_WORLD);
  
  for (i=0;i<num_ranks;i++){
    for (j=0;j<num_ranks;j++){
      //printf("rank: %u; allCounts[%u]: %Lu\n",my_rank, i*num_ranks + j,allCounts[i*num_ranks + j]);
      totalCount[j] += allCounts[i*num_ranks + j];
    }
  
  }
  
  ////if (my_rank == 0){
  //  for (i=0;i<num_ranks;i++){
  //    printf("totalCount[%u]: %Lu\n",i,totalCount[i]);
  //    //  total += totalCount[i];
  //  }
    //}
  
  
  *balanced = 1;
  total = 0;
  for (i=0;i<num_ranks;i++){
    total += totalCount[i];
  }
  
  K = (long int)(total/num_ranks);
  for (i=0;i<num_ranks;i++){
    if (labs(totalCount[i]-K) > .10*K)
      *balanced = 0;
  }
  
  //printf("balanced: %u\n", *balanced);
  

  //free(allCounts);
  
  

}
