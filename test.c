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
  int i, colIndex = 0;
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
      fscanf(fp, "%lu  %Lf  %Lf %Lf\n", &temp.num, &temp.xyz[0], &temp.xyz[1], &temp.xyz[2]);
      array[i++] = temp;
    }
    fclose(fp);
  }
  
  for (i = 0; i < num ; i++)
    printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
  
  //qsort(array, num, sizeof(struct data_struct), compare_datastruct);
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
    

  for (i = 0; i < num ; i++)
    printf("%Lu\t%0.15Lf\t%0.15Lf\t%0.15Lf\n", array[i].num, array[i].xyz[0], array[i].xyz[1], array[i].xyz[2]);
   
}
