#include "cvodesim.h"

/*
 * relative error tolerance
 * absolute error tolerance
*/
double RelTol = 1.0E-3, AbsTol = 1.0E-16;

int ODE_POSITIVE_VALUES_ONLY = 0;

/*
 * set the flags
 * @param: only positive values
*/
void ODEonlyPositiveValuesAllowed(int i)
{
   ODE_POSITIVE_VALUES_ONLY = i;
}

/*
 * @param: relative error tolerance allowed
 * @param: absolute error tolerance allowed
*/
void ODEtolerance(double relerr,double abserr)
{
   RelTol = relerr;
   AbsTol = abserr;
}

/**/
typedef struct
{
  void (*ODEfunc)(double, double*, double*, void*);
  void * userData;
  int numEvents;
  EventFunction eventFunction;
  ResponseFunction responseFunction;
} UserFunction;


/* f routine. Compute f(t,u). */

static int f(realtype t, N_Vector u, N_Vector udot, void * userFunc)
{
  realtype *udata, *dudata;
  UserFunction * info;
  
  udata = NV_DATA_S(u);
  dudata = NV_DATA_S(udot);

  info = (UserFunction*) userFunc;

  if ((*info).ODEfunc != NULL)
      ((*info).ODEfunc)(t,udata,dudata,(*info).userData);

  return (0);
}


/* Check function return value...
     opt == 0 means SUNDIALS function allocates memory so check if
              returned NULL pointer
     opt == 1 means SUNDIALS function returns a flag so check if
              flag >= 0
     opt == 2 means function allocates memory so check if returned
              NULL pointer */

static int check_flag(void *flagvalue, int opt)
{
  int *errflag = (int*)flagvalue;

  /* Check if SUNDIALS function returned NULL pointer - no memory allocated */

  if (opt == 0 && flagvalue == NULL) {
    return(1); }

  /* Check if flag < 0 */

  else if (opt == 1) {
    errflag = (int *) flagvalue;
    if (*errflag < 0) {
      return(1); }}

  /* Check if function returned NULL pointer - no memory allocated */

  else if (opt == 2 && flagvalue == NULL) {
    return(1); }

  return(0);
}

/*event functions*/
static int _CheckEventTriggers(realtype t,N_Vector y, realtype *gout, void * g_data)
{
	int i;
	EventFunction event;
	ResponseFunction response;
	realtype *u;
	UserFunction * info;

	u = NV_DATA_S(y);

	info = (UserFunction*) g_data;

	for (i=0; i < info->numEvents; ++i)
	{
		event = info->eventFunction;
		gout[i] = (int)(event(i, t,u,info->userData) == 0);
	}

	return (0);
}

static void _ProcessEvent(realtype t,N_Vector y, void * g_data)
{
	int i, ret=0;
	EventFunction event;
	ResponseFunction response;
	realtype *u;
	UserFunction * info;

	u = NV_DATA_S(y);

	info = (UserFunction*) g_data;

	for (i=0; i < info->numEvents; ++i)
	{
		event = info->eventFunction;
		response = info->responseFunction;
		if (event(i, t,u,(*info).userData) > 0)
			response(i, u,(*info).userData);
	}
}

/* The following is to convert a propensity function and stoichiometry to an ode function*/

typedef struct
{
   void (*propensityFunction)(double, double*,double*,void*);
   double * N;
   int numReactions;
   int numVars;
   double * rates;
   void * data;
} ODEsim2Struct;

static void odeFunc( double time, double * u, double * du, void * udata )
{
   int i,j;
   ODEsim2Struct * s = (ODEsim2Struct*)udata;
   double * Stoichiometrytc_matrix = s->N;
   double * rates = s->rates;
   int numReactions = s->numReactions;
   int numVars = s->numVars;
   
   s->propensityFunction(time, u, rates, s->data);
   
   for (j=0; j < numReactions; ++j)
      if (rates[j] < 0)
	     rates[j] = 0.0;
   
   for (i=0; i < numVars; ++i)
   {
      du[i] = 0;
      for (j=0; j < numReactions; ++j)
      {
         if (Stoichiometrytc_matrix[numReactions*i + j] != 0)
            du[i] += rates[j]*Stoichiometrytc_matrix[numReactions*i + j];
       }
   }
}
/*End of converting propensity + stoichiometry to ode*/

/*
* The Simulate function using Cvode (double precision)
* /param  number of species (rows of stoichiometry matrix)
* /param  number of reactions (columns of stoichiometry matrix)
* /param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
* /param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
* /param  intial values for species
* /param  start time
* /param  end time 
* /param  time increments for the simulation
* /param  user data type for storing other information
* /return  one dimentional array -- { row1, row2...}, where each row contains {time1,x1,x2,....}
*/

double * ODEsim2(int m, int n, double * N, void (*f)(double, double*,double*,void*), double *x0, double startTime, double endTime, double dt, void * dataptr, int numEvents, EventFunction eventFunctions, ResponseFunction responseFunctions)
{
	double * y;
	ODEsim2Struct * s = (ODEsim2Struct*)malloc(sizeof(ODEsim2Struct));
	s->N = N;
	s->propensityFunction = f;
	s->numReactions = n;
	s->numVars = m;
	s->rates = (double*) malloc(n * sizeof(double));
	s->data = dataptr;
	
	y = ODEsim(m,x0,&odeFunc,startTime,endTime,dt,s,numEvents, eventFunctions, responseFunctions);
	
	free(s->rates);
	free(s);
	
	return(y);
}

/*
 * Gets jacobian matrix of the system at the given point
 * /param  number of species (rows of stoichiometry matrix)
 * /param  number of reactions (columns of stoichiometry matrix)
 * /param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
 * /param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
 * /param: array of values (point where Jacobian will be calculated)
 * /param: additional parameters needed for ode function
 * /ret: 2D array made into linear array
 */
double* jacobian2(int m, int n, double * N, void (*f)(double,double*,double*,void*), double * point, void * dataptr, double * eigenreal, double * eigenim)
{
	double * y;
	ODEsim2Struct * s = (ODEsim2Struct*)malloc(sizeof(ODEsim2Struct));
	s->N = N;
	s->propensityFunction = f;
	s->numReactions = n;
	s->numVars = m;
	s->rates = (double*) malloc(n * sizeof(double));
	s->data = dataptr;
	
	y = jacobian(m,point, &odeFunc, s, eigenreal, eigenim);
	
	free(s->rates);
	free(s);
	
	return(y);
}

/*
 * Bring a system to steady state
 * /param  number of species (rows of stoichiometry matrix)
 * /param  number of reactions (columns of stoichiometry matrix)
 * /param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
 * /param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
 * /param: array of initial values
 * /param: minimum allowed value
 * /param: maximum time for simulation
 * /param: the difference in time to use for estimating steady state
 * /ret: array of values
 */
double* steadyState2(int m, int n, double * N, void (*f)(double,double*,double*,void*), double * initialValues, void * dataptr, double minerr, double maxtime, double delta, int numEvents, EventFunction eventFunctions, ResponseFunction responseFunctions)
{
	double * y;
	ODEsim2Struct * s = (ODEsim2Struct*)malloc(sizeof(ODEsim2Struct));
	s->N = N;
	s->propensityFunction = f;
	s->numReactions = n;
	s->numVars = m;
	s->rates = (double*) malloc(n * sizeof(double));
	s->data = dataptr;
	y = steadyState(m, initialValues, &odeFunc, s, minerr, maxtime, delta, numEvents, eventFunctions, responseFunctions);
	free(s->rates);
	free(s);
	return(y);
}
/*
 * Find the rates of change after simulating for the given amount of time
 * /param  number of species (rows of stoichiometry matrix)
 * /param  number of reactions (columns of stoichiometry matrix)
 * /param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
 * /param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
 * /param: array of initial values
 * /param: time for simulation
 * /ret: array of values
 */
double* getDerivatives2(int m, int n, double * N, void (*f)(double,double*,double*,void*), double * initValues, double startTime, double endTime, double stepSize, void * dataptr)
{
	double * y;
	ODEsim2Struct * s = (ODEsim2Struct*)malloc(sizeof(ODEsim2Struct));
	s->N = N;
	s->propensityFunction = f;
	s->numReactions = n;
	s->numVars = m;
	s->rates = (double*) malloc(n * sizeof(double));
	s->data = dataptr;
	y = getDerivatives(m, initValues, &odeFunc, startTime, endTime, stepSize, s);
	free(s->rates);
	free(s);
	return(y);
}

double* ODEsim(int N, double* initialValues, void (*odefnc)(double,double*,double*,void*), double startTime, double endTime, double stepSize, void * params, int numEvents, EventFunction eventFunctions, ResponseFunction responseFunctions)
{
	double reltol, abstol, t, tout, * data, * y;
	void * cvode_mem = 0;
	N_Vector u;
	int flag, i, j, M;
	UserFunction * funcData;
	realtype * udata;
	int * gi = 0;
	
	t = 0.0;
	tout = 0.0;
	
	if (startTime < 0) startTime = 0;
	if (endTime < startTime) { return 0; }

	if ( (2*stepSize) > (endTime-startTime) ) stepSize = (endTime - startTime)/2.0;

	/*setup tolerance*/

	reltol = RelTol; 
	abstol = AbsTol;

	if (N < 1) { return (0); }  /*no variables in the system*/

	u = N_VNew_Serial(N);  /* Allocate u vector */
	if(check_flag((void*)u, 0)) { return(0); }

	/* Initialize u vector */

	udata = NV_DATA_S(u);

	if (initialValues != NULL)
		for (i=0; i < N; ++i)
			udata[i] = initialValues[i];

	/* allocate output matrix */

	M = (int)((endTime - startTime) / stepSize);
	data = (double*) malloc ((N+1) * (M+1)  * sizeof(double) );

	/* setup CVODE */

	cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
	if (check_flag((void *)cvode_mem, 0)) return(0);

	flag = CVodeInit(cvode_mem, f, startTime, u);
	if (check_flag(&flag, 1))
	{
		CVodeFree(&cvode_mem);
		N_VDestroy_Serial(u);
		if (data) free(data);
		return(0);
	}
	
	CVodeSStolerances(cvode_mem, reltol, abstol);

	funcData = (UserFunction*) malloc( sizeof(UserFunction) );
	(*funcData).ODEfunc = odefnc;
	(*funcData).userData = params;
	(*funcData).numEvents = numEvents;
	(*funcData).eventFunction = eventFunctions;
	(*funcData).responseFunction = responseFunctions;

	flag = CVodeSetUserData(cvode_mem, funcData);
	if(check_flag(&flag, 1))
	{
		CVodeFree(&cvode_mem);
		N_VDestroy_Serial(u);
		free(funcData);
		if (data) free(data);
		return(0);
	}

	flag = CVBand(cvode_mem, N, 0, N-1);
	if (check_flag(&flag, 1))
	{
		CVodeFree(&cvode_mem);
		N_VDestroy_Serial(u);
		free(funcData);
		if (data) free(data);
		return(0);
	}
    
    /* setup events */

	if (numEvents > 0)
	{
		flag = CVodeRootInit(cvode_mem, numEvents, _CheckEventTriggers);

		if (flag != CV_SUCCESS)
		{
			CVodeFree(&cvode_mem);
			N_VDestroy_Serial(u);
			free(funcData);
			if (data) free(data);
			return(0);
		}
	}

	/* setup for simulation */

	t = startTime;
	tout = startTime;
	i = 0;
	if (numEvents > 0)
		gi = (int*)malloc(numEvents * sizeof(int));

	/*main simulation loop*/

	y = NV_DATA_S(u);
	
	while (i <= M)
	{
		/*store data*/
		
		if (data)
		{
		   data[ (N+1)*i ] = t;
		   for (j=0; j < N; ++j)
		   {
			   if (ODE_POSITIVE_VALUES_ONLY && y[j] < 0) //specially for non-negative (biological) systems
			   {
				  CVodeFree(&cvode_mem);
				  N_VDestroy_Serial(u);
				  free(funcData);
				  if (data) free(data);
				  data = 0;
				  return 0;
			   }
			   else
				 data[ i*(N+1) + j+1 ] = y[j]; //normal case (negative and positive values allowed)
		   }
		}
		++i;

		tout = t + stepSize;
		flag = CVode(cvode_mem, tout, u, &t, CV_NORMAL);
		y = NV_DATA_S(u);
		
		if (flag == CV_ROOT_RETURN) //check for events
		{
			CVodeGetRootInfo(cvode_mem, gi);
			for (j=0; j < numEvents; ++j)
				if (gi[j])
					responseFunctions(j, y,params); //event triggered response
			flag = CV_SUCCESS;
		}
		
		if (check_flag(&flag,  1))
		{
		   CVodeFree(&cvode_mem);
		   N_VDestroy_Serial(u);
		   free(funcData);
		   free(gi);
		   if (data) free(data);
		   data = 0;	 
		   return 0;
		}
	}
  
	CVodeFree(&cvode_mem);
  
	N_VDestroy_Serial(u);
  
	free(funcData);
	free(gi);
  
	return(data);   /*return outptus*/
}

/*
 * Gets jacobian matrix of the system at the given point
 * @param: number of variables
 * @param: array of values (point where Jacobian will be calculated)
 * @param: ode function pointer
 * @param: additional parameters needed for ode function
 * @ret: 2D array made into linear array
 */
double* jacobian(int N, double * point,  void (*odefnc)(double,double*,double*,void*), void * params, double * wr, double * wi)
{
	int i,j,k;
	double dx, * dy0, * dy1, * J;
	
	if (odefnc == 0 || point == 0) return (0);
	J = (double*) malloc( N*N*sizeof(double));

	dx = 1.0e-5;
	dy0 = (double*) malloc( N*sizeof(double) );
	dy1 = (double*) malloc( N*sizeof(double) );
   

	for (i=0; i < N; ++i)
	{
		point[i] -= dx;             //x = x0-h
		odefnc(1.0,point,dy0,params);  //dy0 = f(x-h)
		point[i] += 2*dx;           //x = x0+h
		odefnc(1.0,point,dy1,params);  //dy1 = f(x+h)
		point[i] -= dx;             //x = x0
		for (j=0; j < N; ++j)
		{
			J[ N*j + i ] = (dy1[j] - dy0[j])/(dx+dx);  // J[j,i] = f(x+h) - f(x-h) / 2h
		}
	}
	free (dy0);
	free (dy1);
	
	if (J && wr && wi)
    {
		k = eigenvalues(J,N,wr,wi); //get eigenvalues of J in wr and wi
	}
	
	return (J);
}

/*
 * Bring a system to steady state
 * @param: number of variables
 * @param: array of initial values
 * @param: ode function pointer
 * @param: maximum allowed value
 * @ret: array of values
 */
double* steadyState(int N, double * initialValues, void (*odefnc)(double,double*,double*,void*), void * params, double maxerr, double maxtime, double delta, int numEvents, EventFunction eventFunctions, ResponseFunction responseFunctions)
{
	double t0, t, tout, stepSize, reltol, abstol, err, temp, * ss;
	void * cvode_mem = 0;
	N_Vector u;
	int flag, i, j;
	UserFunction * funcData;
	realtype * udata, * u0;
	int * gi = 0;
	
	t = 0.0;
	tout = 0.0;
	stepSize = delta/10.0;

	/*setup tolerance*/

	reltol = RelTol; 
	abstol = AbsTol;

	if (N < 1) { return (0); }  /*no variables in the system*/

	u = N_VNew_Serial(N);  /* Allocate u vector */
	if(check_flag((void*)u, 0)) { return(0); }


	/* allocate output vector */
	ss = (double*) malloc (N * sizeof(double) );

	/* Initialize u vector */

	udata = NV_DATA_S(u);
	u0 = (realtype*) malloc(N*sizeof(realtype));
	if (initialValues != NULL)
		for (i=0; i < N; ++i)
			ss[i] = udata[i] = u0[i] = initialValues[i];

	/* setup CVODE */

	cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
	if (check_flag((void *)cvode_mem, 0)) return(0);

	flag = CVodeInit(cvode_mem, f, t, u);
	if (check_flag(&flag, 1))
	{
		CVodeFree(&cvode_mem);
		N_VDestroy_Serial(u);
		if (ss) free(ss);
		if (u0) free(u0);
		return(0);
	}
	
	CVodeSStolerances(cvode_mem, reltol, abstol);

	funcData = (UserFunction*) malloc( sizeof(UserFunction) );
	(*funcData).ODEfunc = odefnc;
	(*funcData).userData = params;
	(*funcData).numEvents = numEvents;
	(*funcData).eventFunction = eventFunctions;
	(*funcData).responseFunction = responseFunctions;

	flag = CVodeSetUserData(cvode_mem, funcData);
	if(check_flag(&flag, 1))
	{
		CVodeFree(&cvode_mem);
		N_VDestroy_Serial(u);
		free(funcData);
		if (ss) free(ss);
		if (u0) free(u0);
		return(0);
	}

	flag = CVBand(cvode_mem, N, 0, N-1);
	if (check_flag(&flag, 1))
	{
		CVodeFree(&cvode_mem);
		N_VDestroy_Serial(u);
		free(funcData);
		if (ss) free(ss);
		if (u0) free(u0);
		return(0);
	}
    
    /* setup events */

	if (numEvents > 0)
	{
		flag = CVodeRootInit(cvode_mem, numEvents, _CheckEventTriggers);

		if (flag != CV_SUCCESS)
		{
			CVodeFree(&cvode_mem);
			N_VDestroy_Serial(u);
			free(funcData);
			if (ss) free(ss);
			if (u0) free(u0);
			return(0);
		}
	}

	/* setup for simulation */

	t = t0 = 0.0;
	tout = t;
	i = 0;
	if (numEvents > 0)
		gi = (int*)malloc(numEvents * sizeof(int));

	/*main simulation loop*/
	
	while (tout <= maxtime)
	{
		tout = t + stepSize;
		flag = CVode(cvode_mem, tout, u, &t, CV_NORMAL);
		
		if (flag == CV_ROOT_RETURN) //check for events
		{
			CVodeGetRootInfo(cvode_mem, gi);
			for (j=0; j < numEvents; ++j)
				if (gi[j])
					responseFunctions(j, udata, params); //event triggered response
			flag = CV_SUCCESS;
		}
		
		if (check_flag(&flag, 1))
		{
			CVodeFree(&cvode_mem);
			N_VDestroy_Serial(u);
			free(funcData);
			if (ss) free(ss);
			if (u0) free(u0);
			free(gi);
			ss = 0;
			u0 = 0;
			return 0;
		}
		
		err = maxerr + 1.0;

		if (ss && (tout - t0) >= delta)  //measure difference between y[t] - y[t-delta]
		{
			t0 = tout;
			err = 0.0;
			for (j=0; j < N; ++j)
			{
				temp = ( udata[j] - u0[j] )*( udata[j] - u0[j] );
				if (temp > err) err = temp;         //max value from all dx/dt
				ss[j] = u0[j] = udata[j];  //next y points
				if (ODE_POSITIVE_VALUES_ONLY && udata[j] < 0)
				{
					CVodeFree(&cvode_mem);
					N_VDestroy_Serial(u);
					free(funcData);
					if (ss) free(ss);
					if (u0) free(u0);
					free(gi);
					ss = 0; 
					u0 = 0;
					return 0;
				}
			}
		}
		/* check for steady state reached */
		if (err <= maxerr)
		{
		   break;
		}
	}
	if (tout >= maxtime) //steady state not reached in the given amount of time
	{
		if (ss) free(ss);
		if (u0) free(u0);
		ss = 0;
		u0 = 0;
	}
  
	if (u0) free(u0);
	free(gi);
	CVodeFree(&cvode_mem);  /* Free the integrator memory */
	N_VDestroy_Serial(u);
	free(funcData);
	return(ss);   /*return outptus*/
}

/*
 * Find the rates of change after simulating for the given amount of time
 * @param: number of variables
 * @param: array of initial values
 * @param: ode function pointer
 * @param: time for simulation
 * @ret: array of values
 */
double* getDerivatives(int N, double * initialValues, void (*odefnc)(double,double*,double*,void*), double startTime, double endTime, double stepSize, void * params)
{
	double * y, * dy;
	int i, sz;
	
	y = ODEsim(N,initialValues,odefnc,startTime,endTime,stepSize,params,0,0,0);
	if (y == 0) return 0;
	sz = (int)((endTime-startTime)/stepSize);
	dy = (double*) malloc(N * sizeof(double));
  
	for (i=0; i < N; ++i)
	{
	  dy[i] = ( y[ sz*(1+N) + 1+i ] - y[ (1+N)*(sz-1) + 1+i ] )/ stepSize;
	}
	
	free(y);
	return(dy);   /*return outptus*/
}

/*
* print a linearized 2D table to a file
*/
void writeToFile(char* filename, double* data, int rows, int cols)
{
	int i,j;
	FILE * out;
	
	out = fopen(filename,"w");
	
	for (i=0; i < rows; ++i)
	{
		fprintf(out, "%lf", data [ i*cols ]);

		for (j=1; j < cols; ++j)
			fprintf(out, "\t%lf", data[cols*i + j]);

		fprintf(out, "\n");
	}
	fclose(out);
}


