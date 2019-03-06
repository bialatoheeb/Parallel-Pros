#include "headerFuncs.h"

void buildTree(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  long double *arrayMax = (long double *) malloc(3 * sizeof(long double));
  long double *arrayMin = (long double *) malloc(3 * sizeof(long double));
  int i,j,k;
  if (num > 1){
    //Find biggest Dimension
    // BINARY SEARCH FOR MAX AND MIN
    getMaxMin(array, num, colIndex, arrayMax, arrayMin);
    //printf("%5s\t%15s\t%15s\t\n", "COL","MAX","MIN");
    //for (i=0;i<3;i++){
    //  printf("%5u\t%15Lf\t%15Lf\t\n", i,arrayMax[i],arrayMin[i]);
    //}
    getLargestDimension(arrayMax,arrayMin, &colIndex);
    //printf("colUIndex: %u\n", colIndex);

    // Sort Along Dimension
    do_sort(array, num, colIndex);
    // Calculate node values
  
    getNode(arrayMax,arrayMin, num, anode);
    //printf("\n\n=========\nNODE\n");
    printNodeGlobal(anode);
    anode->left = (struct node *)malloc(sizeof(struct node));
    anode->right = (struct node *)malloc(sizeof(struct node));
    //printf("beforePRINT\n");
    //printNode(head);
    int index = (int)num/2;
    if (num%2 == 0){
      buildTree(array, index, anode->left, colIndex);
      buildTree(&array[index], index, anode->right, colIndex);
    }else{    
      buildTree(array, index, anode->left, colIndex);
      buildTree(&array[index], index+1, anode->right, colIndex);
    }
  }else{
    anode->center = array;

    char fname[20];
    sprintf(fname,"/home/gst2d/COMS7900/nodes%03u.txt", my_global_rank);
    FILE *myfile = fopen(fname, "a");
    fprintf(myfile,"LEAF: %Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[0].num, array[0].xyz[0], array[0].xyz[1], array[0].xyz[2]);
    fclose(myfile);
    

    
  }
  
}

void getNode(long double *arrayMax, long double *arrayMin, int num, void *vnode){
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

void printNode(void *vnode){
  struct node *anode = (struct node *)vnode;
  printf("Max: (%15Lf,%15Lf,%15Lf);\n Min: (%15Lf,%15Lf,%15Lf);\n Center: (%15Lf,%15Lf,%15Lf);\n maxRadius: %15Lf;\n num_below: %15u\n", 
	 anode->max[0],anode->max[1],anode->max[2],
	 anode->min[0],anode->min[1],anode->min[2],
	 anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2],
	 anode->maxRadius,
	 anode->num_below);
}

void getLargestDimension(long double *arrayMax, long double *arrayMin, int *colIndex){
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

void getMaxMin(void* varray, int size,  int colIndex, long double *arrayMax, long double *arrayMin){
  struct data_struct* array = (struct data_struct *)varray;
  //long double arrayMax[3], arrayMin[3];
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
  

}
