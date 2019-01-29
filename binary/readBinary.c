#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "mpi.h"

void readFromFile(const char* fname, const int size ){
  //Read in file and print it
  FILE *fp;
  int i = 0;
  long int id;
  long double x,y,z;
  if ((fp = fopen(fname, "rb")) != NULL){
    while(!feof(fp) && i < size){
      //fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num,&temp.xyz[0], &temp.xyz[1], &temp.xyz[2]);
      fread(&id,sizeof(long int),1,fp);
      fread(&x,sizeof(long double),1,fp); 
      fread(&y,sizeof(long double),1,fp); 
      fread(&z,sizeof(long double),1,fp); 
      printf("%20Lu\t%15.5Lf\t%15.5Lf\t%15.5Lf\n", id, x, y, z);
      i++;
    }
    fclose(fp);
  }else{
    printf("FILE DNE\n");
  }
}



int main(int argc, char* argv[]) {
  if (argc < 3){
    printf("NEED 2 args: number of lines, fileid\n");
    exit(0);
  }
  int num = atoi(argv[1]);
  char fname[18+46+7] = "";
  strcat(fname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile");
  strcat(fname, argv[2]);
  strcat(fname,".bin");
  printf("%71s\n", fname);
  //struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  int i = 0, k;
  long int j= 0, l=0;
  int num_ranks, my_rank;
  
  
  //MPI_Init(&argc, &argv);
  //MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  //MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  //
  readFromFile(fname,num);
  //
  //MPI_Finalize();
  return 0;
}
