#include <stdio.h>
#include <string.h>
#include <stdlib.h>

  int main() {
        
        FILE *textFile, *binaryFile;
        char ch, chint[10], chd[22];
	int i = 0;
        long int vali;
	long double vald[3];

        
        /* input Text file */
        textFile = fopen("infile", "r");
        /* open output file in binary  */
        binaryFile = fopen("OutputFile.bin", "wb");

        /* file error handle */
        if (!textFile) 
        {
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
        //while (!feof(textFile)) 
        // {
        //        /* reading input file by  one byte of data  at a time */
        //        fread(&ch, sizeof(char), 1, textFile);
        //        /* character to ascii integer conversion   */
        //        val = ch;
        //        /* writing 4 byte of data to the output file */
        //        fwrite(&val, sizeof(int), 1, binaryFile);
        //        
        //}

	while (!feof(textFile)) 
         {
	   /* reading input file by  one byte of data  at a time */
	   //fread(&chint, sizeof(char), 10, textFile);
	   fscanf(textFile, "%lu  %Lf  %Lf %Lf\n", &vali,&vald[0], &vald[1], &vald[2]);
	   /* character to ascii integer conversion   */
	   //vali = atol(chint);
	   /* writing 4 byte of data to the output file */
	   fwrite(&vali, sizeof(long int), 1, binaryFile);
                
	   for (i =0; i< 3; i++){
	     /* reading input file by  one byte of data  at a time */
	     //fread(&chd, sizeof(char), 22, textFile);
	     /* character to ascii integer conversion   */
	     //vald = strtold(chd, NULL);
	     /* writing 4 byte of data to the output file */
	     fwrite(&vald[i], sizeof(long double), 1, binaryFile);
	   }                
        }

     
        fclose(textFile);
        fclose(binaryFile);
        return 0;
  }
