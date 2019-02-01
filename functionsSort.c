#include "headerFuncs.h"

void getallCount(const int num, const int colIndex, void* varray, void *vallCounts){
  struct data_struct  *array = (struct data_struct *) varray;
  long int* allCounts = (long int *) vallCounts;
  int balanced;
  long double* nodeDivL = (long double *) malloc((num_ranks+2)*sizeof(long double));
  long int* totalCount = (long int *) malloc((num_ranks)*sizeof(long int));
  long double* LDivinfo = (long double *) malloc(((num_ranks+2)*num_ranks)*sizeof(long double));
  long double* LDiv = (long double *) malloc(((num_ranks)*num_ranks)*sizeof(long double));
  long double* L = (long double*) (long double *) malloc((num_ranks+1)*sizeof(long double));
  long double totalMax; // = LDivinfo[num_ranks];
  MPI_Status status;
  long int total=0,K;
  int i, j=0, k=0, D;
					     
  //============================
  //
  // Get nodeDivL (the "percentiles")
  // the min of N evenly split groups on each node
  //
  //============================
  D = (int)(num/num_ranks);
  j = 0;
  nodeDivL[num_ranks+1] = num;
  nodeDivL[num_ranks] = array[num-1].xyz[colIndex]; //array[0].x;
  for(i=0;i<num;i++){
    if (i%D == 0 && j<num_ranks){
      nodeDivL[j] = array[i].xyz[colIndex];
      j++;
      
    }
    
  }
  //if (my_rank == 0){
  //  for (i=0;i< num_ranks+2;i++)
  //    printf("nodeDivL[%u]: %lf\n", i, nodeDivL[i]);
  //}
  
  //============================
  //
  // Get LDivinfo 
  // (all nodeDivLs + each nodes max + each nodes size)
  //
  //============================


  MPI_Allgather(nodeDivL, num_ranks+2,MPI_LONG_DOUBLE, LDivinfo,num_ranks+2,MPI_LONG_DOUBLE, MPI_COMM_WORLD); 

  //============================
  //
  // Get totalMax 
  // The max value across all nodes
  //
  //============================

					     
  totalMax = LDivinfo[num_ranks];
  for (i=0;i<num_ranks;i++){
    if (totalMax < LDivinfo[i*(num_ranks+2) + num_ranks])
      totalMax = LDivinfo[i*(num_ranks+2) + num_ranks];

  }
					     	
  //============================
  //
  // Get LDiv 
  // (all nodeDivLs)
  //
  //============================

  k=0;
  for (i=0; ((num_ranks+2)*num_ranks); i++){
    if (i%(num_ranks+2) == 0){
      for (j=i;j<i+num_ranks; j++){
  	LDiv[k] = LDivinfo[j];
  	k++;
  	if (k >= num_ranks*num_ranks)
  	  break;
      }
    }
    if (k >= num_ranks*num_ranks)
      break;
  }

  //============================
  //
  // Get L 
  // (lower Bound of each + totalMax)
  //
  //============================
  //for (i=0;i<(num_ranks)*num_ranks;i++)
  //  printf("rank: %u; LDiv[%u]: %0.14Lf\n",my_rank,i,LDiv[i]);
  qsort(LDiv, (num_ranks)*num_ranks, sizeof(long double), compare_longdouble);
  //
  //for (i=0;i<(num_ranks)*num_ranks;i++)
  //  printf("rank: %u; LDiv[%u]: %0.14Lf\n",my_rank,i,LDiv[i]);

  for (i=0;i<num_ranks;i++){
    L[i] = LDiv[i + i*num_ranks];
  }
  L[num_ranks] = totalMax;


  //============================
  //
  // Get Counts
  // check if total count balanced
  //
  //============================
  

  getCounts(num, colIndex, array, L, totalCount, allCounts);
  checkBalance(&balanced, totalCount);

  //============================
  //
  // Adjust L if not balanced
  //
  //============================
  
  
  //if (balanced == 0)
  //  adjustL(num, colIndex, array, L, allCounts, totalCount);  
  
}

//============================
//
// If not balanced, get Vars
//
//
// sDiffi long int [num_ranks] totalCount[i]-K
// rangeLi long double [num_ranks] L[i+1] - L[i-1] index 0 is blank
// totalRange long double L[num_ranks] - L[0]
// smallest long double min[array[i+1] - array[i]] > 0
// smallestDiff long int min(sum(sDiffi), smallestDiff) for each iteration of adjustment
// smallestDiffL long double [num_ranks] L at smallestDiff
// K = total/N
// balance => abs(Si-K) < .10*K
//
// adjustCount int [num_ranks] number of adjustments for each limit index 0 is blank
// prevsDiffi = sDiffi
// percentRange double [num_ranks] the percent of rangeLi for each node
//============================
void adjustL(const int num,  const int colIndex, void* varray, void *vL, void *vallCounts, void *vtotalCount){
  struct data_struct  *array = (struct data_struct *) varray;
  long double* L = (long double *) vL;
  long int* allCounts = (long int *) vallCounts; //malloc(num_ranks*n
  long int* totalCount = (long int *) vtotalCount; //malloc((num_ranks)*sizeof(long int));
  
  long int sDiff[num_ranks];
  long int prevsDiff[num_ranks];
  long double rangeL[num_ranks];
  long double myrange;
  long double totalRange = L[num_ranks] - L[0];
  long double smallest;
  long double mysmallest;
  //long double * mysmallest = (long double *) malloc(sizeof(long double));
  long double * allSmallest = (long double *) malloc(num_ranks*sizeof(long double));
  long int smallestDiff=0;
  long int smallestDiffCheck=0;
  long double smallestDiffL[num_ranks];
  int balanced = 0;
  int adjustCount[num_ranks];
  double percentRange[num_ranks];

  MPI_Status status;
  long int total=0,K;
  int i, j=0, k=0, D, startIndex = 1;

 
  //============================
  //
  // Get Required variables
  //
  //============================
  for (i=0; i<num_ranks;i++)
    total+=totalCount[i];
  K = total/num_ranks;

  for (i=0; i<num_ranks; i++){
    sDiff[i] = totalCount[i]-K;
    smallestDiff += sDiff[i];
    smallestDiffL[i] = L[i];
    adjustCount[i] = 0;
    percentRange[i] = 0.1;
    allSmallest[i] = 0;
    allSmallest[num_ranks+i] = 0;
    if (i > 0 && i < num_ranks-1 )
      rangeL[i] = L[i+1] - L[i-1];
  }
  mysmallest = 0;
  //============================
  //
  // Get smallest diff between vals
  //
  //============================

  mysmallest = array[num-1].xyz[colIndex] - array[0].xyz[colIndex];
  for (i=0;i<num-1; i++){
    smallest = array[i+1].xyz[colIndex] - array[i].xyz[colIndex];
    if (mysmallest > smallest && smallest > 0)
      mysmallest = smallest;
  }
  
  //MPI_Allgather(mysmallest, 1,MPI_LONG_DOUBLE, allSmallest,num_ranks,MPI_LONG_DOUBLE, MPI_COMM_WORLD); 

  smallest = mysmallest;
  for (i=0; i<num_ranks; i++){
    if (smallest > allSmallest[i])
      smallest = allSmallest[i];
  }
  


  for (i=0;i<num_ranks;i++){
    if (my_rank != i){
      MPI_Send(&mysmallest, 1, MPI_LONG_DOUBLE, i, 0, MPI_COMM_WORLD);
      	
    }else{
      allSmallest[my_rank] = mysmallest;
    }
  }
  for (i=0; i<num_ranks;i++){
    if (my_rank != i){
      MPI_Recv(allSmallest+i, 1, MPI_LONG_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
	
    }else{
      allSmallest[my_rank] = mysmallest;
    }
  }
  
    
  smallest = mysmallest;
  for (i=0; i<num_ranks; i++){
    //printf("rank: %u; allSmallest[%u]: %0.14Lf\n", my_rank, i, allSmallest[i]);
    if (smallest > allSmallest[i])
      smallest = allSmallest[i];
  }
  
  //printf("smallest: %0.14Lf\n", smallest);

  //============================
  //
  // Adjust Each L until "Balanced"
  //
  //============================
  
  while (balanced == 0){
    //printf("\n==============\n");
    //for (i=0;i<num_ranks;i++)
    //  printf("adjustCount[%u]: %u\n",i,adjustCount[i]);
    balanced = 1;
    for (i=startIndex; i<num_ranks;i++){ //check each Li for i = 1-> N-2
      if (fabsl(sDiff[i-1]) > 0.10*K){
	balanced == 0;
	// Use range > 0
	if (rangeL[i] > 0)
	  myrange = rangeL[i];
	else
	  myrange = totalRange;
	
	// Fix Range
	if (adjustCount[i] != 0){ // NOT first iteration
	  if (myrange*percentRange[i] > smallest){// if stepsize not to small
	  
	    if (fabsl(prevsDiff[i-1]) < fabsl(sDiff[i-1])) //previous step to big
	      percentRange[i] /=2;
	    else if (prevsDiff[i-1]*sDiff[i-1] > 0) // stepped over target
	      percentRange[i] /=2;
	  }else{                                 // stepsize too small
	    if (totalCount[i-1] > 0){ //skip node if count != 0
	      prevsDiff[i-1] = sDiff[i-1];
	      startIndex += 1;
	      break; // Go to next node immediately (skip the rest)
	    }
	  }
	}
	// Make Adjustments
	adjustCount[i] += 1;
	if (sDiff[i-1] > 0)
	  L[i] = L[i] - myrange*percentRange[i];
	else
	  L[i] = L[i] + myrange*percentRange[i];
	  
	// Fix if Li is out of range
	for (j=0;j<num_ranks-1;j++){
	  if (L[i] > L[i+1])
	    L[i+1] = L[i];
	}	 
	for (j=num_ranks;j>1;j--){
	  if (L[i] < L[i-1])
	    L[i-1] = L[i];
	}
	
	// getCounts
	getCounts(num, colIndex, array, L, totalCount, allCounts);

	// Set new Vars
	smallestDiffCheck = 0;
	for (j=0; j<num_ranks; j++){
	  sDiff[j] = totalCount[j]-K;
	  smallestDiffCheck += sDiff[j];
	  if (j > 0 && j < num_ranks-1 )
	    rangeL[j] = L[j+1] - L[j-1];
	}
	if (smallestDiffCheck < smallestDiff){
	  smallestDiff = smallestDiffCheck;
	  for (j=0; j<num_ranks; j++){
	       smallestDiffL[i] = L[i];
	  }
	} 
	
	// Adjust 1 limit at a time
	break; 
      }
    }// break out of here  
    
  }
  balanced = 1;
  for (i=0;i<num_ranks;i++){
    if (L[i]!=smallestDiffL[i]){
      balanced = 0;
      L[i] = smallestDiffL[i];
    }
  }
  if (balanced == 0)
    getCounts(num, colIndex, array, L, totalCount, allCounts);
  
	
}

void getCounts(const int num,  const int colIndex, void* varray, void *vL, void *vtotalCount, void *vallCounts){
  struct data_struct  *array = (struct data_struct *) varray;
  long double* L = (long double *) vL;
  long int* totalCount = (long int *) vtotalCount;
  long int* allCounts = (long int *) vallCounts; //malloc(num_ranks*num_ranks*sizeof(long int));
  long int* nodeCount = (long int *) malloc(num_ranks*sizeof(long int));
  long double totalMax; // = LDivinfo[num_ranks];
  MPI_Status status;
  long int total=0,K;
  int i, j=0, k=0, D;
  
  //============================
  //
  // Get nodeCount
  // Counts of each division on each node
  //
  //============================

  j = 0;
  for (i=0;i<num_ranks;i++){
    nodeCount[i] = 0;
    totalCount[i] = 0;
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
	
  //============================
  //
  // Get allCounts
  // share nodeCounts with all nodes
  //
  //============================

	
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
  
  //if (my_rank == 0){
  //for (i=0;i<num_ranks*num_ranks;i++){
  //  printf("rank: %u; allCounts[%u]: %Lu\n",my_rank, i,allCounts[i]);
  //}

  //============================
  //
  // Get totalCounts
  // the minimum of each node
  //
  //============================


  for (i=0;i<num_ranks;i++){
    for (j=0;j<num_ranks;j++){
      //printf("rank: %u; allCounts[%u]: %Lu\n",my_rank, i*num_ranks + j,allCounts[i*num_ranks + j]);
      totalCount[j] += allCounts[i*num_ranks + j];
    }
  
  }
}

void checkBalance(void *vbalanced, void *vtotalCount){
  int* balanced = (int *) vbalanced;
  long int* totalCount = (long int *) vtotalCount;
  long int total=0,K;
  int i;
  
  //============================
  //
  // Get K and check balance
  // K = total/N
  // balance => abs(Si-K) < .10*K
  //
  //============================


  *balanced = 1;
  total = 0;
  for (i=0;i<num_ranks;i++){
    total += totalCount[i];
  }
  
  K = (long int)(total/num_ranks);
  for (i=0;i<num_ranks;i++){
    if (labs(totalCount[i]-K) > .10*K){
      *balanced = 0;
      break;
    }
    
  }
  
  //printf("balanced: %u\n", *balanced);
  
}

void printNodeL(void* vL){
  long double  *L = (long double *) vL;
  int i;
  for (i=0;i<num_ranks+2;i++)
    printf("L[%u]: %0.14Lf\n", i,L[i]);
  
  printf("\n");

}
