#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
        
  FILE *textFile, *binaryFile;
  char ch, chint[10], chd[22], infname[17+46], outfname[18+46+7];
  int i = 0, k;
  long int j= 0, l=0;
  long int vali;  // 1 value integer
  long double vald[3]; //3 values double
  int num_ranks, my_rank;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  
  for (i=1; i<=501; i++){
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
    j = (i-1)*20000000+1;
    //l = (i-1)*20000000+100;
    while (!feof(textFile)){
      fscanf(textFile, "%lu  %Lf  %Lf %Lf\n", &vali,&vald[0], &vald[1], &vald[2]);
      fwrite(&j, sizeof(long int), 1, binaryFile);
      
      for (k =0; k< 3; k++){
	fwrite(&vald[k], sizeof(long double), 1, binaryFile);
      }   
      j++;
      //printf("j: %lu\n",j);
      //if (j > (i-1)*20000000+100)
      //	break;
      //
    }
    printf("size: %lu\n", j - (i-1)*20000000);
  
     
    fclose(textFile);
    fclose(binaryFile);
  
  }
  MPI_Finalize();
  return 0;
}
