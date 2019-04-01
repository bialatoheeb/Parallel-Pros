#include "headerFuncs.h"

struct node * buildTreeGlobal(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  //long double *arrayMax = (long double *) malloc(3 * sizeof(long double));
  //long double *arrayMin = (long double *) malloc(3 * sizeof(long double));
  
  int i,j,k, globalNum;
  printf("BUILDTREEGLOBAL%d %d\n", num, my_global_rank);
  if (num_ranks > 1){
    //Find biggest Dimension
    // BINARY SEARCH FOR MAX AND MIN
    getMaxMinGlobal(array, num, colIndex, anode->max, anode->min); //arrayMin);
    printf("MAXMIN%d %d\n", my_rank, my_global_rank);
    getLargestDimensionGlobal(anode->max, anode->min, &colIndex);
    printf("LARGESTDIM%d %d\n", my_rank, my_global_rank);
    array = globalSort(array, &num, colIndex, &globalNum);
    printf("GLOBALSORT%d %d\n", my_rank, my_global_rank);
    return splitRanks(array, num, anode, colIndex);
    
    //return;
  }else{
    getMaxMin(array, num, -1, anode->max, anode->min); //arrayMin);
    anode->num_below = num;
    anode->center = array;
    
    return anode;
    
    
  }
}

void getNodeGlobal(int num, void *vnode, int globalNum){
  struct node *anode = (struct node *)vnode;
  int i;
  long double radToMax=0, radToMin=0;
  anode->center = (struct data_struct *)malloc(sizeof(struct data_struct));
  anode->maxRadius = 0;
  for (i=0;i<3;i++){
    //anode->max[i] = arrayMax[i];
    //anode->min[i] = arrayMin[i];
    anode->center->xyz[i] = (anode->max[i] + anode->min[i])/2;
    radToMax += (anode->center->xyz[i]-anode->max[i])*(anode->center->xyz[i]-anode->max[i]);
    radToMin += (anode->center->xyz[i]-anode->min[i])*(anode->center->xyz[i]-anode->min[i]);
  }
  
  radToMax = sqrt(radToMax);
  radToMin = sqrt(radToMin);
  if (radToMax > radToMin)
    anode->maxRadius = radToMax;
  else
    anode->maxRadius = radToMin;
  
  anode->num_below = globalNum;
  
}

void printNodeGlobal(void *vnode){
  struct node *anode = (struct node *)vnode;
  char fname[20];
  sprintf(fname,"/home/gst2d/COMS7900/nodes%03u.txt", my_global_rank);
  FILE *myfile = fopen(fname, "a");
  fprintf(myfile,"LRank: %03u;\nMax: (%15Lf,%15Lf,%15Lf);\nMin: (%15Lf,%15Lf,%15Lf);\nCenter: (%15Lf,%15Lf,%15Lf);\nmaxRadius: %15Lf;\nnum_below: %15u\n========\n",
	  my_rank,
	  anode->max[0],anode->max[1],anode->max[2],
	  anode->min[0],anode->min[1],anode->min[2],
	  anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2],
	  anode->maxRadius,
	  anode->num_below);
  fclose(myfile);
}

void getLargestDimensionGlobal(long double *arrayMax, long double *arrayMin, int *colIndex){
  long double range = arrayMax[0] - arrayMin[0];
  *colIndex = 0;
  if (range < (arrayMax[1] - arrayMin[1])){
    *colIndex = 1;
    range = arrayMax[1] - arrayMin[1];
  }
  if (range < (arrayMax[2] - arrayMin[2])){
    *colIndex = 2;
    range = arrayMax[2] - arrayMin[2];
  }
  
}

void getMaxMinGlobal(void* varray, int size,  int colIndex, long double *arrayMax, long double *arrayMin){
  struct data_struct* array = (struct data_struct *)varray;
  //long double arrayMax[3], arrayMin[3];
  long double *allMax = (long double *) malloc(3 * num_ranks * sizeof(long double));
  long double *allMin = (long double *) malloc(3 * num_ranks * sizeof(long double));
  
  int i,j,k;
  if (colIndex < 0){
    for (i=0;i<3;i++){
      arrayMax[i] = array[0].xyz[i];
      arrayMin[i] = array[0].xyz[i];
    }

    for (i=1;i<size;i++){
      for (j=0;j<3;j++){
	if (arrayMax[j] < array[i].xyz[j])
	  arrayMax[j] = array[i].xyz[j];
	else if (arrayMin[j] > array[i].xyz[j])
	  arrayMin[j] = array[i].xyz[j];
      }
    }
  }else{
    arrayMax[colIndex] = array[0].xyz[colIndex];
    arrayMin[colIndex] = array[0].xyz[colIndex];
    

    for (i=1;i<size;i++){
      
      if (arrayMax[colIndex] < array[i].xyz[colIndex])
	arrayMax[colIndex] = array[i].xyz[colIndex];
      else if (arrayMin[colIndex] > array[i].xyz[colIndex])
	arrayMin[colIndex] = array[i].xyz[colIndex];
      
    }
  }

  //MPI_Allgather(arrayMax, 3, MPI_LONG_DOUBLE, allMax, 3,MPI_LONG_DOUBLE, MPI_LOCAL_COMM); 
  //MPI_Allgather(arrayMin, 3, MPI_LONG_DOUBLE, allMin, 3,MPI_LONG_DOUBLE, MPI_LOCAL_COMM); 
  MPI_Allgather(arrayMax, 3, ld_type, allMax, 3,ld_type, myCommCollection->localcomm); 
  MPI_Allgather(arrayMin, 3, ld_type, allMin, 3,ld_type, myCommCollection->localcomm); 
  
  
  if (colIndex < 0){
    for (i=0;i<3;i++){
      arrayMax[i] = allMax[i];
      arrayMin[i] = allMin[i];
    }

    for (i=3;i<num_ranks*3;i+=3){
      for (j=0;j<3;j++){
	if (arrayMax[j] < allMax[i+j])
	  arrayMax[j] = allMax[i+j];
	else if (arrayMin[j] > allMin[i+j])
	  arrayMin[j] = allMin[i+j];
      }
    }
  }else{
    arrayMax[colIndex] = allMax[colIndex];
    arrayMin[colIndex] = allMin[colIndex];
    
    for (i=3;i<num_ranks*3;i+=3){
      
      if (arrayMax[colIndex] < allMax[i+colIndex])
    	arrayMax[colIndex] = allMax[i+colIndex];
      else if (arrayMin[colIndex] > allMin[i+colIndex])
    	arrayMin[colIndex] = allMin[i+colIndex];
      
    }
  }

  
  


}

//struct node * splitRanks(void *varray, int num, void *vnode, int colIndex){
//  struct data_struct* array  = (struct data_struct *)varray;
//  struct node *anode = (struct node *)vnode;
//  char fname[71] = "/home/gst2d/COMS7900/aout.txt";
//  //FILE *fp = fopen(fname, "a");
//  //fprintf(fp,"splitRanks: my_global_rank: %d, num: %d\n",my_global_rank,num);
//  //fclose(fp);
//
//  int i = 0, j, *size;
//  if (num_ranks > 1){
//    anode->left = (struct node *)malloc(sizeof(struct node));
//    anode->right = (struct node *)malloc(sizeof(struct node));
//    if (colIndex >= 0){
//      for (i=0;i<3;i++){
//	//if (colIndex != i){
//	  anode->left->max[i] = anode->max[i];
//	  anode->left->min[i] = anode->min[i];
//	  anode->right->max[i] = anode->max[i];
//	  anode->right->min[i] = anode->min[i];
//	  //}
//      }           
//    }
//    if (my_rank < num_ranks/2){
//      MPI_Comm_split(MPI_LOCAL_COMM,
//    		     0,
//    		     my_rank,
//    		     &MPI_TEMP_COMM);
//      MPI_Comm_free(&MPI_LOCAL_COMM);
//      //MPI_LOCAL_COMM = MPI_TEMP_COMM;
//      j = MPI_Comm_dup(MPI_TEMP_COMM, &MPI_LOCAL_COMM);
//      MPI_Comm_free(&MPI_TEMP_COMM);
//      MPI_Comm_size(MPI_LOCAL_COMM, &num_ranks);
//      //printf("HELLO\n");
//      MPI_Comm_rank(MPI_LOCAL_COMM, &my_rank);
//      return buildTreeGlobal(array, num, anode->left, colIndex);
//      //myFunc();
//    
//    }else{
//      MPI_Comm_split(MPI_LOCAL_COMM,
//    		     1,
//    		     my_rank,
//    		     &MPI_TEMP_COMM);
//      MPI_Comm_free(&MPI_LOCAL_COMM); 
//      //MPI_LOCAL_COMM = MPI_TEMP_COMM;
//      j = MPI_Comm_dup(MPI_TEMP_COMM, &MPI_LOCAL_COMM);
//      MPI_Comm_free(&MPI_TEMP_COMM);
//      MPI_Comm_size(MPI_LOCAL_COMM, &num_ranks);
//      //printf("HELLO\n");
//      MPI_Comm_rank(MPI_LOCAL_COMM, &my_rank);
//      return buildTreeGlobal(array, num, anode->right, colIndex);
//      //myFunc();
//    
//    }
//    
//    
//  }
//
//
//}


struct node * splitRanks(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  char fname[71] = "/home/gst2d/COMS7900/aout.txt";
  int i = 0, j, *size;
  MPI_Barrier(myCommCollection->localcomm);
  //printf("SPLIT RANKS num_ranks: %d\n",num_ranks);
  if (num_ranks > 1){
    anode->left = (struct node *)malloc(sizeof(struct node));
    anode->right = (struct node *)malloc(sizeof(struct node));
    if (colIndex >= 0){
      for (i=0;i<3;i++){
	//if (colIndex != i){
	anode->left->max[i] = anode->max[i];
	anode->left->min[i] = anode->min[i];
	anode->right->max[i] = anode->max[i];
	anode->right->min[i] = anode->min[i];
	//}
      }           
    }
    
    if (my_rank < num_ranks/2){
      myCommCollection = myCommCollection->next;
      MPI_Group_free(&myCommCollection->prev->localgroup);
      MPI_Comm_free(&myCommCollection->prev->localcomm);
      free(myCommCollection->prev);
      if (myCommCollection->this_num_ranks > 1){
	MPI_Comm_size(myCommCollection->localcomm, &num_ranks);
	MPI_Comm_rank(myCommCollection->localcomm, &my_rank);
      }else{
	num_ranks = myCommCollection->this_num_ranks;
	my_rank = 0;
      }
      return buildTreeGlobal(array, num, anode->left, colIndex);
    }else{
      myCommCollection = myCommCollection->next;
      MPI_Group_free(&myCommCollection->prev->localgroup);
      MPI_Comm_free(&myCommCollection->prev->localcomm);
      free(myCommCollection->prev);
      if (myCommCollection->this_num_ranks > 1){
	MPI_Comm_size(myCommCollection->localcomm, &num_ranks);
	MPI_Comm_rank(myCommCollection->localcomm, &my_rank);
      }else{
	num_ranks = myCommCollection->this_num_ranks;
	my_rank = 0;
      }
      return buildTreeGlobal(array, num, anode->right, colIndex);
    }
    
    
  }


}
