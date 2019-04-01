#include "headerFuncs.h"


void create_array_datatype(){

  MPI_Datatype data_type[2];
  int data_length[2];
  MPI_Aint displ[2], lower_bound, extent;

  // Describe the MPI_LONG_INT field in the struct
  displ[0] = 0;
  data_type[0] = MPI_LONG_INT;
  data_length[0] = 1;
  
  //Describe the MPI_LONG_DOUBLE in the field.
  //Obtain offset using size of MPI_LONG_INT already described
  MPI_Type_get_extent(MPI_LONG_INT, &lower_bound, &extent);
  displ[1] = data_length[0] * extent;
  data_type[1] = MPI_LONG_DOUBLE;
  data_length[1] = 3;

  // Define the new data_type of our struct and commit
  MPI_Type_create_struct(2, data_length, displ, data_type, &array_type);
  MPI_Type_commit(&array_type);
  
  MPI_Type_create_resized(MPI_LONG_INT, 0, sizeof(long int), &li_type);
  MPI_Type_commit(&li_type);

  MPI_Type_create_resized(MPI_LONG_DOUBLE, 0, sizeof(long double), &ld_type);
  MPI_Type_commit(&ld_type);
}

