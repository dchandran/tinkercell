/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ****************************************************************************/
 
 #include "ssa.h"

#define valueAt(array, N, i, j) ( array[ (((i)*(N)) + (j)) ] )

double * SSA(int m, int n, double * N, PropensityFunction propensity, double *x0, double startTime, double endTime, int maxSz, int * arraysz, void * dataptr, int numEvents, EventFunction eventFunction, ResponseFunction responseFunction)  //ssa
{	
	int iter, i, k, sz;
	double * x, * y, * v, rand, lambda, time, sum, * x2;
	
	x = (double*) malloc( (1+m) * maxSz * sizeof(double) );   //output	
	y = (double*) malloc( m * sizeof(double) );   //values	
	v = (double*) malloc( n * sizeof(double) );   //rates
	
	iter = i = k = 0;
	rand = lambda = sum = 0;
	
	time = startTime;
	//initialize values
	x[ 0 ] = startTime;
	for (i = 0; i < m; ++i)
	{
		if (x0[i] < 0.0)
			x0[i] = 1.0;
		if (x0[i] > 0.0 && x0[i] < 1.0)
			x0[i] = 1.0;
		x[ i+1 ] = y[i] = x0[i];   
	}
	
	while (time < endTime && iter < maxSz)   //the big loop
	{	
		for (i=0; i < numEvents; ++i)               //events
			if (eventFunction(i,time,y,dataptr) != 0)
				responseFunction(i,y,dataptr);

		x[ (1+m)*iter ] = time;
		for (i = 0; i < m; ++i)   //store output data
		{
			x[ (1+m)*iter + i+1 ] = y[i];		
		}
		
		propensity(time, y, v, dataptr);  //calculate rates
		for (i=0; i < n; ++i)
			if (v[i] < 0.0)
				v[i] = 0.0;
		
		lambda = 0;
		for (i=0; i < n; ++i) lambda += v[i];   //lambda = sum of rates
		
		if (lambda <= 0) 
		{ 
			++iter; 
			break; 
		}
		
		for (i =0; i < n; ++i) v[i] = v[i] / lambda;  //convert to prob values
		
		
		sum = 0;
		rand = mtrand();
		for (k = 0; k < n; ++k)    //pick a reaction
		{			
			if ((sum + v[k]) > rand) break;
			sum += v[k];
		}
		
		//printf("%i\n",k);
		for (i = 0; i < m; ++i)       //update values
		{
			y[i] = y[i] + N[i*n+k];
			if (y[i] < 0.0) y[i] = 0.0;
		}
		
		rand = mtrand();
		
		time -= log(rand)/lambda;		//update time
		++iter;		
	}
	
	if (y) free(y);
	if (v) free(v);
	
	--iter;
	x2 = (double*) malloc( (1+m) * iter * sizeof(double) );
	
	sz = (1+m)*iter;
	for (i=0; i < sz; ++i) x2[i] = x[i];
	free(x);
	x = x2;	
	
	if (arraysz) (*arraysz) = iter;
	
	return x;
}

double * getRatesFromSimulatedData(double* data, int rows, int cols1, int cols2, void (*f)(double,double*,double*,void*), void* param)
{
	int i,j;
	double * y, * rates, * dat, time = 0.0;
	
	y = malloc( cols1 * sizeof(double));
	rates = malloc( cols2 * sizeof(double));
	dat = malloc(rows * (1+cols2) * ( sizeof(double)));
	
	for (i=0; i < rows; ++i)
	{
		for (j=0; j < cols1; ++j)		
			y[j] = data [ (1+cols1)*i + j+1 ];  //get simulated data row i
		
		time = data [ (1+cols1)*i ];
		f(time,y,rates,param); //get rates
		
		dat [ (1+cols2)*i ] = time;
		for (j=0; j < cols2; ++j)
			dat [ (1+cols2)*i + 1+j ] = rates[j];
	}
	
	free(y);
	free(rates);
	
	return dat;
}
double rnorm()
{
	return (sqrt(-2.0*log( mtrand() )) *cos (2.0*3.14159* mtrand() ));
}

double * Langevin(int n, int m, double * N, PropensityFunction propensity, double * inits, double endTime, double dt, void * params)
{
	double t = 0;	
	double * array = malloc( (1 + endTime/dt) * (1+n) * sizeof(double) );
	double * y = malloc( n * sizeof(double) );
	double * dy = malloc( n * sizeof(double) );
	double * rates = malloc (m * sizeof(double) );	
	int i,j,k;
	
	for (i=0; i < n; ++i) y[i] = inits[i];
	
	k = 0;
	
	while (t < endTime)
	{
		//store into output matrix
		valueAt(array,1+n, k, 0) = t;
		for (i=0; i < n; ++i)
			valueAt(array,1+n, k, i+1) = y[i];
	
		propensity(t, y, rates, params);  //get rates
		
		for (j=0; j < m; ++j)
			if (rates[j] < 0)
				rates[j] = 0;
		
		//calc stochastic diff. eqs
		for (i=0; i < n; ++i)
		{
			dy[i] = 0;
			for (j=0; j < m; ++j)
			{
				if (valueAt(N,m,i,j) != 0)
					dy[i] += rates[j]*valueAt(N,m,i,j)*dt - rnorm()*valueAt(N,m,i,j)*sqrt(rates[j]*dt);
			}
			y[i] += dy[i];
		}
		
		t += dt;
		++k;
	}
	
	free(y);
	free(dy);
	free(rates);
	
	return array;
}


