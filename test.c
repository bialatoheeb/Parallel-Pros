/*
 Authors: Toheeb A. Biala, Khem Narayan Poudel, Gabriel Toban
 This code reads in the data and sort (based on any desired coordinates) 
 using a variant of the quicksort  in the C standard library.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct data_struct{
  long int num;
  long double x;
  long double y;
  long double z;
};

int compare_x(const void* s1, const void* s2){
  struct data_struct  *p1 = (struct data_struct *) s1;
  struct data_struct  *p2 = (struct data_struct *) s2;
  return p1->x > p2->x;
}

int compare_y(const void* s1, const void* s2){
  struct data_struct  *p1 = (struct data_struct *) s1;
  struct data_struct  *p2 = (struct data_struct *) s2;
  return p1->y > p2->y;
}

int compare_z(const void* s1, const void* s2){
  struct data_struct  *p1 = (struct data_struct *) s1;
  struct data_struct  *p2 = (struct data_struct *) s2;
  return p1->z > p2->z;
}

int main(int argc, char* argv[]){ 
  
  int num;
  FILE *fp;
  int i;
  struct data_struct temp;
  
  if (argc != 3){
    printf("Needs two arguments. Exting now\n");
    exit(0);
  }

  // Number of elements to sort
  num = atoi(argv[1]);

  // Array of struct
  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  
  i = 0;
  if ((fp = fopen(argv[2], "r")) != NULL){
    while(!feof(fp) && i < num){
      fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num, &temp.x, &temp.y, &temp.z);
      array[i++] = temp;
    }
    fclose(fp);
  }
  
  for (i = 0; i < num ; i++)
    printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].x, array[i].y, array[i].z);
  
  
   //qsort(array, num, sizeof(struct data_struct), compare_x);
   //qsort(array, num, sizeof(struct data_struct), compare_y);
   qsort(array, num, sizeof(struct data_struct), compare_z);

   for (i = 0; i < num ; i++)
     printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].x, array[i].y, array[i].z);
   
}
