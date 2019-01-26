#ifndef HEADER_FILE
#define HEADER_FILE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

struct data_struct{
  long int num;
  long double xyz[3];
};

void getArraySize(const char* fname, int* size);
void readFromFile(const char* fname, const int size, void* array);
void printFile( const int size, void* varray );


int compare_datastruct(const void* s1, const void* s2, int index);
int compare_x(const void* s1, const void* s2);
int compare_y(const void* s1, const void* s2);
int compare_z(const void* s1, const void* s2);

void getNodeL(const int nodes, const int total, const int colIndex, void* Lv, void* arrayv);
void printNodeL(const int num_ranks, void* vL);

void getL(int num_ranks, int my_rank, void * vnodeL, void * vLinfo, void * vL);

#endif
