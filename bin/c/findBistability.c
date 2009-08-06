/****************************************************************************
 
 This file goes hand-in-hand with the code in runbistability.c
 runbistability.c generates the ode file. This file assumes the existence of the ode file
 and performs the bistabilty analysis.
 
 ****************************************************************************/
 
#include "ga_bistable.h"
#include "ode.c"  //automatically generated from odeFunction.c

void ode(double time,double * u,double * du,void * data)
{
   Parameters * p = (Parameters*)data;
   assignParameters((*p).params);
   TCodeFunc( time, u, du, 0 );
   int i;
   for (i=0; i < (*p).numVars; ++i)
	du[i] *= (*p).alphas[i];
}


int main()
{
   FILE * out = fopen("temp.out","w");
   int i;
   TCinitialize();
   BistablePoint bp = makeBistable(TCvars,TCparams,TCinit,100,1000,&(ode));
   Parameters * p = bp.param;

   if (!p) return 0;
   fprintf(out,"\nparameters: ");
   printf("\nparameters: ");
   for (i=0; i < TCparams; ++i)
   {
       fprintf(out, "%s = %lf\t",TCparamnames[i], (*p).params[i]);
       printf("%s = %lf\t",TCparamnames[i], (*p).params[i]);
   }
   fprintf(out,"\nalphas: ");
   printf("\nalphas: ");
   for (i=0; i < TCvars; ++i)
   {
       fprintf(out,"%s = %lf\t",TCvarnames[i], (*p).alphas[i]);
       printf("%s = %lf\t",TCvarnames[i], (*p).alphas[i]);
   }
   
   if (bp.unstable)
   {
      fprintf(out,"\nunstable steady state:   ");
      printf("\nunstable steady state:   ");
      for (i=0; i < (*p).numVars; ++i)
	  {
	      fprintf(out,"%s = %lf\t",TCvarnames[i], bp.unstable[i]);
          printf("%s = %lf\t",TCvarnames[i], bp.unstable[i]);
	  }
      free(bp.unstable);
   }

   if (bp.stable1)
   {
      fprintf(out,"\nstable steady state:   ");
      printf("\nstable steady state:   ");
      for (i=0; i < (*p).numVars; ++i)
	  {
	      fprintf(out,"%s = %lf\t",TCvarnames[i],bp.stable1[i]);
          printf("%s = %lf\t",TCvarnames[i],bp.stable1[i]);
	  }

      if (bp.stable2 && (bp.stable2 != bp.stable1)) free(bp.stable2);
	  free(bp.stable1);
   }
   fprintf(out,"\n");
   printf("\n");
   deleteIndividual(p);
   fclose(out);
   return 0;
}
