#ifndef VQ_H_
#define VQ_H_

// defines
#define         SUCCESS         1
#define         FAILURE         0
#define         TRUE            1
#define         FALSE           0
#define         MAXVECTDIM      20
#define         MAXPATTERN      14000
#define         MAXCLUSTER      100

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <math.h>
#include <iostream>

// structs & classes

struct aCluster {
   double       Center[MAXVECTDIM];
   int          Member[MAXPATTERN];
   int          NumMembers;
};

struct aVector {
   double       Center[MAXVECTDIM];
   int          Size;
};

class VQsyst {
private:
   aCluster     Cluster[MAXCLUSTER];
   int          NumPatterns;
   int          NumDimensions;
   int		NumClusters;
   double       Threshold;
   void         Attach(int,int);
   int          AllocateCluster();
   void         CalcNewClustCenter(int);

   double       EucNorm(int, int);
   int          FindClosestCluster(int);
   
   FILE *OutFile;
public:
   VQsyst();
   int Assigned;
   double Pattern[MAXPATTERN][MAXVECTDIM+1];
   int InitOutput(char *fname);
   void LoadClusters();
   void RunVQ(int pat, double nrg);
   void ShowClusters();
   void SQLFlagSilent(int Winner);
   void SQLAssignPattern(double v1, double v2, double v3, double v4, double v5, double v6, double v7, double v8, double v9, double v10, double v11, double v12, int c);
   void SQLUpdateAssigned(int assigned);
   void UpdateDB();
   sqlite3 *db;
   void SQLAllocateCluster(int cid);
   void SQLUpdateCenters(int cid, double cck, int k);
   void SQLUpdateMembers(int cid, int mem);
   ~VQsyst(){fclose(OutFile);}
};

#endif /* VQ_H_ */
