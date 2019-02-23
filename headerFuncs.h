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

struct node{
  long double max[3], min[3], maxRadius;
  struct node *left, *right;
  int num_below;
  struct data_struct *center; //leafValue;                                                                                                                                                     
};

//MPI_Comm MPI_COMM_WORLD;
MPI_Datatype array_type;
MPI_Datatype ld_type;
int num_ranks;
int my_rank;
int timePrint;

void getMaxMin(void*, int, int, long double*, long double*);
void getLargestDimension(long double *, long double *, int *);
void getNode(long double *, long double *, int, void *);
void printNode(void *);
void buildTree(void *, int, void *, int);
     
void getArraySize(const char*, int*);
void readFromFile(char*, const int, void*);
void readFromFileAllRead(int, void*);
void readFile1(int , void* );
void printFile( const int, void*);

double timestamp();
int compare_datastruct(const void*, const void*, int);
int compare_x(const void*, const void*);
int compare_y(const void*, const void*);
int compare_z(const void*, const void*);
int compare_longdouble(const void*, const void*);

void getallCount(int, const int, void*, void*);
void getCounts(int, const int, void*, void *, void *, void *);
void checkBalance(void *, void *);
void printNodeL(void*);
void adjustL(int, const int, void*, void *, void*, void *, void*);
void do_sort(struct data_struct*, int, int);
void printCounts(void *);
void verify(long double, long double);
//void getL(void *, void *, void *);
//void getCounts(const int num, const int colIndex, const void * varray, void * vL, void * vnodeCount, void * vtotalCount, void * vallCounts, int * balanced);

struct data_struct*  AllToAllSend(void *, int *, void *);
struct data_struct*  AllToAllIsend(void *, int *, void *);
void create_array_datatype();
#endif
