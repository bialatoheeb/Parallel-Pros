#include "headerFuncs.h"
int localSearch(struct node * anode, double radius, struct data_struct target){
  int i,j,k, count = 0;
  long double targetDir[3], targetPoint[3], targetMagnitude = 0, testRadius=0;//, dist = 0;
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
    //if (my_global_rank == 0){
    //  printf("Center: (%15Lf,%15Lf,%15Lf);\n",
    //	 anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2]);
    //  printf("MaxRadius: %15Lf\n",anode->maxRadius);
    //  printf("Target: (%15Lf,%15Lf,%15Lf);\n",
    //	     target.xyz[0],target.xyz[1],target.xyz[2]);
    //  printf("TestRadius: %15Lf\n\n",testRadius);
    //}
  
    if (targetMagnitude < anode->maxRadius || testRadius < radius){ //THIS MEAND NODE MAX RADIUS IS IN RADIUS OF TARGET
      //if (my_global_rank == 0)
      //	printf("LEfT\n");
      count += localSearch(anode->left, radius, target);
      //if (my_global_rank == 0)
      //	printf("Right\n");
      count += localSearch(anode->right,radius, target);
      return count;
    }else{
      return 0;
    }
    
  }else{
    //leaf - target < radius
    for (i=0;i<3;i++){
      targetMagnitude += pow(target.xyz[i]-anode->center->xyz[i],2);
    }
    
    targetMagnitude = sqrt(targetMagnitude);
    //if (my_global_rank == 0){
    //  printf("LEAF Center: (%15Lf,%15Lf,%15Lf);\n",
    //	 anode->center->xyz[0],anode->center->xyz[1],anode->center->xyz[2]);
    //}
    if (targetMagnitude < radius){
      //printf("rank: %u\n", my_global_rank);
      return 1;
    }else
      return 0;
  }
 }
