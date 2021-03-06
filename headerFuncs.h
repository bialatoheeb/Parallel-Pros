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

struct Gnode{
  long double max[3], min[3], maxRadius;
  struct Gnode *left, *right, *parent;
  int this_rank, num_below, assigned;
  struct data_struct *center; //leafValue;
};

struct commgroupcollection{
  int this_num_ranks;
  int *ranks;// = {0,0,0};
  MPI_Group localgroup;
  MPI_Comm localcomm;
  struct commgroupcollection * prev;
  struct commgroupcollection * next;

};

//MPI_Comm MPI_COMM_WORLD;
MPI_Comm MPI_LOCAL_COMM, MPI_TEMP_COMM, dup_comm_world;
MPI_Group world_group;
MPI_Datatype array_type;
MPI_Datatype ld_type, li_type;
struct commgroupcollection * myCommCollection, * tempCollection;
int num_ranks, global_num_ranks, numRanges, maxLevel;
int my_rank, my_global_rank;
int timePrint;

void compareFunc(struct data_struct*, struct data_struct, int);
struct Gnode * buildEmptyGtree(struct Gnode *, int, int);
void getGNode(void *);
void printGNode(void *);
void printGTree(void *);
void getSendSize(struct Gnode *, double, struct data_struct *, int, int *);
void initAssigned(struct Gnode *);
void getSendSize1(struct Gnode *, double, struct data_struct *, int, int *);
void getSendSize1Target(struct Gnode *, double, struct data_struct, int *);
int sendSizeTest(struct node *, double, struct data_struct *, int);
void searchTest(struct data_struct *,struct data_struct*, int);
void compareTargets(struct data_struct *, struct data_struct *, int, int);
void compareTargetsLocalTree(struct node *, struct data_struct *, int);
void globalTreeMaster(struct Gnode *, struct node *);

struct data_struct* globalSort(void *, int *, int, int *);

int localSearch(struct node *, double, struct data_struct );

void getMaxMin(void*, int, int, long double*, long double*);
void getLargestDimension(long double *, long double *, int *);
void getNode(int, void *);
void printNode(void *);
void buildTree(void *, int, void *, int);

void getMaxMinGlobal(void*, int, int, long double*, long double*);
void getLargestDimensionGlobal(long double *, long double *, int *);
void getNodeGlobal(int, void *, int);
void printNodeGlobal(void *);
struct node * buildTreeGlobal(void *, int, void *, int);
struct node * splitRanks(void *, int, void *, int);
     
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

struct data_struct*  AllToAllSend(void *, int *, void *);
struct data_struct*  AllToAllIsend(void *, int *, void *);
void create_array_datatype();
#endif
