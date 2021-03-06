#include "headerFuncs.h"
void createCommCollections(int mymin, int mymax, struct commgroupcollection *acgrange){
  int i, mymid, aval = mymin;
  acgrange->this_num_ranks = mymax-mymin+1;
  acgrange->ranks = (int *)malloc(acgrange->this_num_ranks*sizeof(int));
  for (i=0;i<acgrange->this_num_ranks;i++){//al=mymin;aval<=mymax;aval++){
    acgrange->ranks[i] = aval;
    aval++;
  }
  if (acgrange->this_num_ranks > 1){    
    numRanges += 1;
    acgrange->next = (struct commgroupcollection *)malloc(sizeof( struct commgroupcollection));
    acgrange->next->prev = acgrange;
    mymid = mymin + (int)acgrange->this_num_ranks/2;
    if (my_global_rank < mymid){
      createCommCollections(mymin, mymid-1, acgrange->next);
      
    }else{
      createCommCollections(mymid, mymax, acgrange->next);
    }
  }
  

}

void createCommLevel(struct commgroupcollection * cgc, int assigned){
  int allAssigned,i,j;
  int * whosAssigned = (int *)malloc(global_num_ranks*sizeof(int));
  MPI_Allreduce(&assigned, &allAssigned, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); 
  if (allAssigned < global_num_ranks){
    MPI_Allgather(&assigned, 1, MPI_INT, whosAssigned, 1, MPI_INT, MPI_COMM_WORLD);
    if (assigned == 0){
      cgc->this_num_ranks = allAssigned;
      cgc->ranks = (int *)malloc(allAssigned*sizeof(int));
      j=0;
      for (i=0;i<allAssigned;i++){
	if (whosAssigned[i] == 0){
	  cgc->ranks[j] = i;
	  j++;
	}
      }
    }
    MPI_Group_incl( world_group, cgc->this_num_ranks, cgc->ranks, &cgc->localgroup );
    MPI_Comm_create( dup_comm_world, cgc->localgroup, &cgc->localcomm );
    if (assigned == 0){
      free(cgc->ranks);
      MPI_Comm_free(&cgc->localcomm);
      MPI_Group_free(&cgc->localgroup);
    }
  }else{
    MPI_Group_incl( world_group, cgc->this_num_ranks, cgc->ranks, &cgc->localgroup );
    MPI_Comm_create( dup_comm_world, cgc->localgroup, &cgc->localcomm );
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2){
    printf("NEED 1 args: number of data points\n");
    exit(0);
  }
  int targetSize = 20;
  if (argc > 2){
    targetSize = atoi(argv[2]);
  }
  timePrint = 0;
  int datapoints = atoi(argv[1]);
  int num;
  int i, j, k, l, colIndex = 0, mymid ;
  double startTime[6], endTime[6], avgTime[6];
  struct node headNode, *localHead, *buildLocalHead;
  struct Gnode Gtree, *currNode;
  MPI_Status mystat;
  startTime[0] = timestamp();
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  global_num_ranks = num_ranks;
  my_global_rank = my_rank;
  
  j = MPI_Comm_dup(MPI_COMM_WORLD, &dup_comm_world); 
  MPI_Comm_group( dup_comm_world, &world_group );
  MPI_Comm_create( dup_comm_world, world_group, &MPI_LOCAL_COMM );
  if (my_rank == num_ranks-1){
    num = (int)datapoints/num_ranks + datapoints%num_ranks;
  }else{

    num = (int)datapoints/num_ranks;
  }
  
  //=============================== 
  // CREATE COMM COLLECTION
  //=============================== 
  myCommCollection= (struct commgroupcollection *)malloc(sizeof( struct commgroupcollection));
  MPI_Comm_group( dup_comm_world, &myCommCollection->localgroup );
  MPI_Comm_create( dup_comm_world, myCommCollection->localgroup, &myCommCollection->localcomm );
  myCommCollection->next= (struct commgroupcollection *)malloc(sizeof( struct commgroupcollection));
  myCommCollection->next->prev = myCommCollection;
  numRanges = 1;
  mymid = (int)global_num_ranks/2;
  if (my_global_rank < mymid){
    createCommCollections(0, mymid-1, myCommCollection->next);
  }else{
    createCommCollections(mymid, global_num_ranks-1, myCommCollection->next);
  }
  tempCollection = myCommCollection->next;
  
  MPI_Allreduce(&numRanges, &maxLevel, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  for (i=1;i<maxLevel;i++){
    if (i > numRanges){
      createCommLevel(tempCollection, 0);
    }else{
      createCommLevel(tempCollection, 1);      
      tempCollection = tempCollection->next;
    }
    
    
    MPI_Barrier(MPI_LOCAL_COMM);
    
  }


  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  
  //=============================== 
  //READ
  //=============================== 
  create_array_datatype();
  readFromFileAllRead(datapoints, array );
  localHead = buildTreeGlobal(array, num, &headNode, -1);
  array = localHead->center;
  num = localHead->num_below;
  
  //=============================== 
  //BUILD GLOBAL TREE ON RANK ZERO
  //=============================== 
  globalTreeMaster(&Gtree, localHead);
  //========================
  //
  //                                           TARGETS SECTION
  //
  //========================
  char fname[80] = "/home/tab7v/localstorage/public/coms7900-data/binary/bdatafile00501.bin";
  
  struct data_struct* targetArray  = (struct data_struct *) malloc(targetSize * sizeof(struct data_struct));
  int * sendSize = (int *)calloc(global_num_ranks,sizeof(int));
  int mySendSize = 0;
  int totalSendSize = 0;
  struct data_struct* sendArray;//  =
  struct data_struct* allSendArrays[global_num_ranks];
  if (my_global_rank == 0)
    readFromFile(fname, targetSize, targetArray );

  //========================
  //   GET NUM OF TARGETS FOR EACH RANK
  //========================
  if (my_global_rank == 0){
    getSendSize1(&Gtree, 0.1, targetArray, targetSize, sendSize);
    for (i=0;i<global_num_ranks;i++){
      totalSendSize += sendSize[i];
    }   
  }  

  //========================
  //   LOCAL TREE BUILD 
  //========================

  buildTree(array, num, localHead, -1);

  //========================
  //   RANK 0 SENDS NUM OF TARGETS TO OTHER RANKS
  //========================

  MPI_Scatter(sendSize, 1, MPI_INT, &mySendSize, 1, MPI_INT, 0, MPI_COMM_WORLD);


  //========================
  //   RANK 0 ASSIGNS TARGETS TO OTHER RANKS
  //========================
  if (my_global_rank == 0){
    for (i = 0;i<global_num_ranks;i++){
      if (sendSize[i] > 0)
	allSendArrays[i] = (struct data_struct *) malloc(sendSize[i] * sizeof(struct data_struct)); 
    }
    mySendSize = sendSize[0];    
    for (i = 1;i<global_num_ranks;i++){
      if (sendSize[i] > 0){
	getSendArray(&Gtree, 0.1, targetArray, targetSize, allSendArrays[i], sendSize[i], i);
	MPI_Send(allSendArrays[i], sendSize[i], array_type, i, 0, MPI_COMM_WORLD);
      }
    }
    if (mySendSize > 0){
      sendArray = (struct data_struct *) malloc(mySendSize * sizeof(struct data_struct)); 
      getSendArray(&Gtree, 0.1, targetArray, targetSize, sendArray, sendSize[0], 0);
    }
    
  }else{
    if (mySendSize > 0){
      sendArray = (struct data_struct *) malloc(mySendSize * sizeof(struct data_struct)); 
      MPI_Recv(sendArray, mySendSize, array_type, 0, 0, MPI_COMM_WORLD, &mystat);    
    }
  }
  double radius[3] = {0.01, 0.05, 0.1};
  int localCount = 0, totalCount, tgi = 0, sendi = 0, radi = 0;
  long int *radiCounts = (long int *) malloc(4*mySendSize*sizeof(long int));
  long int *allRadiCounts;
  int *tsendSize;
  
  //========================
  //   EACH RANK COUNT LOCALLY
  //========================
  for (sendi =0; sendi<mySendSize; sendi++){
    radi = sendi*4;
    radiCounts[radi++] = sendArray[sendi].num;
    for (i=0;i<3;i++){
	
	radiCounts[radi++] = localSearch(localHead, radius[i], sendArray[sendi]);
	
    }    
  }

  if (my_global_rank == 0){
    MPI_Status stat;
    int d,gc, nonZeroRanks = 0;
    allRadiCounts = (long int *) calloc(3*targetSize,sizeof(long int));
    tsendSize = (int *)malloc(global_num_ranks*sizeof(int));
    tsendSize[0] = sendSize[0]*4;
    //========================
    //   GET SEND TOTAL SEND SIZE
    //========================
    for (i=1;i<global_num_ranks;i++){
      tsendSize[i] = sendSize[i]*4;
      if (tsendSize[i] > 0)
	nonZeroRanks++;
    }
    
    //========================
    //   SUM COUNTS FOR RANK 0
    //========================

    for (radi=0;radi<tsendSize[my_global_rank];radi+=4){
      k = (int)(radiCounts[radi]-targetArray[0].num)*3;
      for (j=1; j<=3;j++){
	allRadiCounts[k+j-1] += radiCounts[radi+j];	
      }
    }
    free(radiCounts);
    
    //========================
    //   SUM COUNTS FOR OTHER RANKS
    //========================
    i = 1;
    while (i<nonZeroRanks){ 

      MPI_Probe(MPI_ANY_SOURCE, 123, MPI_COMM_WORLD, &stat);
      d = stat.MPI_SOURCE;
      MPI_Get_count(&stat,li_type,&gc);
      
      radiCounts = (long int *)malloc(tsendSize[d]*sizeof(long int));      
      MPI_Recv(radiCounts,tsendSize[d] , li_type, d, 123, MPI_COMM_WORLD, &mystat);
      i++;
      for (radi=0;radi<tsendSize[d];radi+=4){      	
      	k = (int)(radiCounts[radi]-targetArray[0].num)*3;
      	for (j=1; j<=3;j++){
      	  allRadiCounts[k+j-1] += radiCounts[radi+j];      	  
      	}      	
      }    
      free(radiCounts);
    }
    
  }else{
    mySendSize *=4;
    if (mySendSize > 0)
      MPI_Send(radiCounts, mySendSize, li_type, 0, 123, MPI_COMM_WORLD);
    
  }
  MPI_Barrier(MPI_COMM_WORLD);
  
  if (my_global_rank == 0){
    
    
    ////================
    ////
    //// PRINT ALL TARGETS
    ////
    ////================
    
    printf("%20s\t%10s\t%10s\t%10s\n", "TargetID", "0.01", "0.05","0.1");
    for (tgi =0; tgi<targetSize; tgi++){
      printf("%20Lu",targetArray[tgi].num);
      k = tgi*3;
      for (i=0;i<3;i++){
	printf("\t%10Lu",allRadiCounts[ k+i]);
      }
      printf("\n");      
    }
  
  }
  
   
  MPI_Finalize();
  return 0;
  
}
