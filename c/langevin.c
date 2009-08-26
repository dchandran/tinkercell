#include "langevin.h"

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
		getValue(array,1+n, k, 0) = t;
		for (i=0; i < n; ++i)
			getValue(array,1+n, k, i+1) = y[i];
	
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
				if (getValue(N,m,i,j) != 0)
					dy[i] += rates[j]*getValue(N,m,i,j)*dt - rnorm()*getValue(N,m,i,j)*sqrt(rates[j]*dt);
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


