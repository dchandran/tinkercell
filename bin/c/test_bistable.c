#include "ga_bistable.h"

void ode1(double time,double * u,double * du,void * data)
{
   Parameters * p = (Parameters*)data;
   double * k = (*p).params;
   double * a = (*p).alphas;
   double  r0 = k[2] - k[3]*u[0],  //inflow/outflow of s0
           r1 = k[4] - k[5]*u[1],  //inflow/outflow of s1
           r2 = k[0]*u[0]*u[0]*u[1] - k[1]*u[0]*u[0]*u[0]; //2s0 + s1 <=> 3s0
   du[0] = a[0]*(r0 + r2);
   du[1] = a[1]*(r1 - r2);
}

void ode2(double time,double * u,double * du,void * data)
{
   Parameters * p = (Parameters*)data;
   double * k = (*p).params;
   double * a = (*p).alphas;
   double  r0 = k[0]/(k[1] + pow(u[1],4)) - k[2]*u[0],  //s0
           r1 = k[3]/(k[4] + pow(u[0],4)) - k[5]*u[1];  //s1
   du[0] = a[0]*r0;
   du[1] = a[1]*r1;
}

int main()
{
   int i;
   double iv[] = { 0.8, 0.3 };
   BistablePoint bp = makeBistable(2,6,iv,100,1000,&(ode1));

   Parameters * p = bp.param;

   if (!p) return 0;

   if (bp.unstable)
   {
      printf("\nunstable steady state:   ");
      for (i=0; i < (*p).numVars; ++i)
          printf("%lf ",bp.unstable[i]);
      free(bp.unstable);
   }
   else
   {
      printf("no unstable state\n");
   }

   if (bp.stable1)
   {
      printf("\nstable steady state:   ");
      for (i=0; i < (*p).numVars; ++i)
          printf("%lf ",bp.stable1[i]);

      free(bp.stable1);
      //free(bp.stable2);
   }
   else
   {
      printf("\nno stable states\n");
   }

   printf("\nparameters: ");
   for (i=0; i < (*p).numParams; ++i)
   {
       printf("%lf ",(*p).params[i]);
   }
   printf("\n");
   printf("\nalphas: ");
   for (i=0; i < (*p).numVars; ++i)
   {
       printf("%lf ",(*p).alphas[i]);
   }
   printf("\n\n");
   deleteIndividual(p);
   return 0;
}
