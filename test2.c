#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"


//struct data_struct{
//  long int num;
//  long double x;
//  long double y;
//  long double z;
//};

struct data_struct{
  long int num;
  long double xyz[3];
};

int compare_datastruct(const void* s1, const void* s2, int index){
  struct data_struct  *p1 = (struct data_struct *) s1;
  struct data_struct  *p2 = (struct data_struct *) s2;
  return p1->xyz[index] > p2->xyz[index];
}

int compare_x(const void* s1, const void* s2){
  //struct data_struct  *p1 = (struct data_struct *) s1;
  //struct data_struct  *p2 = (struct data_struct *) s2;
  //return p1->x > p2->x;
  return compare_datastruct(s1, s2, 0);
}

int compare_y(const void* s1, const void* s2){
  //struct data_struct  *p1 = (struct data_struct *) s1;
  //struct data_struct  *p2 = (struct data_struct *) s2;
  //return p1->y > p2->y;
  return compare_datastruct(s1, s2, 1);
}

int compare_z(const void* s1, const void* s2){
  //struct data_struct  *p1 = (struct data_struct *) s1;
  //struct data_struct  *p2 = (struct data_struct *) s2;
  //return p1->z > p2->z;
  return compare_datastruct(s1, s2, 2);
}


int main(int argc, char* argv[]){ 
  
  int num;
  FILE *fp;
  int i, j, total=0, K=0, colIndex =0;
  int num_ranks;
  int my_rank;
  struct data_struct temp;
  
  
  if (argc != 3){
    printf("Needs two argument and to be run with 2 cores.\nUsage <mpirun -n 2 test \"num\" infile>\nExiting now\n");
    exit(0);
  }

  // Number of elements to work on
  num = atoi(argv[1]);

  // declare array of struct
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));

  //Read in file and store in array
  i = 0;
  if ((fp = fopen(argv[2], "rb")) != NULL){
    while(!feof(fp) && i < num){
      //fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num,&temp.xyz[0], &temp.xyz[1], &temp.xyz[2]);
      fread(&temp.num,sizeof(long int),1,fp);
      fread(&temp.xyz[0],sizeof(long double),1,fp); 
      fread(&temp.xyz[1],sizeof(long double),1,fp); 
      fread(&temp.xyz[2],sizeof(long double),1,fp); 
      array[i++] = temp;
    }
    fclose(fp);
  }

  // vars for lower bound
  total = i;
  num_ranks = 2;
   
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
     
  MPI_Status status;
  MPI_Datatype array_type, data_type[2];
  int data_length[2];
  MPI_Aint displ[2], lower_bound, extent;

  //sort
  if (colIndex == 0)
    qsort(array, num, sizeof(struct data_struct), compare_x);
  else if (colIndex == 1)
    qsort(array, num, sizeof(struct data_struct), compare_y);
  else if (colIndex == 2)
    qsort(array, num, sizeof(struct data_struct), compare_z);
  else{
    printf("colIndex is between 0 and 2\n");
    exit(0);
  }
 
  //Get N lower values + max
  
  long double L[num_ranks+1]; //lower limits and max
  K = (int)(total/num_ranks);
  j = 0;
  L[num_ranks] = array[total-1].xyz[colIndex]; //array[0].x;
  for(i=0;i<total;i++){
    if (i%K == 0 && j<num_ranks){
      L[j] = array[i].xyz[colIndex];
      printf("L[%u]: %0.14Lf\n", j,L[j]);
      j++;
      
    }
    //if (L[num_ranks] < array[i].x)
    //  L[num_ranks] = array[i].x;
    
  }
  printf("L[%u]: %0.14Lf\n", num_ranks,L[j]);
      
  printf("total: %u\n", total);
  printf("    K: %u\n", K);
  
  printf("Lower Bounds and Max\n=======================\n\n");
  for (i=0;i<num_ranks+1;i++)
    printf("%0.15Lf\t", L[i]);
  printf("\n");
  
    
  // Describe the MPI_LONG_INT field in the struct
  displ[0] = 0;
  data_type[0] = MPI_LONG_INT;
  data_length[0] = 1;    
  
  //Describe the MPI_LONG_DOUBLE in the field.
  //Obtain offset using size of MPI_LONG_INT already described
  MPI_Type_get_extent(MPI_LONG_INT, &lower_bound, &extent);
  displ[1] = data_length[0] * extent;
  data_type[1] = MPI_LONG_DOUBLE;
  data_length[1] = 3;
  
  
  // Define the new data_type of our struct and commit
  MPI_Type_create_struct(2, data_length, displ, data_type, &array_type);
  MPI_Type_commit(&array_type);
  
  //Print before receiving changes from rank 0
  if (my_rank == 1){
    printf("\n\n\n");
    printf("\nArray before changes\n");
    printf("\n\n\n");
    for (i = 0; i < num; i++)
      printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
  }
  
  if (my_rank == 0){
    // Change all my array entries as defined below
    for (i = 0; i < num; i++){
      array[i].xyz[0] = 1.0*i;
      array[i].xyz[1] = 2.0*i;
      array[i].xyz[2] = 3.0*i;
    }
  
    // Send only index from 10 to 30
    MPI_Send(&array[10], 20, array_type, 1, 123, MPI_COMM_WORLD);
  }else{
    // I am rank 1
    // Receive index from 10 to 30. we could receive on any index
    MPI_Recv(&array[10], 20, array_type, 0, 123, MPI_COMM_WORLD, &status);
  
    printf("\n\n\n");
    printf("\nArray after changes\n");
    printf("\n\n\n");
  
    // Observe the change of array[10] to array[30] in the print below
    for (i = 0; i < num; i++)
      printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);    
  }
  MPI_Type_free(&array_type);
  free(array);
  MPI_Finalize();
}
