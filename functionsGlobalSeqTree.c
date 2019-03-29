#include "headerFuncs.h"

struct node * buildTreeGlobal(void *varray, int num, void *vnode, int colIndex){
  struct data_struct* array  = (struct data_struct *)varray;
  struct node *anode = (struct node *)vnode;
  //long double *arrayMax = (long double *) malloc(3 * sizeof(long double));
  //long double *arrayMin = (long double *) malloc(3 * sizeof(long double));
  
  int i,j,k, globalNum;
  
  //char fname[71] = "/home/gst2d/COMS7900/aout.txt";
  //FILE *fp = fopen(fname, "a");
  //fclose(fp);
  printf("INSID EBUILD TREE my_global_rank: %d, BADnum_ranks: %d num: %d\n",my_global_rank,num_ranks, num);
  //  printFile(num, array);
  //if (my_global_rank == 2)
  //  printf("%u\n", num_ranks);
  
  if (num_ranks > 1){
    //Find biggest Dimension
    // BINARY SEARCH FOR MAX AND MIN
    //if (my_global_rank == 0){
    //	printf("%5s\t%15s\t%15s\t\n", "COL","MAX","MIN");
    //	for (i=0;i<3;i++){
    //	  //printf("%5u\t%15Lf\t%15Lf\t\n", i,arrayMax[i],arrayMin[i]);
    //	  printf("%5u\t%15Lf\t%15Lf\t\n", i,anode->max[i],anode->min[i]);
    //	}
    //}
    //fprintf(fp,"GLOBAL TREE my_global_rank: %d, num: %d, num_ranks: %d\n",my_global_rank,num, num_ranks);
    
    //printf("BEFORE GET MAX MIN my_global_rank: %d, num: %d\n",my_global_rank,num);
    if (my_global_rank == 45)
      printf("MAXMIN\n");
  
    getMaxMinGlobal(array, num, colIndex, anode->max, anode->min); //arrayMin);
    //return anode;
    if (my_global_rank == 45)
      printf("LARGESTDIM\n");
  
    getLargestDimensionGlobal(anode->max, anode->min, &colIndex);
    //if (my_global_rank == 2){
    //  printf("before global Sort\n");
    //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", num-1, array[num-1].xyz[0], array[num-1].xyz[1], array[num-1].xyz[2]);
    //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", 0, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
    //}
    //printf("AFTER getLargestDim my_global_rank: %d, num: %d\n",my_global_rank,num);
    if (my_global_rank == 45)
      printf("GLOBALSORT\n");
  
    array = globalSort(array, &num, colIndex, &globalNum);
    //printf("AFTER globalSort  my_global_rank: %d, num: %d\n",my_global_rank,num);
    //return anode;
    //if (my_global_rank == 2){
    //  printf("after global Sort\n");
    //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", num-1, array[num-1].xyz[0], array[num-1].xyz[1], array[num-1].xyz[2]);
    //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", 0, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
    //}
    //getNodeGlobal(num, anode, globalNum);
    
    //printf("colIndex: %3u\n",colIndex);
    //printNodeGlobal(anode);
    
    if (my_global_rank == 45)
      printf("SPLITRANKS\n");
  
    
    return splitRanks(array, num, anode, colIndex);
    
    //return;
  }else{
    //if (my_global_rank == 1)
    //  printf("Max: (%15Lf,%15Lf,%15Lf);\n Min: (%15Lf,%15Lf,%15Lf);\n", 
    //	   anode->max[0],anode->max[1],anode->max[2],
    //	   anode->min[0],anode->min[1],anode->min[2]);
    getMaxMin(array, num, -1, anode->max, anode->min); //arrayMin);
    //if (my_global_rank == 1)
    //  printf("Max: (%15Lf,%15Lf,%15Lf);\n Min: (%15Lf,%15Lf,%15Lf);\n", 
    //	   anode->max[0],anode->max[1],anode->max[2],
    //	   anode->min[0],anode->min[1],anode->min[2]);
    //
    
    //getLargestDimension(anode->max, anode->min, &colIndex);
    //getNodeGlobal(anode->max, anode->min, num, anode);
    anode->num_below = num;
    anode->center = array;
    
    //printf("colIndex: %3u\n",colIndex);
    //printNodeGlobal(anode);
    return anode;
    
    //if (my_global_rank == 2){
    //  printf("before global Sort\n");
    //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", num-1, array[num-1].xyz[0], array[num-1].xyz[1], array[num-1].xyz[2]);
    //  printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", 0, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
    //}
    do_sort(array, num, colIndex);
    
    
  }
    //printf("colUIndex: %u\n", colIndex);
    
    // Sort Along Dimension
    //do_sort(array, num, colIndex);
    // Calculate node values
  
    
    //printf("\n\n=========\nNODE\n");
    //
    //printf("beforePRINT\n");
    //printNode(head);
    
  
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
  //char fname[71] = "/home/gst2d/COMS7900/aout.txt";
  //FILE *fp = fopen(fname, "a");
  //printf("maxMinGlobal my_global_rank: %d, num: %d\n",my_global_rank,size);
  //fclose(fp);
  //return;
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
  MPI_Allgather(arrayMax, 3, ld_type, allMax, 3,ld_type, MPI_LOCAL_COMM); 
  MPI_Allgather(arrayMin, 3, ld_type, allMin, 3,ld_type, MPI_LOCAL_COMM); 
  
  
  //fprintf(fp,"AFTER MPI maxMinGlobal my_global_rank: %d, num: %d\n",my_global_rank,size);
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
  //FILE *fp = fopen(fname, "a");
  //fprintf(fp,"splitRanks: my_global_rank: %d, num: %d\n",my_global_rank,num);
  //fclose(fp);

  int i = 0, j, *size;
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
      MPI_Comm_split(MPI_LOCAL_COMM,
    		     0,
    		     my_rank,
    		     &MPI_TEMP_COMM);
      MPI_Comm_free(&MPI_LOCAL_COMM);
      //MPI_LOCAL_COMM = MPI_TEMP_COMM;
      j = MPI_Comm_dup(MPI_TEMP_COMM, &MPI_LOCAL_COMM);
      MPI_Comm_free(&MPI_TEMP_COMM);
      MPI_Comm_size(MPI_LOCAL_COMM, &num_ranks);
      //printf("HELLO\n");
      MPI_Comm_rank(MPI_LOCAL_COMM, &my_rank);
      return buildTreeGlobal(array, num, anode->left, colIndex);
      //myFunc();
    
    }else{
      MPI_Comm_split(MPI_LOCAL_COMM,
    		     1,
    		     my_rank,
    		     &MPI_TEMP_COMM);
      MPI_Comm_free(&MPI_LOCAL_COMM);
      //MPI_LOCAL_COMM = MPI_TEMP_COMM;
      j = MPI_Comm_dup(MPI_TEMP_COMM, &MPI_LOCAL_COMM);
      MPI_Comm_free(&MPI_TEMP_COMM);
      MPI_Comm_size(MPI_LOCAL_COMM, &num_ranks);
      //printf("HELLO\n");
      MPI_Comm_rank(MPI_LOCAL_COMM, &my_rank);
      return buildTreeGlobal(array, num, anode->right, colIndex);
      //myFunc();
    
    }
    
    
  }


}
