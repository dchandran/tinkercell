/****************************************************************************
**
** This file creates an input window which allows users to run the runssa.c code
** with specific inputs for start time, end time, max array size, and x-axis
** AND
** gets information from TinkerCell, generates a rate equation model, runs
** the Gillespie simulation, and outputs the data to TinkerCell
**
****************************************************************************/

#include "TC_api.h"

void runSSA(Matrix input)
{
	int maxsz = 100000,i;
	double time = 50.0;
	int xaxis = 0, k, sz = 0, selection = 0, rateplot = 0;
	Array A,B;
	FILE * out;
	char* appDir;
	Matrix params, initVals, allParams;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)valueAt(input,0,0);
		if (input.rows > 1)
			time = valueAt(input,1,0);
		if (input.rows > 2)
			maxsz = (int)valueAt(input,2,0);
		if (input.rows > 3)
			rateplot = (int)valueAt(input,3,0);
	}

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (A[0] == 0)
		{
			TCFreeArray(A);
			//A = tc_allItems();
			tc_errorReport("No Model Selected\0");
			return;

		}
	}
	else
	{
		A = tc_allItems();
	}
	
	params = tc_getModelParameters(A);
	B = tc_itemsOfFamilyFrom("Molecule\0",A);
	initVals = tc_getInitialValues(B);
	
	allParams.rows = (initVals.rows+params.rows);
	allParams.cols = 2;
	allParams.rownames = (char**)malloc((initVals.rows+params.rows+1)*sizeof(char*));
	allParams.values = (double*)malloc(2*allParams.rows*sizeof(double));
	allParams.colnames = 0;
	
	for (i=0; i < params.rows; ++i)
	{
		allParams.rownames[i] = params.rownames[i];
		valueAt(allParams,i,1) = 
				2*valueAt(params,i,0) - 
								(valueAt(allParams,i,0) = valueAt(params,i,0)/10.0);
	}
	for (i=0; i < initVals.rows; ++i)
	{
		allParams.rownames[i+params.rows] = initVals.rownames[i];
		valueAt(allParams,i+params.rows,1) = 
				2*valueAt(initVals,i,0) - 
								(valueAt(allParams,i+params.rows,0) = valueAt(initVals,i,0)/10.0);
	}
	allParams.rownames[(initVals.rows+params.rows)] = 0;
	
	free(params.rownames);
	params.rownames = 0;
	free(initVals.rownames);
	initVals.rownames = 0;
	TCFreeMatrix(initVals);
	TCFreeMatrix(params);
	TCFreeArray(B);

	if (A[0] != 0)
	{
		k = tc_writeModel( "runssa", A );
		TCFreeArray(A);
		if (!k)
		{
			tc_errorReport("No Model\0");
			TCFreeMatrix(allParams);
			return;
		}
	}
	else
	{
		TCFreeArray(A);
		TCFreeMatrix(allParams);
		tc_errorReport("No Model\0");
		return;
	}

	out = fopen("runssa.c","a");
	
	if (!out)
	{
		TCFreeArray(A);
		TCFreeMatrix(allParams);
		tc_errorReport("Cannot write to file runssa.c in user directory\0");
		return;
	}
	
	fprintf(out, "void assignInputs(double * k, TCmodel * model)\n{\n" );
	
	for (i=0; i < allParams.rows; ++i)
	{
		fprintf(out, "    model->%s = k[%i];\n", allParams.rownames[i], i );
	}

	fprintf( out , "}\n\
#include \"TC_api.h\"\n\
#include \"ssa.h\"\n\n\
static double _time0_ = 0.0;\n\
void ssaFunc(double time, double * u, double * rates, void * data)\n\
{\n\
	TCpropensity(time, u, rates, data);\n\
	if (time > _time0_)\n\
	{\n\
			tc_showProgress(\"Gillespie algorithm\",(int)(100 * time/%lf));\n\
			_time0_ += %lf;\n\
	}\n\
}\n\
\n\
static void computeStats(double * mu, double * var, Matrix * values, void * data)\n\
{\n\
	int i,j;\n\
	double * sum_xx = (double*)malloc((TCvars+TCreactions) * sizeof(double));\n\
	double * sum_x = (double*)malloc((TCvars+TCreactions) * sizeof(double));\n\
	double * rates = (double*)malloc(TCreactions * sizeof(double));\n\
	double * u = (double*)malloc(TCvars * sizeof(double));\n\
	for (i=0; i < (TCvars+TCreactions); ++i)\n\
		sum_x[i] = sum_xx[i] = 0.0;\n\
	for (i=values->rows/2; i < values->rows; ++i)\n\
	{\n\
		for (j=0; j < TCvars; ++j)\n\
		{\n\
			u[j] = valueAt((*values),i,j+1);\n\
			sum_x[j] += u[j];\n\
			sum_xx[j] += u[j]*u[j];\n\
		}\n\
		TCpropensity(0,u,rates,data);\n\
		for (j=0; j < TCreactions; ++j)\n\
		{\n\
			sum_x[j+TCvars] += rates[j];\n\
			sum_xx[j+TCvars] += rates[j]*rates[j];\n\
		}\n\
	}\n\
	for (j=0; j < (TCvars+TCreactions); ++j)\n\
	{\n\
		mu[j] = sum_x[j]/(values->rows/2);\n\
		var[j] = sum_xx[j]/(values->rows/2) - mu[j]*mu[j];\n\
	}\n\
	free(u);\n\
	free(rates);\n\
	free(sum_x);\n\
	free(sum_xx);\n\
}\n\
void run(Matrix input) \n\
{\n\
	initMTrand();\n\
	int sz = 0,i;\n\
	double * y, *y0, * mu, * var;\n\
	Matrix data;\n\
	Array A;\n\
	char ** names;\n\
	char s[100];\n\
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	(*model) = TC_initial_model;\n\
	if (input.rows > TCparams)\n\
		assignInputs(input.values,model);\n\
	TCinitialize(model);\n\
	y = SSA(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, 0, %lf, %i, &sz, (void*)model);\n\
	if (!y) \
	{\n\
		tc_errorReport(\"Stochastic simulation failed! Some values might be becoming negative or reaching Inf. Double check your model.\");\n\
		return;\n\
	}\n\
	data.rows = sz;\n\
	data.cols = 1+TCvars;\n\
	data.values = y;\n\
	mu = (double*)malloc((TCvars+TCreactions)*sizeof(double));\n\
	var = (double*)malloc((TCvars+TCreactions)*sizeof(double));\n\
	computeStats(mu,var,&data,(void*)model);\n\
	A = tc_findItems(TCvarnames);\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
	   sprintf(s, \"mean=%%.3lf \\nsd=%%.3lf\",mu[i],var[i]);\n\
	   tc_displayText(A[i],s);\n\
	}\n\
	free(A);\n\
	A = tc_findItems(TCreactionnames);\n\
	for (i=0; i < TCreactions; ++i)\n\
	{\n\
	   sprintf(s, \"mean=%%.3lf \\nsd=%%.3lf\",mu[i+TCvars],var[i+TCvars]);\n\
	   tc_displayText(A[i],s);\n\
	}\n\
	free(A);\n\
	free(mu);\n\
	free(var);\n\
	names = TCvarnames;\n\
	if (%i)\n\
	{\n\
		y0 = getRatesFromSimulatedData(y, data.rows, TCvars , TCreactions , 1 , &(TCpropensity), 0);\n\
		free(y);\n\
		y = y0;\n\
		TCvars = TCreactions;\n\
		names = TCreactionnames;\n\
	}\n\
	data.cols = 1+TCvars;\n\
	data.values = y;\n\
	data.rownames = 0;\n\
	data.colnames = malloc( (1+TCvars) * sizeof(char*) );\n\
	data.colnames[0] = \"time\\0\";\n\
	int i,j;\n\
	for(i=0; i<TCvars; ++i) data.colnames[1+i] = names[i];\n\
	tc_multiplot(2,1);\n\
	tc_plot(data,%i,\"Stochastic Simulation\",0);\n\
	tc_hist(data,1,\"Histogram\");\n\
	free(data.colnames);\n\
	free(y);\n\
	free(model);\n\
	return;\n}\n",time,time/20.0,time,maxsz,rateplot,xaxis);

	fclose(out);

	//tc_compileBuildLoad("runssa.c -lssa\0","run\0","Gillespie algorithm\0");
	tc_compileBuildLoadSliders("runssa.c -lssa\0","run\0","Gillespie algorithm\0",allParams);
	
	TCFreeMatrix(allParams);
	
	return;
}

void runCellSSA(Matrix input)
{
	double time = 50.0;
	int xaxis = 0;
	int selection = 0, k;
	int numcells = 10;
	double replication = 0.05;
	double death = 0.0001;
	double mutants = 0.001;
	int gridsz = 100;
	FILE * out;
	Array A;
	char * cmd;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)valueAt(input,0,0);
		if (input.rows > 1)
			time = valueAt(input,1,0);
		if (input.rows > 2)
			numcells = (int)valueAt(input,2,0);
		if (input.rows > 3)
			replication = valueAt(input,3,0);
		if (input.rows > 4)
			death = valueAt(input,4,0);
		if (input.rows > 5)
			mutants = (int)valueAt(input,5,0);
		if (input.rows > 6)
			gridsz = (int)valueAt(input,6,0);
	}

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (A[0] == 0)
		{
			TCFreeArray(A);
			//A = tc_allItems();
			tc_errorReport("No Model Selected\0");
			return;

		}
	}
	else
	{
		A = tc_allItems();
	}

	if (A[0] != 0)
	{
		k = tc_writeModel( "cells_ssa", A );
		TCFreeArray(A);
		if (!k)
		{
			tc_errorReport("No Model\0");
			return;
		}
	}
	else
	{
		TCFreeArray(A);
		tc_errorReport("No Model\0");
		return;
	}

	out = fopen("cells_ssa.c","a");

	fprintf( out , "#include \"TC_api.h\"\n#include \"cells_ssa.h\"\n\n\
				   static double _time0_ = 0.0;\n\
				   void ssaFunc(double time, double * u, double * rates, void * data)\n\
				   {\n\
				   TCpropensity(time, u, rates, data);\n\
				   if (time > _time0_)\n\
				   {\n\
				   tc_showProgress(\"Multi-cell algorithm\",(int)(100 * time/%lf));\n\
				   _time0_ += %lf;\n\
				   }\n\
				   }\n\
				   \n\
				   \n\
				   void run() \n\
				   {\n\
				   initMTrand();\n\
				   TCinitialize();\n\
				   int sz = 0;\n\
				   double ** y = cells_ssa(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, %lf, %i, 0, %i, %lf, %lf, %lf);\n\
				   if (!y || !y[0] || !y[1]) \
				   {\n\
				   tc_errorReport(\"Simulation failed! Possible cause of failure: some values are becoming negative. Double check your model.\");\n\
				   return;\n\
				   }\n\
				   Matrix data1;\n\
				   Matrix data2;\n\
				   data2.rows = sz;\n\
				   data2.cols = 2;\n\
				   data2.values = y[0];\n\
				   data2.colnames = malloc(2 * sizeof(char*));\n\
				   data2.colnames[0] = \"time\";\n\
				   data2.colnames[1] = \"cells\";\n\
				   data2.rownames = 0;\n\
				   data1.rows = sz;\n\
				   char ** names = TCvarnames;\n\
				   data1.cols = 1+TCvars;\n\
				   data1.values = y[1];\n\
				   data1.rownames = 0;\n\
				   data1.colnames = malloc( (1+TCvars) * sizeof(char*) );\n\
				   data1.colnames[0] = \"time\\0\";\n\
				   int i,j;\n\
				   for(i=0; i<TCvars; ++i) data1.colnames[1+i] = names[i];\n\
				   tc_plot(data1,0,\"Multi-cell simulation\",0);\n\
				   tc_plot(data2,0,\"Cell growth\",1);\n\
				   free(data1.colnames);\n\
				   free(data2.colnames);\n\
				   free(y[0]);\n\
				   free(y[1]);\n\
				   free(y);\n\
				   return;\n}\n",time,time/20.0,time,gridsz,numcells,replication,death,mutants);

	fclose(out);

	tc_compileBuildLoad("cells_ssa.c -lssa\0","run\0","Multi-cell algorithm\0");

	return;
}

void setupSSA()
{
	Matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "time", "max size", "plot", 0 };
	double values[] = { 0, 100, 100000, 0 };
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important
	char * options2[] = { "Variables", "Rates", 0 }; //null terminated -- very important

	m.rows = 4;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	tc_createInputWindow(m,"Gillespie algorithm",&runSSA);
	tc_addInputWindowOptions("Gillespie algorithm",0, 0,  options1);
	tc_addInputWindowOptions("Gillespie algorithm",3, 0,  options2);
}

void setupCellSSA()
{
	Matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "time", "num. cells", "growth rate", "death rate", "mutation rate", "num. points" , 0 };
	double values[] = { 0, 100, 100, 0.05, 0.001, 0.001, 100 };
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	m.rows = 7;
	m.cols = 1;

	tc_createInputWindow(m,"Multi-cell stochastic simulation",&runCellSSA);
	tc_addInputWindowOptions("Multi-cell stochastic simulation",0, 0,  options1);
}

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setupSSA, "Discreet stochastic simulation", "uses custom Gillespie algorithm (compiles to C program)", "Simulate", "Plugins/c/stochastic.PNG", "", 1, 0, 0);
	tc_addFunction(&setupCellSSA, "Multi-cell stochastic simulation", "uses custom Gillespie algorithm (compiles to C program)", "Simulate", "Plugins/c/cells.PNG", "", 1, 0, 0);
}
