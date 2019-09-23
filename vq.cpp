#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <sstream>
#include <math.h>
#include <iostream>
#include "Pusher.h"
#include "vq.h"



// InitOutput

int VQsyst::InitOutput(char *fname) {
if((OutFile = fopen(fname, "w")) == NULL){
    printf("Unable to open file %s for output",fname);
    return FAILURE;
    }
return SUCCESS;
}

// sqlite operations

//SQL Load Clusters
void VQsyst::LoadClusters(){
   sqlite3 *db;
   char *zErrMsg = 0;
   sqlite3_stmt *ppStmt;
   int rc;
   char buf[128];
   int rows = 0;
//   double cent;
   int i;
   int members;

fprintf(stdout, "Loading Clusters\n");

   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't load clusters: %s\n", sqlite3_errmsg(db));
      //exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }

   /* Get Number of Clusters */
   sprintf(buf, "SELECT COUNT(*) FROM clusters;");
   if( sqlite3_prepare_v2(db, buf, strlen(buf)+1, &ppStmt, NULL) != SQLITE_OK ){
	fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
   }
   if(ppStmt){
	sqlite3_step(ppStmt);
	rows=sqlite3_column_int(ppStmt,0);
	sqlite3_step(ppStmt);
	sqlite3_finalize(ppStmt);
   }

/* Get Other Data */
	sprintf(buf, "SELECT dimensions, threshold, assigned FROM meta WHERE rowid=1;");
	sqlite3_prepare_v2(db, buf, strlen(buf)+1, &ppStmt, NULL);
	sqlite3_step(ppStmt);
	NumDimensions=sqlite3_column_int(ppStmt,0);
	Threshold=sqlite3_column_double(ppStmt,1);
	Assigned=sqlite3_column_int(ppStmt,2);
	sqlite3_step(ppStmt);
	sqlite3_finalize(ppStmt);
	//sqlite3_close(db);

	sqlite3_stmt *stmt;
	for (i=0; i<rows; i++){
	           /* Get Centers */
	        for (int k=0; k<NumDimensions; k++){
	        //sqlite3_open("data.db", &db);
	        snprintf(buf,128, "SELECT cluster_center%i FROM clusters WHERE ID=?", k);
	        sqlite3_prepare_v2(db,  buf, strlen(buf)+1, &stmt, NULL);
	        sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
	        sqlite3_bind_int(stmt, 1, i);
	        sqlite3_step(stmt);
	        Cluster[i].Center[k]=sqlite3_column_double(stmt,0);
	        sqlite3_clear_bindings(stmt);
	        sqlite3_reset(stmt);
	        sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
	        sqlite3_finalize(stmt);
	        //sqlite3_close(db);
	        }



	/*for (i=0; i<rows; i++){
	sprintf(buf, "SELECT cluster_center0 FROM clusters WHERE ID=%d;",i);
	if( sqlite3_prepare_v2(db, buf, strlen(buf)+1, &ppStmt, NULL) != SQLITE_OK ){
	fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
	}
	if(ppStmt){
		sqlite3_step(ppStmt);
		cent=sqlite3_column_double(ppStmt,0);
		sqlite3_step(ppStmt);
		sqlite3_finalize(ppStmt);
		Cluster[i].Center[0]=cent;
	}
	sprintf(buf, "SELECT cluster_center1 FROM clusters WHERE ID=%d;",i);
	if( sqlite3_prepare_v2(db, buf, strlen(buf)+1, &ppStmt, NULL) != SQLITE_OK ){
	fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
	}
	if(ppStmt){
		sqlite3_step(ppStmt);
		cent=sqlite3_column_double(ppStmt,0);
		sqlite3_step(ppStmt);
		sqlite3_finalize(ppStmt);
		Cluster[i].Center[1]=cent;
	}*/
	   /* Get Members */
	sprintf(buf, "SELECT members FROM clusters WHERE ID=%d;",i);
	if( sqlite3_prepare_v2(db, buf, strlen(buf)+1, &ppStmt, NULL) != SQLITE_OK ){
	fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
	}
	if(ppStmt){
		sqlite3_step(ppStmt);
		members=sqlite3_column_int(ppStmt,0);
		sqlite3_step(ppStmt);
		sqlite3_finalize(ppStmt);
		Cluster[i].NumMembers=members;
	}
}
	fprintf(stdout, "Clusters Loaded\n");
	 sqlite3_close(db);
}

// Allocate cluster in database
void VQsyst::SQLAllocateCluster(int cid){

   //sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char buf[128];

   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't allocate cluster: %s\n", sqlite3_errmsg(db));
      //exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }

   /* Create merged SQL statement */
   sprintf(buf, "INSERT INTO clusters (ID) VALUES (%d);", cid);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, buf, NULL, NULL, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error on cluster allocation: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Cluster Added\n");
   }
    sqlite3_close(db); 
}

void VQsyst::SQLUpdateCenters(int cid, double cck, int k){
   char *zErrMsg = 0;
   sqlite3_stmt *stmt;
   sqlite3_open("data.db", &db);
   char buf[128];
    snprintf(buf,128, "UPDATE clusters SET cluster_center%i = @cck WHERE ID=@cid", k);
    sqlite3_prepare_v2(db,  buf, strlen(buf)+1, &stmt, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
	sqlite3_bind_int(stmt, 1, k);
	sqlite3_bind_double(stmt, 1, cck);
	sqlite3_bind_int(stmt, 2, cid);
	sqlite3_step(stmt);
	sqlite3_clear_bindings(stmt);
	sqlite3_reset(stmt);
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

/*
// SQL Update Centers
void VQsyst::SQLUpdateCenters(int cid, double cc0, double cc1){
   char *zErrMsg = 0;
   sqlite3_stmt *stmt;
   //int rc;
   char buf[128];
    sqlite3_open("data.db", &db);   
sprintf(buf, "UPDATE clusters SET cluster_center0 = @cc0, cluster_center1 = @cc1 WHERE ID=@cid;");
sqlite3_prepare_v2(db,  buf, strlen(buf)+1, &stmt, NULL);
sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
sqlite3_bind_double(stmt, 1, cc0);
sqlite3_bind_double(stmt, 2, cc1);
sqlite3_bind_int(stmt, 3, cid);
sqlite3_step(stmt);
sqlite3_clear_bindings(stmt);
sqlite3_reset(stmt);
sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
sqlite3_finalize(stmt);
sqlite3_close(db);         
}*/

// SQL Update member count
void VQsyst::SQLUpdateMembers(int cid, int mem){

   char *zErrMsg = 0;
   int rc;
   char buf[128];

   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't update members: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }

   /* Create merged SQL statement */
   sprintf(buf, "UPDATE clusters SET members = %d WHERE ID=%d;", mem, cid);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, buf, NULL, NULL, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error on member count update: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Cluster member count updated\n");
   }
   sqlite3_close(db); 
}

// Flag cluster as silence
void VQsyst::SQLFlagSilent(int Winner){
   char *zErrMsg = 0;
   sqlite3_stmt *stmt;
   int rc;
   char buf[128];
rc = sqlite3_open("data.db", &db);    
sprintf(buf, "UPDATE clusters SET flag = @flg WHERE ID=@Winner");
sqlite3_prepare_v2(db,  buf, strlen(buf)+1, &stmt, NULL);
sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
sqlite3_bind_text(stmt, 1, "s", 1, SQLITE_TRANSIENT);
sqlite3_bind_int(stmt, 2, Winner);
sqlite3_step(stmt);
sqlite3_clear_bindings(stmt);
sqlite3_reset(stmt);
sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
sqlite3_finalize(stmt);
 sqlite3_close(db);  
}


// SQL Update Assigned Patterns count
void VQsyst::SQLUpdateAssigned(int assigned){

   char *zErrMsg = 0;
   int rc;
   char buf[128];

   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't update assigned: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }

   /* Create merged SQL statement */
   sprintf(buf, "UPDATE meta SET assigned = %d WHERE rowid=1;", assigned);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, buf, NULL, NULL, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error on assigned count update: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Assigned patterns count updated succesfully\n");
   }
   sqlite3_close(db);  
}

// SQL assign pattern to cluster
void VQsyst::SQLAssignPattern(double v1, double v2, double v3, double v4, double v5, double v6, double v7, double v8, double v9, double v10, double v11, double v12, int c){

   char *zErrMsg = 0;
   sqlite3_stmt *stmt;
   int rc;
   char buf[128];

   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't assign pattern: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }
sprintf(buf, "INSERT INTO patterns VALUES(@v1,@v2,@v3,@v4,@v5,@v6,@v7,@v8,@v9,@v10,@v11,@v12,@c);");
sqlite3_prepare_v2(db,  buf, strlen(buf)+1, &stmt, NULL);
sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
sqlite3_bind_double(stmt, 1, v1);
sqlite3_bind_double(stmt, 2, v2);
sqlite3_bind_double(stmt, 3, v3);
sqlite3_bind_double(stmt, 4, v4);
sqlite3_bind_double(stmt, 5, v5);
sqlite3_bind_double(stmt, 6, v6);
sqlite3_bind_double(stmt, 7, v7);
sqlite3_bind_double(stmt, 8, v8);
sqlite3_bind_double(stmt, 9, v9);
sqlite3_bind_double(stmt, 9, v9);
sqlite3_bind_double(stmt, 10, v10);
sqlite3_bind_double(stmt, 11, v11);
sqlite3_bind_double(stmt, 12, v12);
sqlite3_bind_int(stmt, 13, c);
sqlite3_step(stmt);
sqlite3_clear_bindings(stmt);
sqlite3_reset(stmt);
sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
sqlite3_finalize(stmt);
sqlite3_close(db);    
}

// VQ methods

// Constructor

VQsyst::VQsyst(){
   Assigned = 0;
   NumDimensions = 0;
   NumPatterns = 0;
   Threshold = 0;

   for (int i=0; i < MAXCLUSTER; i++) {
	   this->Cluster[i].NumMembers = 0;
   }
   //sqlite3 *db;
   //char *zErrMsg = 0;
   sqlite3_stmt *ppStmt;
   int rc;
   char buf[128];
   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't get number of clusters: %s\n", sqlite3_errmsg(db));
      //exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }

   /* Get Number of Clusters */
   sprintf(buf, "SELECT COUNT(*) FROM clusters;");
   if( sqlite3_prepare_v2(db, buf, strlen(buf)+1, &ppStmt, NULL) != SQLITE_OK ){
	fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
   }
   if(ppStmt){
	sqlite3_step(ppStmt);
	NumClusters=sqlite3_column_int(ppStmt,0);
	sqlite3_step(ppStmt);
	sqlite3_finalize(ppStmt);
   }
   sqlite3_close(db);         
}

// RunVQ

void VQsyst::RunVQ(int pat, double nrg){
  int Winner;
  double dist;
   printf("\n\nPATTERN %d:\n",pat);
   fprintf(OutFile,"\n\nPATTERN %d:\n",pat);
   Winner = FindClosestCluster(pat);
   if (Winner == -1) {
      Winner=AllocateCluster();
      SQLAllocateCluster(Winner);
      if (nrg<60.0){
      	 SQLFlagSilent(Winner);
      }
      //printf(" so allocate a new cluster %d\n",Winner);
      }
    else {
      printf("The closest cluster is: %d\n",Winner);
      dist= EucNorm(pat,Winner);
      dist= sqrt(dist);
      if (dist>Threshold) {
         printf("distance %f > %f", dist,Threshold);
         printf("\nTherefore cluster %d failed the distance test.\n",Winner);
         Winner=AllocateCluster();
	 SQLAllocateCluster(Winner);
	 if (nrg<60.0){
      	 	SQLFlagSilent(Winner);
	 }
         printf("so create NEW cluster number:%d\n",Winner);
         }
       else {
         printf("Distance %f < %f", dist,Threshold);
         printf("\nTherefore cluster %d passed the distance test.\n",Winner);
         }
      }
   //printf("pattern %d assigned to cluster %d\n",pat,Winner);
   fprintf(OutFile,"pattern %d assigned to cluster %d\n",pat,Winner);
   SQLAssignPattern(Pattern[pat][0],Pattern[pat][1],Pattern[pat][2],Pattern[pat][3],Pattern[pat][4],Pattern[pat][5],Pattern[pat][6],Pattern[pat][7],Pattern[pat][8],Pattern[pat][9],Pattern[pat][10],Pattern[pat][11],Winner);
   Assigned = Assigned+1;
   Attach(Winner,pat);
   CalcNewClustCenter(Winner);
   //ShowClusters();
}

// Designate the next free cluster as active

int VQsyst::AllocateCluster(){
   int n;
n=NumClusters;
NumClusters++;
return n;
}

// Euclidian norm between a pattern, p, and a cluster center,c-1 the first K vectors to do this

double VQsyst::EucNorm(int p, int c){
double dist;
int i;
dist=0;
for (i=0; i<NumDimensions ;i++){
   dist += (Cluster[c].Center[i]-Pattern[p][i])*(Cluster[c].Center[i]-Pattern[p][i]);
   }
return dist;
}

// Returns the index of the cluster to which the pattern, pat, has the closest Euclidean distance. 

int VQsyst::FindClosestCluster(int pat){
   int i, ClustID;
   double MinDist, d;
MinDist =9.9e+99;
ClustID=-1;
for (i=0; i<NumClusters; i++) {
   d=EucNorm(pat,i);
   if (d<MinDist) {
      MinDist=d;
      ClustID=i;
      }
   }
if (ClustID<0) {
   printf("No Clusters exist ");
   fprintf(OutFile,"No Clusters exist ");
   }
return ClustID;
}

// Adds the pattern (whose index is p) to the cluster center whose index is p.

void VQsyst::Attach(int c,int p){
int MemberIndex;

   MemberIndex=Cluster[c].NumMembers;
   Cluster[c].Member[MemberIndex]=p;
   Cluster[c].NumMembers++;
   SQLUpdateMembers(c, Cluster[c].NumMembers);
}

// Calculate a new cluster center for the specified cluster,c

void VQsyst::CalcNewClustCenter(int c){
   int VectID,j,k;
   double tmp[MAXVECTDIM];

   for (j=0; j<NumDimensions; j++) {
      tmp[j]=0.0;
      }
   for (j=0; j<Cluster[c].NumMembers; j++) {
      VectID=Cluster[c].Member[j];
      for (k=0; k<NumDimensions; k++) {
         tmp[k] += Pattern[VectID][k];
         }
      }
   for (k=0; k<NumDimensions; k++) {
      tmp[k]=tmp[k]/Cluster[c].NumMembers;
      Cluster[c].Center[k]=tmp[k];
      }
}

//   Display the cluster centers for each of the allocated clusters

void VQsyst::ShowClusters(){
   int cl,i;
printf("\nThe new cluster centers are:");
fprintf(OutFile,"\nThe new cluster centers are:");
for (cl=0; cl<NumClusters; cl++) {
   printf("\nCLUSTER %d ==>[%f,%f]", cl,Cluster[cl].Center[0],Cluster[cl].Center[1]);
   fprintf(OutFile,"\nCLUSTER %d ==>[%f,%f]", cl,Cluster[cl].Center[0],Cluster[cl].Center[1]);
   }
printf("\nCLUSTER Membership");
fprintf(OutFile,"\nCLUSTER Membership");
for (cl=0; cl<NumClusters; cl++) {
   printf("\n  Cluster %d ==>{",cl);
   fprintf(OutFile,"\n  Cluster %d ==>{",cl);
   for (i=0; i<Cluster[cl].NumMembers; i++) {
      printf("%d ",Cluster[cl].Member[i]);
      fprintf(OutFile,"%d ",Cluster[cl].Member[i]);
      }
   printf("}\n");
   fprintf(OutFile,"}\n");
   }

}

void VQsyst::UpdateDB(){
	sleep(1); // wait for pushing to stop
	int cl;
	int rc;
   /* Open database */
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't update database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }
fprintf(stdout, "Updating cluster centers in database\n");
for (cl=0; cl<NumClusters; cl++) {
        for (int k=0; k<NumDimensions; k++){
                SQLUpdateCenters(cl, Cluster[cl].Center[k], k);
        }
   }

SQLUpdateAssigned(Assigned);
fprintf(stdout, "Database updated succesfully.\nClosing database...\n");
sqlite3_close(db);
}
