#include "ga_bistable.h"
#include "opt.h"

#define valueAt(array, N, i, j) ( array[ (i)*(N) + (j) ] )

static Parameters * _PARAM = 0;
static double * _DU = 0;
static double * _U = 0;
static double * _U0 = 0;
static double MIN_EIG_DEV = 0.1;
static double SS_MIN_ERROR = 1.0e-5;
static double SS_MAX_TIME = 1000.0;
static double SS_MIN_DT = 10.0;
static double * INIT_VALUE = 0;
static double MIN_ERROR = 1.0;
static int PRINT_STEPS = 1;
static int GA_MAX_ITERATIONS = 100;
static int GA_POPULATION_SZ = 1000;

static double * UNSTABLE_PT = 0;
static double * STABLE_PT = 0;

static Parameters ** BAD_PARAMS = 0;

static void (*ODE_FNC)(double,double *,double *,void *);
void deleteGAindividual(void * individual);
void * clone(void * x);

static double distance( double * y1, double * y2, int n )
{
   double diff = 0;
   int i;
   for (i=0; i < n; ++i)
   {
       diff += (y1[i]-y2[i])*(y1[i]-y2[i]);
   }
   return diff;
}

static void normalize (double * a , int n)
{
   double sum = 0;
   int i;
   for (i=0; i < n; ++i) sum += (a[i]*a[i]);
   if (sum > 0)
   {
      sum = sqrt(sum);
      for (i=0; i < n; ++i) a[i] /= sum;
   }
}

static int isBad(Parameters * p)
{
   if (BAD_PARAMS)
   {
      int i=0;
      while (BAD_PARAMS[i])
      {
         if (   (distance( BAD_PARAMS[i]->params, p->params, p->numParams ) < 10.0)
             && (distance( BAD_PARAMS[i]->alphas, p->alphas, p->numVars   ) < 10.0) )
            return 1;
         ++i;
      }
      return 0;
   }
   return 0;
}

static void setBad(Parameters * p)
{
   int i=0, k = 0;
   Parameters ** temp;

   if (BAD_PARAMS)
   {  
      while (BAD_PARAMS[k]) ++k;
      temp = BAD_PARAMS;
      BAD_PARAMS = malloc( (k+1) * sizeof(Parameters*) );
      for (i=0; i < k; ++i)
          BAD_PARAMS[i] = temp[i];
      BAD_PARAMS[k-1] = (Parameters*)clone(p);
      BAD_PARAMS[k] = 0;
      free(temp);
   }
   else
   {
      BAD_PARAMS = malloc( 2 * sizeof(Parameters) );
      BAD_PARAMS[0] = (Parameters*)clone(p);
      BAD_PARAMS[1] = 0;
   }
}

static void deleteBadParams()
{
   if (BAD_PARAMS)
   {
       int i=0;
       while (BAD_PARAMS[i])
       {
          deleteGAindividual((void*)(BAD_PARAMS[i]));
       }
   }
}

static double FMIN(int n, double x[])
{
   int i;
   double sumsq = 0;
   
   if (_U0 && distance(x,_U0,n) < MIN_ERROR) return 1.0;

   ODE_FNC(1.0,x,_DU,(void*)_PARAM);
   
   for (i=0; i < n; ++i)
       sumsq += (_DU[i]*_DU[i]);
   return (sumsq);
}

void deleteGAindividual(void * individual)
{
   Parameters * p = (Parameters*)individual;
   if (p != NULL)
   {
      free(p->params);
      free(p->alphas);
      free(p);
   }
}

void * clone(void * x)
{
   int i;
   Parameters * p = malloc(sizeof(Parameters));
   Parameters * net = (Parameters*)x;
   
   p->numVars = (*net).numVars;
   p->numParams = (*net).numParams;
   p->params  = malloc(  p->numParams * sizeof(double) );
   p->alphas  = malloc(  p->numVars * sizeof(double) );

   for (i = 0; i < p->numVars; ++i)
   {
      p->alphas[i]  = (*net).alphas[i];
   }
   for (i = 0; i < p->numParams; ++i)
   {
      p->params[i]  = (*net).params[i];
   }
   return ((void*)p);
}

Parameters * randomNetwork(int numVars, int numParams)
{
   int i;
   
   Parameters * p = malloc(sizeof(Parameters));
   p->numParams = numParams;
   p->numVars = numVars;
   p->params  = malloc( numParams * sizeof(double) );
   p->alphas  = malloc( numVars * sizeof(double) );

   for (i = 0; i < numParams; ++i) p->params[i] = 10.0*randnum;
   for (i = 0; i < numVars; ++i) p->alphas[i] = 2.0*randnum - 1.0;
   normalize (p->alphas , p->numVars);
   return (p);
}

static double * regularSteadyState(Parameters * p, double * iv)
{
   int i;
   int N = p->numVars;
   double * ss, * alphas = malloc(N*sizeof(double));

   for (i=0; i < N; ++i)
   {
       alphas[i] = p->alphas[i];
       p->alphas[i] = 1.0;
   }

   ss = steadyState(N,iv,ODE_FNC,(void*)p,SS_MIN_ERROR,SS_MAX_TIME,SS_MIN_DT);
   
   for (i=0; i < N; ++i)
       p->alphas[i] = alphas[i];
   free(alphas);

   return ss;
}

static double * unstableSteadyState(Parameters * p, double * iv)
{
   double * ss = steadyState(p->numVars,iv,ODE_FNC,(void*)p,SS_MIN_ERROR,SS_MAX_TIME,SS_MIN_DT);
   return ss;
}

static double * findZeros(Parameters * p, double * x, double * fopt)
{
   int i;
   double * ss = 0;
   
   _DU = malloc(p->numVars*sizeof(double));
   _PARAM = p;
   
   for (i=0; i < p->numVars; ++i)
   {
       _U[i] = x[i];
   }

   if (NelderMeadSimplexMethod(p->numVars, &(FMIN), _U, 10.00, fopt, 1000.0, 1.0e-10) == success)
   {
         if ((*fopt) <= 1.0e-5)
              ss = _DU;
          else
              free(_DU);
   }
   else
      free(_DU);

   return ss;
}

double fitness(void * individual)
{
   int i, allPos, N;
   double * ss0, * ss1, * ss2, fmin, score;
   Parameters * p = (Parameters*)individual;
   
   allPos = 1;
   N = p->numVars;

   for (i=0; i < p->numVars; ++i)
   {
      if (p->alphas[i] < -MIN_EIG_DEV)
      {
          allPos = 0;
          break;
      }
   }
   if (allPos) return (0.0);

   ss0 = regularSteadyState(p,INIT_VALUE);

   if (ss0 == 0) { return (0.0); }
   /*for (i=0; i < N; ++i)
   {
       if (ss0[i] < 0.0) //negative steady state
       {
           free(ss0);
           return (0.0);
       }
   }*/

   for (i=0; i < p->numVars; ++i)  //tell nelder-mead to avoid this value
   {
       _U0[i] = ss0[i];
   }

   ss1 = findZeros(p,ss0,&fmin);

   if (ss1 != 0)   //ok, we have a zero
   {
       ss2 = unstableSteadyState(p,ss1);   //is it really a stable state
       if (ss2 == 0)   //(simu != findZero) so this is a stable state
       {
           free(ss1);
           ss1 = 0;
           //setBad(p); //stay away from untra-sensitive points!
           return 0.0;
       }
       else
         free(ss2);
   }

   if (ss1 == 0)
   {
       /*double * y = ODEsim(N,INIT_VALUE,ODE_FNC,0,200.0,0.1,(void*)p);

       if (!y) return (0.0);

       double diff = 0.0;
       for (i=0; i < N; ++i)
       {
          diff += (valueAt(y,N+1,2000,i+1)-ss0[i])*(valueAt(y,N+1,2000,i+1)-ss0[i]);
          if (valueAt(y,N+1,2000,i+1) < 0.0) //negative steady state
          {
             diff = 0.0;
             break;
          }
       }

       if (diff < MIN_ERROR)
       {
          free(ss0);
          free(y);
          return (0.0);
       }

       double * yend = malloc(N*sizeof(double));
       for (i=0; i<N; ++i)
           yend[i] = valueAt(y,N+1,2000,i+1);

       double maxdy = 0;
       for (i=0; i < N; ++i)
            if (((valueAt(y,N+1,2000,i+1)-(valueAt(y,N+1,1900,i+1)))*
                 (valueAt(y,N+1,2000,i+1)-(valueAt(y,N+1,1900,i+1)))) > maxdy)
                maxdy = ((valueAt(y,N+1,2000,i+1)-(valueAt(y,N+1,1900,i+1)))*
                         (valueAt(y,N+1,2000,i+1)-(valueAt(y,N+1,1900,i+1))));
       double score = 10.0/(10.0  + (maxdy));*/
       /*if (score > 0.3)
       {
         double * J = jacobian(N, yend, ODE_FNC, p);  //get jacobian at steady state
         if (J)
         {
            double * wr = 0, * wi = 0;        //wr = real component, wi = imaginary component
            int k = eigenvalues(J,N,&wr,&wi); //get eigenvalues of J in wr and wi
            free(J);
            int stablePt = 1;
            if (k)  //if everything is ok (CLAPACK)
            {
                //normalize(wr,N);
                double wmax = 0.0, wmin = 100000.0;
                for (j=0; j<N; ++j)
                {
                   //if (wr[j] > wmax)  wmax = wr[j];
                   //if (wr[j] < wmin)  wmin = wr[j];
                   if (wr[j] > 0.0)
                   { 
                       score = 0.0;
                       break;
                   }
                }
                free(wr);
                free(wi);
                //score *= 1.0/(1.0 + (wmax - wmin)*(wmax - wmin));
            }
         }
       }*/
       //free(yend);
       free (ss0);
       //free (y);
       score = 1.0e-5/(1.0e-5 + fmin);
       return score;
    }

    for (i=0; i < N; ++i)
    {
       if (ss1[i] < 0.0) //negative steady state
       {
           free(ss0);
           free(ss1);
           return (0.0);
       }
    }

    if (distance(ss1,ss0,N) < MIN_ERROR)
    {
        free(ss0);
        free(ss1);
        return (0.0);
    }

    if (STABLE_PT)
       free(ss0);
    else
       STABLE_PT = ss0;

    if (UNSTABLE_PT)
        free(ss1);
    else
        UNSTABLE_PT = ss1;
    return 1.0;
}

/*randomly change the values of a parameter array*/
void * mutate(void * individual)
{
   int i,j,n,m;
   Parameters * p = (Parameters*)individual;

   n = p->numParams;
   m = p->numVars;

   if (mtrand() < 0.5)
   {
      i = (int)(mtrand() * n);
      p->params[i] *= randnum;
   }
   else
   {
      j = (int)(mtrand() * m);
      p->alphas[j] *= 2.0 * randnum - 1.0;
   }
   normalize (p->alphas , m);
   return (p);
}

/*Mix two parameter arrays*/
void * crossover(void * individual1, void * individual2)
{
   int i;
   Parameters 	* net1 = (Parameters*)individual1,
				* net2 = (Parameters*)individual2,
				* net3 = (Parameters*)clone(individual1);
   
   if (mtrand() < 0.5)
   {
      for (i = 0; i < (*net3).numParams; ++i)
      {
         (*net3).params[i] = (*net1).params[i];
      }
      for (i = 0; i < (*net3).numVars; ++i)
      {
         (*net3).alphas[i] = (*net2).alphas[i];
      }
   }
   else
   {
      for (i = 0; i < (*net3).numVars; ++i)
      {
         (*net3).alphas[i] = 0.5 * ((*net1).alphas[i] + (*net2).alphas[i]);
      }
      for (i = 0; i < (*net3).numParams; ++i)
      {
         (*net3).params[i] = 0.5 * ((*net1).params[i] + (*net2).params[i]);
      }
   }
   normalize((*net3).alphas , (*net3).numVars);
   return ((void*)net3);
}

/*Allocate memory for a given number of parameter arrays*/
Parameters ** initGApopulation(int sz, int n, int p)
{
   int i;
   Parameters ** pop = malloc(sz * sizeof(void*));
   
   for (i=0; i < sz; ++i)
   {
      pop[i] = randomNetwork(n,p);
   }
   return pop;
}

/*Callback function that is called during each GA run*/
int callbackf(int gen, void ** pop, int popsz)
{
   int i,j;
   double x;
   void * y;
   Parameters * p;
   
   y = pop[0];
   x = fitness(y);

   if (PRINT_STEPS)
   {
       printf("%i  %lf\n", gen, x);
       if (x == 1.0) printf("target reached.\n\n");
   }
   if (x == 1.0) 
   { 
       return (1); 
   }

   if (gen > 0 && (gen % 20) == 0)
   {
       p = 0;
       for (i=0; i < popsz/2; ++i)
       {
           p = (Parameters*)pop[i];
           for (j=0; j < p->numVars; ++j) p->alphas[j] *= 2.0 * randnum;
           for (j=0; j < p->numParams; ++j) p->params[j] *= 2.0 * randnum;
           normalize (p->alphas , p->numVars);
           normalize (p->params , p->numParams);
       }
   }

   return (0);
}

static double** findTwoSteadyStates(Parameters * p0)
{
   double * iv = unstableSteadyState(p0,INIT_VALUE), * iv2, * y, * y0, fopt, diff, ** ys;
   int i,j;
   Parameters * p;

   if (!iv) return 0;

   p = clone((void*)p0);
   for (i=0; i < p->numVars; ++i) p->alphas[i] = 1.0;

   iv2 = malloc( p->numVars * sizeof(double) ); //unstable point
   for (i=0; i < p->numVars; ++i) iv2[i] = iv[i];

   y = y0 = 0; //y0 and y are the two steady states (if they exist)

   for (i=0; i < 100; ++i)
   {
      for (j=0; j < p->numVars; ++j)
           iv2[j] = iv[j] + 10.0*randnum - 5.0;  //random perturbation

      //y = steadyState(p->numVars,iv2, ODE_FNC, p,SS_MIN_ERROR,SS_MAX_TIME,SS_MIN_DT); //steady state
      y = 0;
      _DU = malloc( (p->numVars) * sizeof(double));
      _PARAM = p;
      for (j=0; j < p->numVars; ++j)
      {
          _U0[j] = _U[j] = INIT_VALUE[j];
      }
      if (NelderMeadSimplexMethod(p->numVars, &(FMIN), _U, 1.00, &fopt, 1000.0, 1.0e-10) == success)
      {
          if (fopt < 1.0e-5)
              y = _DU;
          else
              free(_DU);
      }
      else
          free(_DU);

      if (y)
      {
          for (j=0; j < p->numVars; ++j)
               if (y[j]==iv2[j])
                   y[j] = 0.0;
          if (y0)  //first steady state exists
          {
             diff = 0;  //difference between second steady state
             for (j=0; j < p->numVars; ++j)
                  diff += (y[j] - y0[j])*(y[j] - y0[j]);

             if (diff > MIN_ERROR) //significantly different
             {
                 deleteGAindividual((void*)p);
                 ys = malloc(3 * sizeof(double*));
                 ys[0] = iv;
                 ys[1] = y0;
                 ys[2] = y;
                 free(iv2);
                 return ys;  //found two different steady states
             }
             free(y);
          }
          else  //first steady state does not exist
          {
             y0 = y;
          }
      }
   }
   free(iv);
   free(iv2);
   deleteGAindividual((void*)p);
   free(y);
   free(y0);
   return 0;
}

BistablePoint makeBistable(int n, int p,double* iv, int maxIter, int popSz, void (*odefnc)(double,double*,double*,void*))
{
   //ODEflags(1);
   int popsz1, i;
   BistablePoint ans;
   Parameters * param;
   GApopulation pop;
   
   _U0 = malloc( n * sizeof(double));
   _U = malloc( n * sizeof(double));
   ODE_FNC = odefnc;
   popsz1 = popSz/5;
   INIT_VALUE = iv;

   GAinit(&deleteGAindividual,&clone,&fitness,&crossover,&mutate,0);
   pop = GArun((void**)initGApopulation(popSz,n,p),popSz,popsz1,maxIter,&callbackf);
   
   param = pop[0];
 
   for (i=1; i < popsz1; ++i) deleteGAindividual(pop[i]);
   free(pop);
   
   ans.param = 0;
   ans.unstable = ans.stable1 = ans.stable2 = 0;

   if (fitness((void*)param) < 1)
   {
       deleteGAindividual(param);
       return ans;
   }

   ans.param = param;
   ans.unstable = ans.stable1 = ans.stable2 = 0;

   //double ** ys = findTwoSteadyStates(param);
   // if (ys)
   if (UNSTABLE_PT)
       ans.unstable = UNSTABLE_PT;

   if (STABLE_PT)
       ans.stable1 = STABLE_PT;

   free(_U0);
   free(_U);
   deleteBadParams();
   return ans;
}

/************************
   Just for Tinkercell
************************/

#include "TC_api.h"
#include "cvodesim.h"
#include "ode.c"

static double _time0_ = 0.0;
static double * rates = 0;
void odeFunc( double time, double * u, double * du, void * udata )
{
	int i,j;
	TCpropensity(time, u, rates, udata);
	for (i=0; i < TCvars; ++i)
	{
		du[i] = 0;
		for (j=0; j < TCreactions; ++j)
		{
			if (valueAt(TCstoic,TCreactions,i,j) != 0)
			du[i] += rates[j]*valueAt(TCstoic,TCreactions,i,j);
		}
	}
}

int main()
{
	int i,j;
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));
	double * rates = malloc(TCreactions * sizeof(double));
	TCinitialize(model);
	
	(*model) = TC_initial_model;
	
	BistablePoint p = makeBistable(TCvars, TCparams, TCinit , 1000, 1000, &odeFunc);
	
   	if (p.stable1)
	{
		printf("First stable point:\n");
		for (i=0; i < p.param->numVars; ++i)
			printf("%lf\n",p.stable1[i]);		
		printf("\n");
		free(p.stable1);
	}
	
	if (p.stable2)
	{
		printf("Second stable point:\n");
		for (i=0; i < p.param->numVars; ++i)
			printf("%lf\n",p.stable2[i]);		
		printf("\n");
		free(p.stable2);
	}
	
	if (p.unstable)
	{
		printf("Unstable point:\n");
		for (i=0; i < p.param->numVars; ++i)
			printf("%lf\n",p.unstable[i]);		
		printf("\n");
		free(p.unstable);
	}
	
	if (p.param)
	{
		printf("Parameters of bistable system:\n");
		for (i=0; i < p.param->numParams; ++i)
			printf("%lf\n",p.param->params[i]);
		
		printf("\n");
		
		if (p.param->params) free (p.param->params);
		if (p.param->alphas) free (p.param->alphas);
		free(p.param);
	}
	
	free(rates);
	free(model);
	return 0;
}


