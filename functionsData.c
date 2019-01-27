#include "headerFuncs.h"

int compare_datastruct(const void* s1, const void* s2, int index){
  struct data_struct  *p1 = (struct data_struct *) s1;
  struct data_struct  *p2 = (struct data_struct *) s2;
  return p1->xyz[index] > p2->xyz[index];
}

int compare_x(const void* s1, const void* s2){
  return compare_datastruct(s1, s2, 0);
}

int compare_y(const void* s1, const void* s2){
  return compare_datastruct(s1, s2, 1);
}

int compare_z(const void* s1, const void* s2){
  return compare_datastruct(s1, s2, 2);
}

int compare_longdouble(const void* s1, const void* s2){
  long double *p1 = (long double *) s1;
  long double  *p2 = (long double *) s2;
  return *p1 > *p2;
}
