#include "headerFuncs.h"

void getArraySize(const char* fname, int* size){
  *size = 20;
}
void readFromFile(const char* fname, const int size, void* varray ){
  //Read in file and store in array
  struct data_struct  *array = (struct data_struct *) varray;
  FILE *fp;
  int i = 0;
  struct data_struct temp;
  if ((fp = fopen(fname, "rb")) != NULL){
    while(!feof(fp) && i < size){
      //fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num,&temp.xyz[0], &temp.xyz[1], &temp.xyz[2]);
      fread(&temp.num,sizeof(long int),1,fp);
      fread(&temp.xyz[0],sizeof(long double),1,fp); 
      fread(&temp.xyz[1],sizeof(long double),1,fp); 
      fread(&temp.xyz[2],sizeof(long double),1,fp); 
      array[i++] = temp;
    }
    fclose(fp);
  }
}

void printFile( const int size, void* varray ){
  //Read in file and store in array
  struct data_struct  *array = (struct data_struct *) varray;
  int i;
  for (i = 0; i < size; i++)
      printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);

}
