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

void printFile( const int size, void* varray ){
  //Read in file and store in array
  struct data_struct  *array = (struct data_struct *) varray;
  int i;
  for (i = 0; i < size; i++)
      printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);

}
