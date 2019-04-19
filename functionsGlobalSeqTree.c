#include "headerFuncs.h"

struct node * buildTreeGlobal(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  
  int i,j,k, globalNum;
  if (num_ranks > 1){

    // BINARY SEARCH FOR MAX AND MIN
    getMaxMinGlobal(array, num, colIndex, anode->max, anode->min); //arrayMin);
    anode->num_below= -1;
    if (maxminflag == 1)
      printf("GETMAXMIN gid%d\n", my_global_rank);
    getLargestDimensionGlobal(anode->max, anode->min, &colIndex);
    if (largestdimflag == 1)
      printf("LARGESTDIM gid%d\n", my_global_rank);
    array = globalSort(array, &num, colIndex, &globalNum);
    if (globalsortflag == 1)
      printf("GLOBALSORT gid%d\n", my_global_rank);
    return splitRanks(array, num, anode, colIndex);
    
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
  float radToMax=0, radToMin=0;
  anode->center = (struct data_struct *)malloc(sizeof(struct data_struct));
  anode->maxRadius = 0;
  for (i=0;i<3;i++){
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

void  printNodeGlobal(void *vnode){
  struct node *anode = (struct node *)vnode;
  char fname[20];
  sprintf(fname,"/home/gst2d/Final/nodes%03u.txt", my_global_rank);
  FILE *myfile = fopen(fname, "a");
  fprintf(myfile,"LRank: %03u;\nMax: (%15f,%15f,%15f);\nMin: (%15f,%15f,%15f);\nCenter: (%15f,%15f,%15f);\nmaxRadius: %15f;\nnum_below: %15u\n========\n",
	  my_global_rank,
	  anode->max[0],anode->max[1],anode->max[2],
	  anode->min[0],anode->min[1],anode->min[2],
	  anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2],
	  anode->maxRadius,
	  anode->num_below);
  fclose(myfile);
}

void getLargestDimensionGlobal(float *arrayMax, float *arrayMin, int *colIndex){
  float range = arrayMax[0] - arrayMin[0];
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

void getMaxMinGlobal(void* varray, int size,  int colIndex, float *arrayMax, float *arrayMin){
  struct data_struct* array = (struct data_struct *)varray;
  float *allMax = (float *) malloc(3 * num_ranks * sizeof(float));
  float *allMin = (float *) malloc(3 * num_ranks * sizeof(float));
  
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

  MPI_Allgather(arrayMax, 3, ld_type, allMax, 3,ld_type, myCommCollection->localcomm); 
  MPI_Allgather(arrayMin, 3, ld_type, allMin, 3,ld_type, myCommCollection->localcomm); 
  //AllgatherLD(arrayMax, allMax, 3);
  //AllgatherLD(arrayMin, allMin, 3);
  
  
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


struct node * splitRanks(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  char fname[71] = "/home/gst2d/COMS7900/aout.txt";
  int i = 0, j, *size;
  if (num_ranks > 1){
    anode->left = (struct node *)malloc(sizeof(struct node));
    anode->right = (struct node *)malloc(sizeof(struct node));
    if (colIndex >= 0){
      for (i=0;i<3;i++){
	anode->left->max[i] = anode->max[i];
	anode->left->min[i] = anode->min[i];
	anode->right->max[i] = anode->max[i];
	anode->right->min[i] = anode->min[i];
      }           
    }
    
    
    
    if (my_rank < num_ranks/2){      
      if (myCommCollection->this_num_ranks > 1){
	myCommCollection = myCommCollection->next;
	num_ranks = myCommCollection->this_num_ranks;
	for (i=0;i<myCommCollection->this_num_ranks; i++){
	  if (my_global_rank == myCommCollection->ranks[i])
	    my_rank = i;
	}
      }else{
	num_ranks = 1;
	my_rank = 0;
      }
      return buildTreeGlobal(array, num, anode->left, colIndex);
    }else{      
      if (myCommCollection->this_num_ranks > 1){
	myCommCollection = myCommCollection->next;
	num_ranks = myCommCollection->this_num_ranks;
	for (i=0;i<myCommCollection->this_num_ranks; i++){
	  if (my_global_rank == myCommCollection->ranks[i])
	    my_rank = i;
	}
      }else{
	num_ranks = 1;
	my_rank = 0;
      }
      return buildTreeGlobal(array, num, anode->right, colIndex);
    }
    
    
  }


}
