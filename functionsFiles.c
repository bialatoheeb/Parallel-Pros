#include "headerFuncs.h"

void getArraySize(const char* fname, int* size){
  *size = 20;
}
void readFromFile(char* fname, const int size, void* varray ){
  //Read in file and print it
  struct data_struct  *array = (struct data_struct *) varray;
  struct data_struct temp;
  
  FILE *fp;
  int i = 0;
  long int id;
  long double x,y,z;
  char * line = NULL;
  size_t len=0;
  ssize_t read;
  if ((fp = fopen(fname, "rb")) != NULL){
    while(!feof(fp) && i < size){
      //fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num,&temp.xyz[0], &temp.xyz[1], &temp.xyz[2]);
      fread(&temp.num,sizeof(long int),1,fp);
      fread(&temp.xyz[0],sizeof(long double),1,fp); 
      fread(&temp.xyz[1],sizeof(long double),1,fp); 
      fread(&temp.xyz[2],sizeof(long double),1,fp); 
      //printf("temp: %Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", temp.num, temp.xyz[0], temp.xyz[1], temp.xyz[2]);

      array[i].num = temp.num;
      array[i].xyz[0] = temp.xyz[0];
      array[i].xyz[1] = temp.xyz[1];
      array[i].xyz[2] = temp.xyz[2];
      //printf("array: %Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);

      //fread(&id,sizeof(long int),1,fp);
      //fread(&x,sizeof(long double),1,fp); 
      //fread(&y,sizeof(long double),1,fp); 
      //fread(&z,sizeof(long double),1,fp); 
      //printf("%20Lu\t%15.5Lf\t%15.5Lf\t%15.5Lf\n", id, x, y, z);
      i++;
    }
    fclose(fp);
  }else{
    printf("FILE DNE\n");
  }
}


void readFromFileAllRead(int sizeOnAll, void* varray ){
  //Read in file and print it
  struct data_struct  *array = (struct data_struct *) varray;
  struct data_struct temp;
  
  FILE *fp;
  int i = 0;
  long int id;
  long double x,y,z;
  char * line = NULL;
  size_t len=0;
  ssize_t read;
  int num_of_reads = 0;
  long int file_no, startline=0, tsize, size;
  size_t offset = 0;
  size_t dataSize = sizeof(long long int) +  3*sizeof(long double);
  char fname[71];
  
  if (my_rank < num_ranks-1){
    size = sizeOnAll/num_ranks;
    file_no = (long int)(size*my_rank/20000000 + 1);
    if (my_rank != 0){      
      startline = (long int)(my_rank*size%20000000) + 1;
      offset = (startline-1)*dataSize;
    }
  }else{
    tsize = sizeOnAll/num_ranks;
    size = tsize + sizeOnAll%num_ranks;
    file_no = (long int)(tsize*my_rank/20000000 + 1);
    startline = (long int)(my_rank*tsize%20000000) + 1;
    offset = (startline-1)*dataSize;
  }
  //printf("rank: %u, size: %lu; file_no: %lu; startline: %lu; offset: %lu\n", my_rank, size, file_no, startline, offset);
  sprintf(fname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", file_no);
  
  if ((fp = fopen(fname, "rb")) == NULL){
    printf("File DNE: %s\n", fname);
    MPI_Finalize();
    exit(0);
  }else{
    if (startline != 0){
      if ( fseek(fp,offset,SEEK_SET) != 0){
	printf("SEEK Failed\n");
	MPI_Finalize();
	exit(0);
      }
    }
  }
  while (num_of_reads < size){
    if (!feof(fp)){
      fread(&temp.num,sizeof(long int),1,fp);
      fread(&temp.xyz[0],sizeof(long double),1,fp); 
      fread(&temp.xyz[1],sizeof(long double),1,fp); 
      fread(&temp.xyz[2],sizeof(long double),1,fp); 
      //printf("temp: %Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", temp.num, temp.xyz[0], temp.xyz[1], temp.xyz[2]);
      
      array[num_of_reads].num = temp.num;
      array[num_of_reads].xyz[0] = temp.xyz[0];
      array[num_of_reads].xyz[1] = temp.xyz[1];
      array[num_of_reads].xyz[2] = temp.xyz[2];
      //printf("array: %Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].nu
      num_of_reads++;
    }else{
      sprintf(fname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", ++file_no);
      if ((fp = fopen(fname, "rb")) == NULL){
	printf("File DNE: %s\n", fname);
	MPI_Finalize();
	exit(0);
      }
    }
  }
  
}
void readFile1(int size, void * varray){
  struct data_struct  *array = (struct data_struct *) varray;
  struct data_struct temp;

  char fname[71];
  FILE *fp;
  int i, j;
  int size_on_nodes = size / num_ranks;
  MPI_Request send_req[num_ranks-1];
  MPI_Status stat;
  int ranks[num_ranks];
  int file_no = 1;

  for (j=0; j < num_ranks- 1; j++) 
    ranks[j] = size_on_nodes;
  ranks[num_ranks - 1] = size_on_nodes + (size % num_ranks);

  //  printf("size_on_nodes = %d\n", size_on_nodes);
  if (my_rank  ==  num_ranks-1){
    struct data_struct * temp_array = (struct data_struct *) malloc(size_on_nodes * sizeof(struct data_struct));
    sprintf(fname,"/home/tab7v/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", file_no);
    if ((fp = fopen(fname, "rb")) == NULL){
	MPI_Finalize();
	printf("Unable to open file %s\n", fname);
	exit(0);
    }
    for (j = 0; j < num_ranks; j++){
      i = 0;
      while (i < ranks[j]){
	if (!feof(fp)){
	  fread(&temp.num,sizeof(long int),1,fp);
	  fread(&temp.xyz[0],sizeof(long double),1,fp);
	  fread(&temp.xyz[1],sizeof(long double),1,fp);
	  fread(&temp.xyz[2],sizeof(long double),1,fp);
	  array[i++] = temp; 
	}else{
	  fclose(fp);
	  sprintf(fname,"/home/tab7v/localstorage/public/coms7900-data/binary/bdatafile%05u.bin", ++file_no);
	  i--;
	  if ( (fp = fopen(fname, "rb")) != NULL)
	    continue;
	}
      }
      if (j != (num_ranks - 1)){
	if (j > 0)
	  MPI_Wait(&send_req[j-1], &stat);
	memcpy(temp_array, array, size_on_nodes * sizeof(struct data_struct));
	MPI_Isend(temp_array, size_on_nodes, array_type, j, j, MPI_COMM_WORLD, &send_req[j]);
      }
    }
  }else{
    MPI_Recv(array, size_on_nodes, array_type, num_ranks-1, my_rank, MPI_COMM_WORLD, &stat);
  }

}






void printFile( const int size, void* varray ){
  //Read in file and store in array
  struct data_struct  *array = (struct data_struct *) varray;
  int i;
  for (i = 0; i < size; i++)
      printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);

}
