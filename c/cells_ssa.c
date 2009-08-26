/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ****************************************************************************/
 
 #include "cells_ssa.h"
/*
 * print a linearized 2D table to a file
*/
static void writeToFile(char* filename, double* data, int rows, int cols)
{
	int i,j;
	FILE * out;

	out = fopen(filename,"w");
	for (i=0; i < rows; ++i)
	{
		fprintf(out, "%lf",getValue(data,cols,i,0));

		for (j=1; j < cols; ++j)
			fprintf(out, "\t%lf",getValue(data,cols,i,j));

		fprintf(out, "\n");
	}
	fclose(out);
}


//SSA:

double ** Cells_ssa(int m, int n, double * N, void (*propensity)(double, double*,double*,void*), double *x0, double endTime, int numPoints, void * dataptr, int numCells, double repRate, double deathRate, double mutantAdv)  //multi cell ssa
{	
	int iter = 0, i = 0, j = 0, k = 0, t0 = 0, t1 = 0;
	double rand = 0, lambda = 0, time = 0, sum = 0, v1, v2, T, t, prevt, * cells, * simu, * y, gridSz,
		** concs, ** concs2, * growthRates, * growthRates2 = 0, * v, **x;
	
	
	initMTrand();
	
	cells = (double*) malloc( numPoints * sizeof(double) );   //output 1 - cell growth
	simu = (double*) malloc( (1+m) * numPoints * sizeof(double) );   //output 2 - concentrations
	gridSz = endTime/((double)numPoints);
	
	for (i=0; i < ((1+m) * numPoints); ++i)
		simu[i] = 0.0;

	for (i=0; i < (numPoints); ++i)
	{
		getValue(simu,m+1,i,0) = gridSz * (double)i; //time
		cells[i] = 0.0;
	}
	
	concs = malloc( numCells * sizeof(double*) );   //species concentrations in each cell
	concs2 = 0;
	growthRates = malloc(numCells * sizeof(double));
	growthRates2 = 0;

	//initialize values across all cells
	for (i=0; i < numCells; ++i)
	{
		growthRates[i] = 1.0;
		concs[i] = malloc(m * sizeof(double));
		
		for (j=0; j < m; ++j)
			concs[i][j] = x0[j];
	}
	growthRates[numCells-1] = 1.0 + mutantAdv;
	
	getValue(simu,1+m,0,0) = 0;
	for (j=0; j < m; ++j)
		getValue(simu,1+m,0,j+1) = x0[j];
	
	v = malloc( n * sizeof(double) );   //rates vector
	
	while (time < endTime)   //the SSA for cells
	{
		//calculate rates and probabilities for cell growth and cell death
		
		if (numCells < 1) break;
		
		v1 = numCells * repRate;
		v2 = numCells * numCells * deathRate;
		
		lambda = v1+v2;
		
		T =  -log(mtrand())/lambda;
		
		v1 /= lambda;
		v2 /= lambda;
		
		t0 = (int)(time/gridSz);
		t1 = (int)((time+T)/gridSz);
		for (; t0 < t1; ++t0) //fill grid values
		{	
			cells[t0] = (double)numCells;
		}
		
		if (mtrand() < v1) //cell growth event
		{
			concs2 = concs;
			concs = malloc( (1+numCells) * sizeof(double*) );
			for (i=0; i < numCells; ++i)
			{
				concs[i] = malloc( m * sizeof(double) );
				for (j=0; j < m; ++j)
					concs[i][j] = concs2[i][j];
				free(concs2[i]);
			}
			
			concs[numCells] = malloc( m * sizeof(double) );
			free(concs2);
			
			lambda = 0.0;
			for (i=0; i < numCells; ++i)  //select a random cell
				lambda += growthRates[i];
			
			rand = mtrand()*lambda;
			sum = 0.0;
			
			for (k=0; k < numCells; ++k)
				if ((sum + growthRates[k]) >= rand) //get random cell that divides
					break;
			growthRates2 = growthRates;
			growthRates = malloc( (1+numCells) * sizeof(double) );

			for (i=0; i < numCells; ++i)
				growthRates[i] = growthRates2[i];
			

			free(growthRates2);
			
			growthRates[numCells] = growthRates[k] * (1.0 + 0.1*(mtrand()-0.5));
			for (i=0; i < m; ++i)
				concs[numCells][i] = concs[k][i] = concs[k][i]/2.0;  //cell division halves the concentrations
			
			++numCells;
			printf("done\n");
		}
		else  //cell death event
		{
			printf("death...");
			k = (int)(numCells * mtrand()); //random cell dies
			--numCells;
			
			concs2 = concs;
			concs = malloc( (numCells) * sizeof(double*) );
			for (i=0; i < numCells; ++i) //copy concentrations
			{
				concs[i] = malloc( m * sizeof(double) );
				for (j=0; j < m; ++j)
					concs[i][j] = concs2[i][j];
				free(concs2[i]);
			}
			free(concs2[numCells]);
			free(concs2);
			growthRates2 = growthRates;
			growthRates = (double*) malloc( (numCells) * sizeof(double) );
			
			for (i=0; i < numCells; ++i) //copy growth rates
				growthRates[i] = growthRates2[i];
			free(growthRates2);
		}
		
		for (j=0; j < numCells; ++j) //for each cell
		{
			t = 0.0;
			prevt = time;
			y = concs[j];
				
			while (t < T)  //the inner SSA for concentrations
			{
				t0 = (int)((prevt)/gridSz);
				t1 = (int)((time+t)/gridSz);
				for (; t0 < t1; ++t0) //fill in grid
				{	
					for (i = 0; i < m; ++i)   //store output concentrations data
						getValue(simu,1+m,t0,i+1) += y[i];	
				}
				prevt = time+t;
				
				propensity(t, y, v, dataptr);  //calculate rates
		
				lambda = 0.0;
				for (i=0; i < n; ++i) lambda += v[i];   //lambda = sum of rates
		
				if (lambda == 0) 
				{  
					break; 
				}
		
				for (i=0; i < n; ++i) v[i] /= lambda;  //convert to prob values
		
				sum = 0;
				rand = mtrand();
				for (k = 0; k < n; ++k)    //pick a reaction
				{			
					if ((sum + v[k]) > rand) break;
					sum += v[k];
				}
		
				for (i = 0; i < m; ++i)       //update values
				{
					y[i] = y[i] + N[i*n+k];
					if (y[i] < 0.0) y[i] = 0.0;
				}
		
				t -= log(mtrand())/lambda;		//update time
			}
		}
		time += T;		//update cell event loop time
	}
	if (v) free(v);
	
	free(growthRates);
	
	if (concs && numCells > 0)
	{
		for (i=0; i < numCells; ++i) //copy concentrations
		{
			free(concs[i]);
		}
		free(concs);
	}
	x = malloc(2 * sizeof(double*));
	x[0] = cells;
	x[1] = simu;
	printf("5\n");
	return x;
}

/*
double N[] = 
{
	0, -1, 0, 0,
	1, 1, -1, 0,
	0, 0, 1, -1
};

void propensity(double t, double* u,double* v,void* p)
{
	v[0] = 20.0;	
	v[1] = 5.0*u[0];
	v[2] = 7.0*u[1];
	v[3] = 5.0*u[2];
}

int main()
{
	double x0[] = { 0.0, 0.0, 0.0 }; 
	double ** x = Cells_ssa(3,4, N, &propensity, x0, 200, 100, 0, 10, 0.05, 0.0001, 0.001);
	
	writeToFile("cells.tab",x[0],100,1);
	writeToFile("simu.tab",x[1],100,4);
	
	free(x[0]);
	free(x[1]);
	free(x);
}
*/
