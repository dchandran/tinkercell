/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ****************************************************************************/
 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "TCstructs.h"

Matrix fullBindingKinetics(int N, char ** rxnNames, char ** proteinNames)
{
   int total = (1 << N) * N - N;

   int * matrix = malloc(total * 3 * sizeof(int)); //[a + b <--> c] x N
   //double * ks = malloc(total * 2 * sizeof(double));

   int i=0,j=0,k=0,x=0;
   int N2 = (1<<N);

   for (j=0; j < N2; ++j)
   {
      x = (j<<1)+1;
      for (i=0; i<N; ++i)
      {
         if ( (1<<(1+i)) & x )  //complex x contains i
         {
            matrix[k*3 + 0] = (x - (1<<(i+1))); //complex without i
            matrix[k*3 + 1] = (1<<(i+1));  //i
            matrix[k*3 + 2] = x;    //complex
            //ks[k*2 + 0] = kon[i];
            //ks[k*2 + 1] = koff[i];
            ++k;
         }
      }
   }

   Matrix M;
   M.rows = 2*N2-1;
   M.cols = k*2;
   M.values = (double*)malloc(M.cols * M.rows * sizeof(double));
   M.rownames = (char**)malloc((M.rows+1) * sizeof(char*));
   M.rownames[M.rows] = 0;
   M.colnames = (char**)malloc((M.cols+1) * sizeof(char*));
   M.colnames[M.cols] = 0;
   //printf("rows = %i\n",M.rows);

   int * js = malloc(M.rows * sizeof(int));

   j = 0;
   int j2 = 1, j3 = 0;
   for (i=1; i <= M.rows; ++i)
   {
      M.rownames[i-1] = malloc(10*sizeof(char));
      
	  if (i==(1<<j))
      {
         sprintf(M.rownames[i-1],"%s\0",proteinNames[j]);
         ++j;
		 js[i-1] = j3;
		 if (rxnNames[j3+1])		 
			++j3;		 
      }
      else
	  {
         sprintf(M.rownames[i-1],"%s.I%i\0",proteinNames[j-1],j2);
		 js[i-1] = j3;
		 ++j2;
	  }
   }

   int a,b,c;
   for (i=0; i<(M.cols * M.rows); ++i) M.values[i] = 0;
   
   int * usedSpecies = malloc(M.rows * sizeof(int));
   for (i=0; i < M.rows; ++i) usedSpecies[i] = 0;
   
   for (i=0; i<k; ++i)
   {
      a = matrix[i*3+0]-1;
      b = matrix[i*3+1]-1;
      c = matrix[i*3+2]-1;
	  
	  usedSpecies[a] = usedSpecies[b] = usedSpecies[c] = 1;

      //printf("a=%i  b=%i  c=%i\n",a,b,c);

      valueAt(M,a,i*2+0) = -1.0;
      valueAt(M,b,i*2+0) = -1.0;
      valueAt(M,c,i*2+0) = 1.0;

      valueAt(M,a,i*2+1) = 1.0;
      valueAt(M,b,i*2+1) = 1.0;
      valueAt(M,c,i*2+1) = -1.0;

      M.colnames[i*2+0] = malloc(100*sizeof(char));  //rate bind
      sprintf(M.colnames[i*2+0],"%s*%s\0",M.rownames[a],M.rownames[b]);
      M.colnames[i*2+1] = malloc(100*sizeof(char));  //rate unbind
      sprintf(M.colnames[i*2+1],"%s.Kd*%s\0",rxnNames[ js[a] ],M.rownames[c]);
   }
   
   k = 0;
   for (i=0; i < M.rows; ++i) 
      if (usedSpecies[i])
	      ++k;

   Matrix M2;
   M2.rows = k;
   M2.cols = M.cols;
   M2.values = (double*)malloc(M2.cols * M2.rows * sizeof(double));
   M2.colnames = M.colnames;
   M2.rownames = (char**)malloc((k+1) * sizeof(char*));
   M2.rownames[M2.rows] = 0;
   for (i=0,j=0; i<M.rows; ++i)
      if (usedSpecies[i])
	  {
	      M2.rownames[j] = M.rownames[i];
		  ++j;
	  }
	  else
	  {
	      free(M.rownames[i]);
	  } 
   k = 0;
   for (i=0; i<M.rows; ++i)
   {
      if (usedSpecies[i])
	  {
	     for (j=0; j<M.cols; ++j)
		 {
		    valueAt(M2,k,j) = valueAt(M,i,j);
		 }
		 ++k;
	  }
   }
   
   free(M.rownames);
   free(M.values);
   free(matrix);
   free(js);
   free(usedSpecies);
   return M2;
}

int main()
{
   int N = 3;
   //double kon[] = {10.0, 0.5, 0.9};
   //double koff[] = { 1.0, 1.5, 1.1 };
   
   int total;
   char* proteinNames[] = { "P\0", "A\0","B\0" };

   Matrix M = fullBindingKinetics(2,"j0",proteinNames);
   int i=0,j=0;

   for (j=0; j < M.cols; ++j)
   {
      printf("%s ",M.colnames[j]);
   }
   printf("\n");

   for (i=0; i < M.rows; ++i)
   {
      printf("%s ",M.rownames[i]);
      for (j=0; j < M.cols; ++j)
      {
         printf("%lf ",valueAt(M,i,j));
      }
      printf("\n");
   }
   free(M.values);
   free(M.colnames);
   free(M.rownames);
   return 0;
}
