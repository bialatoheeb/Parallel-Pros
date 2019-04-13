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
  }else{
    acgrange->next = NULL;
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

void deleteComms(){
  int i = 1;
  while (tempCollection->next != NULL && i<maxLevel) {
    tempCollection = tempCollection->next;
    MPI_Comm_free(&tempCollection->prev->localcomm);
    MPI_Group_free(&tempCollection->prev->localgroup);
    free(tempCollection->prev);
    i++;
  }
  MPI_Comm_free(&tempCollection->localcomm);
  MPI_Group_free(&tempCollection->localgroup);
  free(tempCollection);
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
  timePrint = 1;
  char timeName[80] = "/home/gst2d/Final/time.txt";
  FILE *timeFile;
  int datapoints = atoi(argv[1]);
  int num;
  int i, j, k, l, colIndex = 0, mymid, timeIndex = 1, timeStops = 12 ;
  int beginflag=0,readflag=0, lhflag=0, gtreeflag=0, readtflag=0, getsizeflag=0;
  int ltreeflag=0,sendsizeflag=0,assigntargetflag=0, localcountflag=0,sumlocalcountflag=0;
  int endflag=0;
  maxminflag=largestdimflag=globalsortflag=0;
  getbucketsflag=getcountsflag=inAdjustLflag=afterAdjustLflag=Bcastflag=0;
  double startTime[timeStops], endTime[timeStops], avgTime[timeStops], dummyTime[timeStops];
  struct node headNode, *localHead, *buildLocalHead;
  struct Gnode Gtree, *currNode;
  MPI_Status mystat;
  
  if (beginflag == 1)
    printf("Begin gid%d\n", my_global_rank);
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  global_num_ranks = num_ranks;
  my_global_rank = my_rank;
  
  if (timePrint == 1){
    //printf("%d,%d,%d",datapoints, 
    
    startTime[0] = timestamp();
    
  }
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
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  myCommCollection= (struct commgroupcollection *)malloc(sizeof( struct commgroupcollection));
  MPI_Comm_group( dup_comm_world, &myCommCollection->localgroup );
  MPI_Comm_create( dup_comm_world, myCommCollection->localgroup, &myCommCollection->localcomm );
  myCommCollection->this_num_ranks = global_num_ranks;
  myCommCollection->ranks = (int *)malloc(myCommCollection->this_num_ranks*sizeof(int));
  for (i=0;i<myCommCollection->this_num_ranks;i++)
    myCommCollection->ranks[i] = i;
  
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

  tempCollection = myCommCollection;
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];    
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      timeFile = fopen(timeName, "a");
      fprintf(timeFile,"%f", avgTime[timeIndex-1]);      
      //fclose(timeFile);
    }
  }

  struct data_struct* array  = (struct data_struct *) malloc(num * sizeof(struct data_struct));
  
  //=============================== 
  //READ
  //=============================== 
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  create_array_datatype();
  readFromFileAllRead(datapoints, array );
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (readflag == 1)
    printf("READPOINTS gid%d\n", my_global_rank);
  //=============================== 
  // GET LOCAL HEAD
  //=============================== 
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  localHead = buildTreeGlobal(array, num, &headNode, -1);
  //deleteLocalHeadBuild(&headNode);
  MPI_Barrier(MPI_COMM_WORLD);
  deleteComms();
  //
  array = localHead->center;
  num = localHead->num_below;
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (lhflag == 1)
    printf("GETLOCALHEAD gid%d\n", my_global_rank);
  
  //=============================== 
  //BUILD GLOBAL TREE ON RANK ZERO
  //=============================== 
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  globalTreeMaster(&Gtree, localHead);
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      printf(";%f", avgTime[timeIndex-1]);
    }
  }
  if (gtreeflag == 1)
    printf("BUILDGLOBALTREE gid%d\n", my_global_rank);
  
  
  //========================
  //
  //                                           TARGETS SECTION
  //
  //========================
  
  char fname[80] = "/home/gst2d/localstorage/public/coms7900-data/binary/bdatafile00501.bin";
  
  struct data_struct* targetArray  = (struct data_struct *) malloc(targetSize * sizeof(struct data_struct));
  int * sendSize = (int *)calloc(global_num_ranks,sizeof(int));
  int mySendSize = 0;
  int totalSendSize = 0;
  struct data_struct* sendArray;//  =
  struct data_struct* allSendArrays[global_num_ranks];
  if (timePrint == 1){
      startTime[timeIndex] = timestamp();    
    }
  if (my_global_rank == 0){
    
    readFromFile(fname, targetSize, targetArray );
    
    if (readtflag == 1)
      printf("READTARGETS gid%d\n", my_global_rank);
  }
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    
    if (my_global_rank == 0){
      avgTime[timeIndex-1] = dummyTime[timeIndex-1];
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }

  //========================
  //   GET NUM OF TARGETS FOR EACH RANK
  //========================
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  if (my_global_rank == 0){
    getSendSize1(&Gtree, 0.1, targetArray, targetSize, sendSize);
    for (i=0;i<global_num_ranks;i++){
      totalSendSize += sendSize[i];
    }   
  }  
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    
    if (my_global_rank == 0){
      avgTime[timeIndex-1] = dummyTime[timeIndex-1];
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (getsizeflag == 1)
    printf("GETSIZE gid%d\n", my_global_rank);
  
  //========================
  //   LOCAL TREE BUILD 
  //========================
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  //reduceLocalHead()
  buildTree(array, num, localHead, -1);
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (ltreeflag == 1)
    printf("LOCALTREEBUILD gid%d\n", my_global_rank);
 
  
  //========================
  //   RANK 0 SENDS NUM OF TARGETS TO OTHER RANKS
  //========================
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  MPI_Scatter(sendSize, 1, MPI_INT, &mySendSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (sendsizeflag == 1)
    printf("SENDSIZE gid%d\n", my_global_rank);
  MPI_Barrier(MPI_COMM_WORLD);
  //========================
  //   RANK 0 ASSIGNS TARGETS TO OTHER RANKS
  //========================
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
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
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (assigntargetflag == 1)
    printf("ASSIGNTARGET gid%d\n", my_global_rank);
  
  double radius[3] = {0.01, 0.05, 0.1};
  int localCount = 0, totalCount, tgi = 0, sendi = 0, radi = 0;
  long int *radiCounts = (long int *) malloc(4*mySendSize*sizeof(long int));
  long int *allRadiCounts;
  int *tsendSize;
  
  //========================
  //   EACH RANK COUNT LOCALLY
  //========================
  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
  }
  for (sendi =0; sendi<mySendSize; sendi++){
    radi = sendi*4;
    radiCounts[radi++] = sendArray[sendi].num;
    for (i=0;i<3;i++){
	
	radiCounts[radi++] = localSearch(localHead, radius[i], sendArray[sendi]);
	
    }    
  }
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }

  if (localcountflag == 1)
    printf("LOCALCOUNTBegin gid%d\n", my_global_rank);

  if (timePrint == 1){
    startTime[timeIndex] = timestamp();    
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
  if (timePrint == 1){
    endTime[timeIndex++] = timestamp();
    dummyTime[timeIndex-1] = endTime[timeIndex-1] - startTime[timeIndex-1];
    MPI_Reduce( &dummyTime[timeIndex-1], &avgTime[timeIndex-1], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timefile = fopen(timeName, "a");
      fprintf(timeFile,";%f", avgTime[timeIndex-1]);
      //fclose(timeFile);
    }
  }
  if (sumlocalcountflag == 1)
    printf("SUMCOUNTS gid%d\n", my_global_rank);
  if (timePrint == 1){
    endTime[0] = timestamp();
    dummyTime[0] = endTime[0] - startTime[0];
    MPI_Reduce( &dummyTime[0], &avgTime[0], 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD ); 
    if (my_global_rank == 0){
      //timeFile = fopen(timeName, "a");
      fprintf(timeFile,";%f\n", avgTime[0]);
      fclose(timeFile);
    }
    
  }
  
  if (my_global_rank == -1){
    
    
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
