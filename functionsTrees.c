#include "headerFuncs.h"


void compareFunc(struct data_struct* array, struct data_struct tarray, int num){
  int i,j,k, r1=0, r2=0, r3=0, tr;
  float dist = 0.0;
  for (i=0;i<num;i++){
    dist = 0.0;
    for (j=0;j<3;j++){
      dist += (tarray.xyz[j]-array[i].xyz[j])*(tarray.xyz[j]-array[i].xyz[j]); //, 2);
    }
  
    dist = sqrt(dist);
    if (dist < 0.01)
      r1+=1;
    if (dist < 0.05)
      r2+= 1;
    if (dist < 0.1)
      r3 += 1;
      
  }
  if (my_global_rank == 0)
    printf("%20Lu",tarray.num);
  MPI_Reduce(&r1, &tr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (my_global_rank == 0)
    printf("\t%10Lu", tr);
  MPI_Reduce(&r2, &tr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (my_global_rank == 0)
    printf("\t%10Lu", tr);
  MPI_Reduce(&r3, &tr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (my_global_rank == 0)
    printf("\t%10Lu\n", tr);
}


void compareTargetsLocalTree(struct node *localTree, struct data_struct *tarray, int numOfTargets){
  int i,j,k, r1=0, r2=0, r3=0, tr, count;
  for (k=0;k<numOfTargets;k++){
    count = 0;
    count = localSearch(localTree, 0.000001, tarray[k]);
    if (count > 0){
	r3 += 1;	
    }
    
  }
  MPI_Reduce(&r3, &tr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (my_global_rank == 0)
    printf("numOfTargets localTree\t%10Lu\n", tr);
  
}

void compareTargets(struct data_struct *array, struct data_struct *tarray, int numOfTargets, int numOfPoints){
  int i,j,k, r1=0, r2=0, r3=0, tr;
  float dist = 0.0;
  for (k=0;k<numOfTargets;k++){
    for (i=0;i<numOfPoints;i++){
      dist = 0.0;
      for (j=0;j<3;j++){
	dist += (tarray[k].xyz[j]-array[i].xyz[j])*(tarray[k].xyz[j]-array[i].xyz[j]); //, 2);
      }
  
      dist = sqrt(dist);
      if (dist < 0.000001){
	r3 += 1;
	break;
      }
    }
  }
  MPI_Reduce(&r3, &tr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (my_global_rank == 0)
    printf("numOfTargets\t%10Lu\n", tr);
  
}

struct Gnode * buildEmptyGtree(struct Gnode *agnode, int ranks_below, int right_side){
  struct Gnode *tnode;
  if (ranks_below > 1){
    agnode->left = (struct Gnode *)malloc(sizeof(struct Gnode));
    agnode->left->parent = agnode;
    agnode->left->num_below = (int)(ranks_below/2);
    agnode->left->assigned = -1;// = NULL;
    buildEmptyGtree(agnode->left, (int)(ranks_below/2), 0);
    agnode->right = (struct Gnode *)malloc(sizeof(struct Gnode));
    agnode->right->parent = agnode;
    agnode->right->assigned = (int)-1;//  = NULL;
    agnode->right->num_below = (int)(ranks_below/2 + ranks_below%2);
    tnode = buildEmptyGtree(agnode->right, (int)(ranks_below/2 + ranks_below%2), 1);
    return tnode;
  }else{    
    if (right_side == 1){
      return agnode;
    }
  }
}

void getGNode(void *vnode){
  struct Gnode *anode = (struct Gnode *)vnode;
  int i;
  float radToMax=0, radToMin=0;
  anode->center = (struct data_struct *)malloc(sizeof(struct data_struct));
  anode->assigned = 0;
  anode->maxRadius = 0;
  for (i=0;i<3;i++){
    anode->center->xyz[i] = (anode->max[i] + anode->min[i])/2;
    radToMax += (anode->center->xyz[i]-anode->max[i])*(anode->center->xyz[i]-anode->max[i]);
    radToMin += (anode->center->xyz[i]-anode->min[i])*(anode->center->xyz[i]-anode->min[i]);
  }
  
  radToMax = sqrt(radToMax);
  radToMin = sqrt(radToMin);
  if (radToMax > radToMin)
    anode->maxRadius = radToMax;
  else
    anode->maxRadius = radToMin;
  
  
  

}

void printGNode(void *vnode){
  struct Gnode *anode = (struct Gnode *)vnode;
  char fname[20];
  sprintf(fname,"/home/tab7v/COMS7900/nodesG%03u.txt", my_global_rank);
  FILE *myfile = fopen(fname, "a");
  fprintf(myfile,"GnodeLRank: %03u;\nMax: (%15Lf,%15Lf,%15Lf);\nMin: (%15Lf,%15Lf,%15Lf);\nCenter: (%15Lf,%15Lf,%15Lf);\nmaxRadius: %15Lf;\nnum_below: %15u\n========\n",
	  my_global_rank,
	  anode->max[0],anode->max[1],anode->max[2],
	  anode->min[0],anode->min[1],anode->min[2],
	  anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2],
	  anode->maxRadius,
	  anode->num_below);
  fclose(myfile);
}
void printGTree(void *vnode){
  struct Gnode *anode = (struct Gnode *)vnode;
  char fname[20];
  sprintf(fname,"/home/tab7v/COMS7900/gtree.txt", my_global_rank);
  FILE *myfile = fopen(fname, "a");
  fprintf(myfile,"LRank: %03u;\nMax: (%15Lf,%15Lf,%15Lf);\nMin: (%15Lf,%15Lf,%15Lf);\nCenter: (%15Lf,%15Lf,%15Lf);\nmaxRadius: %15Lf;\nnum_below: %15u\n========\n",
	  my_global_rank,
	  anode->max[0],anode->max[1],anode->max[2],
	  anode->min[0],anode->min[1],anode->min[2],
	  anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2],
	  anode->maxRadius,
	  anode->num_below);
  fclose(myfile);
}

void getSendSize(struct Gnode * headnode, float radius, struct data_struct *targets, int numOfTargets, int *sendSize){
  struct Gnode *anode;
  struct data_struct *target;
  int i,j,k, targeti, curr_rank = 0;
  float targetDir[3], targetPoint[3], targetMagnitude = 0, testRadius=0;//, dist = 0;
  //SEND SIZE WAS MADE WITH CALLOC
  for (targeti=0; targeti < numOfTargets; targeti++){
    anode = headnode;
    initAssigned(anode);
    curr_rank = 0;
    target = &targets[targeti];
    while (curr_rank < global_num_ranks){
      while (anode->assigned == 1){
	if (anode != headnode){
	  anode=anode->parent;
	}else{
	  
	  curr_rank = global_num_ranks;
	  break;
	}
      }
      targetMagnitude = testRadius = 0;
      if (anode->num_below > 1){

	for (i=0;i<3;i++){
	  targetDir[i] = (target->xyz[i]-anode->center->xyz[i]);
	  targetMagnitude += targetDir[i]*targetDir[i];
	}
  
	targetMagnitude = sqrt(targetMagnitude);
	for (i=0;i<3;i++){
	  targetDir[i] = targetDir[i]/targetMagnitude;
	  targetDir[i] *= anode->maxRadius;
	  targetPoint[i] = anode->center->xyz[i] + targetDir[i];
	  testRadius += pow(target->xyz[i] - targetPoint[i],2);
	
    
	}
	testRadius = sqrt(testRadius);
 
	if (targetMagnitude < anode->maxRadius || testRadius < radius){ //THIS MEAND NODE MAX RADIUS IS IN RADIUS OF TARGET
	  anode->assigned+=1; 
	  if (anode->assigned == 0)
	    anode = anode->left;
	  else if (anode->assigned == 1)
	    anode = anode->right;
	
	}else{
	  curr_rank += anode->num_below;
	  anode->assigned = 1;
	  if (anode != headnode)
	    anode=anode->parent;
	  else
	    curr_rank = global_num_ranks;
	  
	}
    
      }else{
	curr_rank += 1;
	for (i=0;i<3;i++){
	  targetDir[i] = (target->xyz[i]-anode->center->xyz[i]);
	  targetMagnitude += targetDir[i]*targetDir[i];
	}
  
	targetMagnitude = sqrt(targetMagnitude);
	for (i=0;i<3;i++){
	  targetDir[i] = targetDir[i]/targetMagnitude;
	  targetDir[i] *= anode->maxRadius;
	  targetPoint[i] = anode->center->xyz[i] + targetDir[i];
	  testRadius += pow(target->xyz[i] - targetPoint[i],2);
	
    
	}
	testRadius = sqrt(testRadius);
	if (targetMagnitude < anode->maxRadius || testRadius < radius){
	  sendSize[anode->this_rank]+= 1;
	}
	
	anode=anode->parent;
      }
    }
  }
}

void getSendSize1(struct Gnode * headnode, float radius, struct data_struct *targets, int numOfTargets, int *sendSize){
  struct data_struct *target;
  int i,j,k, targeti, curr_rank = 0;
  float targetDir[3], targetPoint[3], targetMagnitude = 0, testRadius=0;//, dist = 0;
  int * aSize = (int *)calloc(global_num_ranks,sizeof(int));
  //SEND SIZE WAS MADE WITH CALLOC
  for (targeti=0; targeti < numOfTargets; targeti++){
    for (i=0;i<global_num_ranks;i++)
      aSize[i] = 0;
    target = &targets[targeti];
    getSendSize1Target(headnode, radius, *target, aSize);
    for (i=0;i<global_num_ranks;i++){
      sendSize[i] += aSize[i];
    }
  }
}

void getSendSize1Target(struct Gnode * anode, float radius, struct data_struct target, int *sendSize){
  int i,j,k, count = 0;
  float targetDir[3], targetPoint[3], targetMagnitude = 0, testRadius=0;//, dist = 0;
  if (anode->num_below > 1){
    for (i=0;i<3;i++){
      targetDir[i] = (target.xyz[i]-anode->center->xyz[i]);
      targetMagnitude += targetDir[i]*targetDir[i];
    }
  
    targetMagnitude = sqrt(targetMagnitude);
    for (i=0;i<3;i++){
      targetDir[i] = targetDir[i]/targetMagnitude;
      targetDir[i] *= anode->maxRadius;
      targetPoint[i] = anode->center->xyz[i] + targetDir[i];
      testRadius += pow(target.xyz[i] - targetPoint[i],2);
    
    
    }
    testRadius = sqrt(testRadius);
    if (targetMagnitude < anode->maxRadius || testRadius < radius){ //THIS MEAND NODE MAX RADIUS IS IN RADIUS OF TARGET
      getSendSize1Target(anode->left, radius, target, sendSize);
      getSendSize1Target(anode->right, radius, target, sendSize);
      return;
    }
    
  }else{
    for (i=0;i<3;i++){
      targetDir[i] = (target.xyz[i]-anode->center->xyz[i]);
      targetMagnitude += targetDir[i]*targetDir[i];
    }
  
    targetMagnitude = sqrt(targetMagnitude);
    for (i=0;i<3;i++){
      targetDir[i] = targetDir[i]/targetMagnitude;
      targetDir[i] *= anode->maxRadius;
      targetPoint[i] = anode->center->xyz[i] + targetDir[i];
      testRadius += pow(target.xyz[i] - targetPoint[i],2);
    
    
    }
    testRadius = sqrt(testRadius);
    
    if (targetMagnitude < anode->maxRadius || testRadius < radius){ //THIS MEAND NODE MAX RADIUS IS IN RADIUS OF TARGET
      sendSize[anode->this_rank]+= 1;
    }
  }
}

int sendSizeTest(struct node * anode, float radius, struct data_struct * targets, int numOfTargets){
  int i,j,k, count = 0,targeti, curr_rank = 0;
  struct data_struct target;
  float targetDir[3], targetPoint[3], targetMagnitude = 0, testRadius=0;//, dist = 0;
  //SEND SIZE WAS MADE WITH CALLOC
  for (targeti=0; targeti < numOfTargets; targeti++){
    target = targets[targeti];
    targetMagnitude = testRadius = 0;
    for (i=0;i<3;i++){
      targetDir[i] = (target.xyz[i]-anode->center->xyz[i]);
      targetMagnitude += targetDir[i]*targetDir[i];
    }
    
    targetMagnitude = sqrt(targetMagnitude);
    for (i=0;i<3;i++){
      targetDir[i] = targetDir[i]/targetMagnitude;
      targetDir[i] *= anode->maxRadius;
      targetPoint[i] = anode->center->xyz[i] + targetDir[i];
      testRadius += pow(target.xyz[i] - targetPoint[i],2);
      
    
    }
    testRadius = sqrt(testRadius);
    if (targetMagnitude < anode->maxRadius || testRadius < radius){ //THIS MEAND NODE MAX RADIUS IS IN RADIUS OF TARGET
      
      count += 1;//localSearch(anode->left, radius, target);
    }
  }
  return count;
}



void getSendArray(struct Gnode * headnode, float radius, struct data_struct *targets, int numOfTargets, struct data_struct *sendArray, int sendSize, int sendRank){
  struct Gnode *anode;
  struct data_struct *target;
  int i,j,k, cnum, cmax, cmin, targeti, sendi = 0, curr_rank = 0;
  float targetDir[3], targetPoint[3], targetMagnitude = 0, testRadius=0;//, dist = 0;
  //SEND SIZE WAS MADE WITH CALLOC

  anode = headnode;
  cmax = global_num_ranks;
  cmin = 0;
  cnum = (int)(cmax/2);
  while (anode->num_below > 1){
    if (sendRank >= cnum){
      anode = anode->right;
      cmin = cnum;
      cnum = (int)((cmax+cmin)/2);
      
    }else{
      anode= anode->left;
      cmax = cnum;
      cnum = (int)((cmax+cmin)/2);
    }
  }
  for (targeti=0; targeti < numOfTargets; targeti++){
    targetMagnitude = testRadius = 0;
      
    target = &targets[targeti];
    for (i=0;i<3;i++){
      targetDir[i] = (target->xyz[i]-anode->center->xyz[i]);
      targetMagnitude += targetDir[i]*targetDir[i];
    }
    targetMagnitude = sqrt(targetMagnitude);
    for (i=0;i<3;i++){
      targetDir[i] = targetDir[i]/targetMagnitude;
      targetDir[i] *= anode->maxRadius;
      targetPoint[i] = anode->center->xyz[i] + targetDir[i];
      testRadius += pow(target->xyz[i] - targetPoint[i],2);
      
    
    }
    testRadius = sqrt(testRadius);
    if (targetMagnitude < anode->maxRadius || testRadius < radius){ //THIS MEAND NODE MAX RADIUS IS IN RADIUS OF TARGET
      
      sendArray[sendi++] = *target;
      
    }
  }
}

void initAssigned(struct Gnode * headnode){

  if (headnode->num_below > 1){
    headnode->assigned = -1;
    initAssigned(headnode->left);
    initAssigned(headnode->right);

  }else
    headnode->assigned = -1;

}

void globalTreeMaster(struct Gnode *Gtree, struct node *localHead){
  struct Gnode *currNode;
  MPI_Status mystat;
  int i,j,k;
  
  //=============================== 
  //BUILD GLOBAL TREE ON RANK ZERO
  //=============================== 
  int ranks_below; 
  float *allDummy = (float *) malloc(3 * num_ranks * sizeof(float));
  if (my_global_rank == 0){
    ranks_below = global_num_ranks;
    Gtree->num_below = ranks_below;
    Gtree->assigned = -1;// 
    currNode = buildEmptyGtree(Gtree, ranks_below, 0);
    j=global_num_ranks-1;
    //=============================== 
    //BUILD FROM LAST RANK LEAF TO FIRST RANK LEAF
    //=============================== 
  
    while (j>0){
      if (currNode->assigned == -1){
	if (currNode->num_below <=1){// then get localHEad from rankJ when needed unless j = 0 go to parent	  
	  MPI_Recv(allDummy,3,MPI_FLOAT,j,0,MPI_COMM_WORLD,&mystat);
	  
	  for (i=0;i<3;i++){
	    currNode->max[i] = allDummy[i];	    
	  }
	  MPI_Recv(allDummy,3,MPI_FLOAT,j,1,MPI_COMM_WORLD,&mystat);
	  for (i=0;i<3;i++){
	    currNode->min[i] = allDummy[i];	    
	  }
	  currNode->this_rank = j;
	  getGNode(currNode);
	  currNode = currNode->parent;
	  j--;
	  if (j==0){
	    if (global_num_ranks == 3){
	      for (i=0;i<3;i++){
		currNode->max[i] = currNode->right->max[i];
		currNode->min[i] = currNode->left->min[i];  
	      }
	      getGNode(currNode);
	      currNode = currNode->parent;	      
	      currNode = currNode->left;
	    }else if (global_num_ranks == 2){
	      currNode = currNode->left;
	    }
	  }
	  
	}else{
	  if (currNode->right->assigned == -1){
	    currNode = currNode->right;
	  }else if(currNode->left->assigned == -1){
	    currNode = currNode->left;
	  }else{
	    //BUILD THIS NODE and GO UP
	    for (i=0;i<3;i++){
	      currNode->max[i] = currNode->right->max[i];
	      currNode->min[i] = currNode->left->min[i];  
	    }
	    getGNode(currNode);
	    currNode = currNode->parent;
	  }
	}
      }else{
	currNode = currNode->parent;
      }
    }
    //=============================== 
    //BUILD FROM RANK 0 LEAF TO GLOBAL HEAD NODE
    //=============================== 
    while (currNode->num_below < global_num_ranks){
      if (currNode->assigned == -1){
	if (currNode->num_below <=1){// then get localHEad from rankJ when needed unless j = 0 go to parent	  
	  for (i=0;i<3;i++){
	    currNode->max[i] = localHead->max[i];
	    currNode->min[i] = localHead->min[i];
	  }
	  
	  currNode->this_rank = 0;
	  getGNode(currNode);
	  currNode = currNode->parent;
	  
	}else{
	  if (currNode->right->assigned == -1){
	    currNode = currNode->right;
	  }else if(currNode->left->assigned == -1){
	    currNode = currNode->left;
	  }else{
	    //BUILD THIS NODE and GO UP
	    for (i=0;i<3;i++){
	      currNode->max[i] = currNode->right->max[i];
	      currNode->min[i] = currNode->left->min[i];  
	    }
	    getGNode(currNode);
	    currNode = currNode->parent;
	  }
	}
      }else{
	currNode = currNode->parent;
      }
    }
    //=============================== 
    //BUILD GLOBAL HEAD
    //=============================== 
    for (i=0;i<3;i++){
      Gtree->max[i] = Gtree->right->max[i];
      Gtree->min[i] = Gtree->left->min[i];
    }
    getGNode(Gtree);
  }else{
    
    for (i=0;i<3;i++){
      allDummy[i] = localHead->max[i];	    
    }
    MPI_Send(allDummy,3,MPI_FLOAT,0,0,MPI_COMM_WORLD);
    for (i=0;i<3;i++){
      allDummy[i] = localHead->min[i];
    }
    MPI_Send(allDummy,3,MPI_FLOAT,0,1,MPI_COMM_WORLD);
	  
  }


}
  




