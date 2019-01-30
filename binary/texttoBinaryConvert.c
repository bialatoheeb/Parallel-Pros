#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"



int main(int argc, char* argv[]) {
  FILE *textFile, *binaryFile;
  char ch, chint[10], chd[22], infname[17+46], outfname[18+46+7];
  int i = 0, k;
  long long int m;
  long int j= 0, l=0;
  long int vali;  // 1 value integer
  long double vald[3]; //3 values double
  int num_ranks, my_rank;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  
  for (i=109; i<=499; i++){
    sprintf(infname,"/home/gst2d/localstorage/public/coms7900-data/datafile%05u.txt",i);
    sprintf(outfname,"/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile%05u.bin",i);
    printf("%63s\n", infname);
    printf("%71s\n", outfname);
    ///* input Text file */
    textFile = fopen(infname, "r");
    /* open output file in binary  */
    binaryFile = fopen(outfname, "wb");
    
    /* file error handle */
    if (!textFile) {
      printf("input file opening issues!!\n");
      return 0;
    }
  
  
    /* file error handle */
    if (!binaryFile) {
      printf(" output file opening issues!!\n");
      return 0;
    }
  
    /*  Read input file and store in binary form 
     */       
    j = (i-1);
    j *= 20000000;
    j += 1;
    m = (i-1);
    m *= 20000000;
    
    //l = (i-1)*20000000+100;
    while (!feof(textFile)){

      for (k=0;k<10;k++)
	  ch = getc(textFile);
      fscanf(textFile, "%Lf  %Lf %Lf\n",&vald[0], &vald[1], &vald[2]);
      //if (j > (i-1)*20000000+20000000-1){
      //	for (k=0;k<10;k++)
      //	  ch = getc(textfile);
      //	fscanf(textFile, "%Lf  %Lf %Lf\n",&vald[0], &vald[1], &vald[2]);
      //}else{
      //	fscanf(textFile, "%lu  %Lf  %Lf %Lf\n", &vali,&vald[0], &vald[1], &vald[2]);
      //}
      fwrite(&j, sizeof(long long int), 1, binaryFile);
      
      for (k =0; k< 3; k++){
	fwrite(&vald[k], sizeof(long double), 1, binaryFile);
      }   
      j++;
      //printf("m: %llu\n",m);
      //printf("i: %lu\n",i);
      //printf("%lu  %Lf  %Lf %Lf\n", j,vald[0], vald[1], vald[2]);
      //if (j > (i-1)*m)
      //	break;
      
    }
    j -= m;
    printf("size: %lu\n", j);
  
     
    fclose(textFile);
    fclose(binaryFile);
  
  }
  MPI_Finalize();
  return 0;
}
