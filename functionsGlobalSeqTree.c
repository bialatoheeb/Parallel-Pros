#include "headerFuncs.h"

void buildTreeGlobal(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  long double *arrayMax = (long double *) malloc(3 * sizeof(long double));
  long double *arrayMin = (long double *) malloc(3 * sizeof(long double));
  
  int i,j,k;
  if (num > 1){
    //Find biggest Dimension
    // BINARY SEARCH FOR MAX AND MIN
    getMaxMinGlobal(array, num, colIndex, arrayMax, arrayMin);
    

    printf("%5s\t%15s\t%15s\t\n", "COL","MAX","MIN");
    for (i=0;i<3;i++){
      printf("%5u\t%15Lf\t%15Lf\t\n", i,arrayMax[i],arrayMin[i]);
    }
    getLargestDimensionGlobal(arrayMax,arrayMin, &colIndex);
    getNodeGlobal(arrayMax,arrayMin, num, anode);
    
    return;
    //printf("colUIndex: %u\n", colIndex);
    
    // Sort Along Dimension
    //do_sort(array, num, colIndex);
    // Calculate node values
  
    
    //printf("\n\n=========\nNODE\n");
    //printNode(anode);
    anode->left = (struct node *)malloc(sizeof(struct node));
    anode->right = (struct node *)malloc(sizeof(struct node));
    //printf("beforePRINT\n");
    //printNode(head);
    int index = (int)num/2;
    if (num%2 == 0){
      buildTreeGlobal(array, index, anode->left, colIndex);
      buildTreeGlobal(&array[index], index, anode->right, colIndex);
    }else{    
      buildTreeGlobal(array, index, anode->left, colIndex);
      buildTreeGlobal(&array[index], index+1, anode->right, colIndex);
    }
  }else{
    anode->center = array;
    printf("LEAF: %Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[0].num, array[0].xyz[0], array[0].xyz[1], array[0].xyz[2]);

    
  }
  
}

void getNodeGlobal(long double *arrayMax, long double *arrayMin, int num, void *vnode){
  struct node *anode = (struct node *)vnode;
  int i;
  long double radToMax=0, radToMin=0;
  anode->center = (struct data_struct *)malloc(sizeof(struct data_struct));
  anode->maxRadius = 0;
  for (i=0;i<3;i++){
    anode->max[i] = arrayMax[i];
    anode->min[i] = arrayMin[i];
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
  
  anode->num_below = num;
  

}

void printNodeGlobal(void *vnode){
  struct node *anode = (struct node *)vnode;
  printf("Max: (%15Lf,%15Lf,%15Lf);\n Min: (%15Lf,%15Lf,%15Lf);\n Center: (%15Lf,%15Lf,%15Lf);\n maxRadius: %15Lf;\n num_below: %15u\n", 
	 anode->max[0],anode->max[1],anode->max[2],
	 anode->min[0],anode->min[1],anode->min[2],
	 anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2],
	 anode->maxRadius,
	 anode->num_below);
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
   
  MPI_Allgather(arrayMax, 3, MPI_LONG_DOUBLE, allMax, 3,MPI_LONG_DOUBLE, MPI_LOCAL_COMM); 
  MPI_Allgather(arrayMin, 3, MPI_LONG_DOUBLE, allMin, 3,MPI_LONG_DOUBLE, MPI_LOCAL_COMM); 
    
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
    

    for (i=3;i<num_ranks;i+=3){
      
      if (arrayMax[colIndex] < allMax[i+colIndex])
	arrayMax[colIndex] = allMax[i+colIndex];
      else if (arrayMin[colIndex] > allMin[i+colIndex])
	arrayMin[colIndex] = allMin[i+colIndex];
      
    }
  }

}
