#include "headerFuncs.h"

void getallCount(int num, const int colIndex, void* varray, void *vallCounts){
  struct data_struct  *array = (struct data_struct *) varray;
  int* allCounts = (int *)vallCounts;
  int balanced;
  long double* nodeDivL = (long double *) malloc((num_ranks+2)*sizeof(long double));
  int* totalCount = (int *) malloc((num_ranks)*sizeof(int));
  long double* LDivinfo = (long double *) malloc(((num_ranks+2)*num_ranks)*sizeof(long double));
  long double* LDiv = (long double *) malloc(((num_ranks)*num_ranks)*sizeof(long double));
  long double* L = (long double *) malloc((num_ranks+1)*sizeof(long double));
  long double totalMax; // = LDivinfo[num_ranks];
  MPI_Status status;
  int total=0,K;
  int i, j=0, k=0, D;
  //double startTime[2], endTime[2], avgTime[2];
  //startTime[0] = timestamp();
  //printf("rank: %u, begin\n", my_rank);
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
  for (i=0;i<num_ranks;i++)
    nodeDivL[i] = array[i*D].xyz[colIndex];
  
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


  MPI_Allgather(nodeDivL, num_ranks+2,MPI_LONG_DOUBLE, LDivinfo,num_ranks+2,MPI_LONG_DOUBLE, MPI_LOCAL_COMM); 

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
  
  //printf("rank: %u, getCount\n", my_rank);
  
  getCounts(num, colIndex, array, L, totalCount, allCounts);
  checkBalance(&balanced, totalCount);
  //endTime[0] = timestamp() - startTime[0];
  //MPI_Reduce(&endTime[0], &startTime[0], 1, MPI_DOUBLE, MPI_SUM, 0, MPI_LOCAL_COMM);
  //avgTime[0] = startTime[0]/num_ranks;
  //if (my_rank == 0){
  //  //printf("HERE\n");
  //  printNodeL(L);
  //  printCount(allCounts);
  //}
  //============================
  //
  // Adjust L if not balanced
  //
  //============================
  
  //printf("node: %u; balanced: %u\n", my_rank,balanced);
  //if (balanced == 0)
  //printf("Before AdjustL rank: %u\n", my_rank);
  
  //startTime[1] = 0.0; //timestamp();
  adjustL(num, colIndex, array, L, allCounts, totalCount, &balanced);  
  //endTime[1] = timestamp() - startTime[1];
  //MPI_Reduce(&endTime[1], &startTime[1], 1, MPI_DOUBLE, MPI_MAX, 0, MPI_LOCAL_COMM);
  //if (my_rank == 0  && timePrint == 1)
  //  printf("%f;%f;", avgTime[0],startTime[1]);
  //printf("After AdjustL rank: %u\n", my_rank);
  
}

//============================
//
// If not balanced, get Vars
//
//
// sDiffi int [num_ranks] totalCount[i]-K
// rangeLi long double [num_ranks] L[i+1] - L[i-1] index 0 is blank
// totalRange long double L[num_ranks] - L[0]
// smallest long double min[array[i+1] - array[i]] > 0
// smallestDiff int min(sum(sDiffi), smallestDiff) for each iteration of adjustment
// smallestDiffL long double [num_ranks] L at smallestDiff
// K = total/N
// balance => abs(Si-K) < .10*K
//
// adjustCount int [num_ranks] number of adjustments for each limit index 0 is blank
// prevsDiffi = sDiffi
// percentRange double [num_ranks] the percent of rangeLi for each node
//============================
void adjustL(int num,  const int colIndex, void* varray, void *vL, void *vallCounts, void *vtotalCount, void *vbalanced){
  struct data_struct  *array = (struct data_struct *) varray;
  long double* L = (long double *) vL;
  int* allCounts = (int *)vallCounts; //malloc(num_ranks*n
  int* totalCount = (int *) vtotalCount; //malloc((num_ranks)*sizeof(int));
  int* balanced = (int *) vbalanced;
  if (*balanced != 0)
    return;
  int sDiff[num_ranks];
  int prevsDiff[num_ranks];
  long double rangeL[num_ranks];
  long double myrange;
  long double totalRange = L[num_ranks] - L[0];
  long double smallest;
  long double mysmallest;
  //long double * mysmallest = (long double *) malloc(sizeof(long double));
  long double * allSmallest = (long double *) malloc(num_ranks*sizeof(long double));
  int smallestDiff=0;
  int smallestDiffCheck=0;
  long double smallestDiffL[num_ranks];
  int smallestDiffBool=0;
  int adjustCount[num_ranks];
  double percentRange[num_ranks];

  MPI_Status status;
  int total=0,K;
  int i, j=0, k=0, D, startIndex = 1, totalIterations;
 
  //============================
  //
  // Get Required variables
  //
  //============================
  for (i=0; i<num_ranks*num_ranks;i++)
    total+=allCounts[i];
  K = total/num_ranks;
  
  for (i=0; i<num_ranks; i++){
    sDiff[i] = totalCount[i]-K;
    smallestDiff += sDiff[i];
    smallestDiffL[i] = L[i];
    adjustCount[i] = 0;
    percentRange[i] = 0.1;
    allSmallest[i] = 0;
    if (i > 0 )
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
  //smallest = mysmallest;
  MPI_Allreduce(&mysmallest, &smallest, 1, MPI_LONG_DOUBLE, MPI_MIN, MPI_LOCAL_COMM);
  
  //============================
  //
  // Adjust Each L until "Balanced"
  //
  //============================
  totalIterations = 0;
  startIndex = 1;
  while (*balanced == 0){
    
    *balanced = 1;
    for (i=startIndex; i<num_ranks;i++){ //check each Li for i = 1-> N-2
      if (fabsl(sDiff[i-1]) > 0.10*K){
    	*balanced = 0;
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
    	      //prevsDiff[i-1] = sDiff[i-1];
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
	//if (my_rank == 0){
	//  //printf("HERE\n");
	//  printNodeL(L);
	//  printCount(allCounts);
	//}
    	
    	// Set new Vars
    	smallestDiffCheck = 0;
    	for (j=0; j<num_ranks; j++){
    	  prevsDiff[j] = sDiff[j]; // Set prefsDiff before sDiff changes
    	  sDiff[j] = totalCount[j]-K;
    	  smallestDiffCheck += sDiff[j];
    	  if (j > 0)// j < num_ranks-1 )
    	    rangeL[j] = L[j+1] - L[j-1];
    	}
    	if (smallestDiffCheck < smallestDiff){
    	  smallestDiff = smallestDiffCheck;
    	  for (j=0; j<num_ranks; j++){
    	       smallestDiffL[i] = L[i];
    	  }
    	}
	break; 
      }
      
    }// break out of here  
    totalIterations++;
  }
  // Use smallestDiffL if it's better than L
  if (my_rank == 0){
    //printf("Total Adjustments: %u\n=====================\n", totalIterations);
    smallestDiffBool = 1;
    smallestDiffCheck = 0;
    for (j=0; j<num_ranks; j++){
      smallestDiffCheck += sDiff[j];
    }
    if (smallestDiff < smallestDiffCheck){
      smallestDiffBool = 0;
      for (j=0; j<num_ranks; j++){
  	L[i] = smallestDiffL[i];
      }
    }
  }
  //smallestDiffBool = 1;
  MPI_Bcast(&smallestDiffBool, 1, MPI_INT, 0, MPI_LOCAL_COMM);
  //printf("rank: %u; smallestDiffBool: %u\n",my_rank,smallestDiffBool);
  if (smallestDiffBool == 0){
    MPI_Bcast(L, num_ranks, MPI_LONG_DOUBLE, 0, MPI_LOCAL_COMM);
    getCounts(num, colIndex, array, L, totalCount, allCounts);
  }
  
}

void getCounts(int num,  const int colIndex, void* varray, void *vL, void *vtotalCount, void *vallCounts){
  struct data_struct  *array = (struct data_struct *) varray;
  long double* L = (long double *) vL;
  int* totalCount = (int *) vtotalCount;
  int* allCounts = (int *)vallCounts; //malloc(num_ranks*num_ranks*sizeof(int));
  int* nodeCount = (int *) malloc(num_ranks*sizeof(int));
  long double totalMax; // = LDivinfo[num_ranks];
  MPI_Status status;
  int total=0,K;
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
  //============================
  //
  // LINEAR METHOD
  //
  //============================

  //k=num_ranks-1;
  //for (i = num-1; i>= 0; i--){
  //  if (array[i].xyz[colIndex] >= L[k])
  //    nodeCount[k]++;
  //  else{
  //    while (array[i].xyz[colIndex] < L[k])
  //	k--;
  //    nodeCount[k]++;
  //  }
  //}
  //
  //for (i=0;i<num_ranks;i++){
  //  MPI_Barrier(MPI_LOCAL_COMM);
  //  if (my_rank == i){
  //    printf("my_rank: %u; BEFORE BISECTION\n", my_rank);
  //    for (j=0;j<num_ranks;j++)
  //	printf("nodeCount[%u]: %u\n", j, nodeCount[j]);
  //  }
  //}

  //============================
  //
  // BISECTION METHOD
  //
  //============================
  //int midi, maxi = num, mini=0, sumi = 0;
  //k = 0;
  //for (k=1;k<num_ranks;k++){
  //  while(1){
  //    midi = (int)(maxi+mini)/2;
  // 
  //    if (midi == mini || midi == maxi){
  //	if (L[k] >= array[mini].xyz[colIndex]){
  //	  midi = mini;
  //	  break;
  //	}else if (L[k] >= array[maxi].xyz[colIndex]){
  //	  midi = maxi;
  //	  break;
  //	}
  //	
  //    }
  //    if (L[k] < array[midi].xyz[colIndex])
  //	maxi=midi;
  //    else
  //	mini=midi;
  //  
  //  }
  //  mini = midi;
  //  maxi = num;
  //  
  //  if (k==1)
  //    nodeCount[k-1] = midi;
  //  else{
  //    nodeCount[k-1] = midi;
  //    for(j = k-2;j>=0;j--)
  //	nodeCount[k-1] -= nodeCount[j];
  //  }
  //}
  //nodeCount[num_ranks-1] = num;
  //for(j = num_ranks-2;j>=0;j--)
  //  nodeCount[num_ranks-1] -= nodeCount[j];

  //============================
  //
  // BISECTION METHOD2
  //
  //============================
  int midi, maxi = num, mini=0, sumi = 0;
  k = 0;
  //for (i=0;i<num_ranks;i++){
  //  MPI_Barrier(MPI_LOCAL_COMM);
  //  if (my_rank == i){
  //    printf("my_rank: %u\n", my_rank);
  for (k=1;k<num_ranks;k++){
    //if (my_rank == i)
    //  printf("%3s\t%3s\t%3s\t%3s\t%15s\t%15s\t%15s\n","k","min","mid","max","a[mid-1]","L[k]","a[mid]");
    j = 0;
    while (1){
      midi = (int)(maxi+mini)/2;
      //if (my_rank == i)
      //printf("%3u\t%3u\t%3u\t%3u\t%15Lf\t%15Lf\t%15Lf\n",k,mini,midi,maxi,array[midi-1].xyz[colIndex],L[k],array[midi].xyz[colIndex]);
      if (L[k] > array[midi].xyz[colIndex]){
	if (midi == mini){
	  midi = maxi;
	  break;
	}
	mini = midi;
      }else if (L[k] > array[midi-1].xyz[colIndex] || midi == mini){
	break;
      }else{
	maxi = midi;
      }  
    }
    maxi = num;
    mini = midi;
    if (k==1)
      nodeCount[k-1] = midi;
    else{
      nodeCount[k-1] = midi;
      for(j = k-2;j>=0;j--)
	nodeCount[k-1] -= nodeCount[j];
    }
    
  }
 //   }
 // }
  nodeCount[num_ranks-1] = num;
  for(j = num_ranks-2;j>=0;j--)
    nodeCount[num_ranks-1] -= nodeCount[j];

  //for (i=0;i<num_ranks;i++){
  //  MPI_Barrier(MPI_LOCAL_COMM);
  //  if (my_rank == i){
  //    printf("my_rank: %u; AFTER BISECTION2\n", my_rank);
  //    for (j=0;j<num_ranks;j++)
  //	printf("nodeCount[%u]: %u\n", j, nodeCount[j]);
  //  }
  //}
  //============================
  //
  // Get allCounts
  // share nodeCounts with all nodes
  //
  //============================

  
  MPI_Allgather(nodeCount, num_ranks,MPI_INT, allCounts,num_ranks,MPI_INT, MPI_LOCAL_COMM);
  
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
  int* totalCount = (int *) vtotalCount;
  int total=0,K;
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
  
  K = (int)(total/num_ranks);
  for (i=0;i<num_ranks-1;i++){
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
  for (i=0;i<num_ranks+1;i++)
    printf("%15Lf\t", i,L[i]);
  
  printf("\n");

}

void printCount(void *vallCounts){
  int* allCounts = (int *)vallCounts;
  int* totalCount = (int *) malloc(num_ranks*sizeof(int));
  int i,j,k;

  if (my_rank == 0){
    printf("%15s|", " ");
    for (i=0; i<num_ranks; i++){
      printf("%15u|", i);
      totalCount[i] = 0;
    }
    printf("\n");
    k = 0;
    for (i=0; i<num_ranks; i++){
      printf("%15u|",i);
      for (j=0; j<num_ranks; j++){
  	
  	printf("%15u|",allCounts[k]);
  	totalCount[j] += allCounts[k];
  	k++;
      }
      printf("\n");
      
    }
    printf("%15s|", "total");
    for (i=0; i<num_ranks; i++){
      printf("%15u|", totalCount[i]);
    }
    printf("\n================\n\n");
  }

}
