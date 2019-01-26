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

int num_ranks;
int my_rank;
     
void getArraySize(const char*, int*);
void readFromFile(const char*, const int, void*);
void printFile( const int, void*);
int compare_datastruct(const void*, const void*, int);
int compare_x(const void*, const void*);
int compare_y(const void*, const void*);
int compare_z(const void*, const void*);

void getNodeL(const int, const int, void*, void*);
void printNodeL(void*);

void getL(void *, void *, void *);
void AllToAllSend(void *, void *, void *, MPI_Datatype);
#endif
