//#include "headerFuncs.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
//#include <ctime>
void readFromFile(const char* fname, int size, const int startline ){
  //Read in file and print it
  FILE *fp;
  int i = 0;
  long int id;
  long double x,y,z;
  char * line = NULL;
  size_t len=0;
  ssize_t read;
  size += startline;
  printf("startline: %u\n", startline);
  if ((fp = fopen(fname, "rb")) != NULL){
    while(!feof(fp) && i < size && i < startline){
      //fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num,&temp.xyz[0], &temp.xyz[1], &temp.xyz[2]);
      fread(&id,sizeof(long int),1,fp);
      fread(&x,sizeof(long double),1,fp); 
      fread(&y,sizeof(long double),1,fp); 
      fread(&z,sizeof(long double),1,fp); 
      //printf("%20Lu\t%15.5Lf\t%15.5Lf\t%15.5Lf\n", id, x, y, z);
      i++;
    }
    printf("i: %u\n", i);
    while(!feof(fp) && i < size && i >= startline){
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

void writeZeros(){
  FILE *binaryFile;
  char outfname[6];
  int i,j,k;
  long long int ival;
  long double vald[3];
  //outfname = "";
  for (i=0;i<10;i++){
    sprintf(outfname,"s%u.bin",i);
      
    binaryFile = fopen(outfname, "wb");
    if (!binaryFile) {
      printf(" output file opening issues!!\n");
      return;
    }
    for (j=0;j<1000;j++){
     
      ival = (long long int)(i*1000 + j + 1);
      fwrite(&ival, sizeof(long long int), 1, binaryFile);
      if (j<500){
	vald[0] = (long double)0;
	vald[1] = (long double)0;
	vald[2] = (long double)0;
      }else{
	vald[0] = (long double)rand()/RAND_MAX*2000.0-1000.0;
	vald[1] = (long double)rand()/RAND_MAX*2000.0-1000.0;
	vald[2] = (long double)rand()/RAND_MAX*2000.0-1000.0;
      }
      
      for (k =0; k< 3; k++){
	fwrite(&vald[k], sizeof(long double), 1, binaryFile);
      }
	
       
    }
    fclose(binaryFile);
  }

}

int main(int argc, char* argv[]) {
  
  srand(time(NULL));
  writeZeros();
  char fname[6];
  int size = 1000, startline=0, i;
  long long int ival;
  long double vald[3];
  srand(time(NULL));
  //outfname = "";
  for (i=0;i<10;i++){
    sprintf(fname,"s%u.bin",i); 
      
    readFromFile(fname,size,startline );
  }
  return 0;
}
