/****************************************************************************
**
** This file creates an input window which allows users to run the runssa.c code
** with specific inputs for start time, end time, max array size, and x-axis
** AND
** gets information from TinkerCell, generates a rate equation model, runs
** the Gillespie simulation, and outputs the data to TinkerCell
**
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "TC_api.h"

static int MAC_WITHOUT_GCC = 0;

void runSSA(tc_matrix input)
{
	int maxsz = 100000,i;
	double time = 50.0;
	int k, sz = 0, selection = 0, rateplot = 0;
	tc_items A,B;
	FILE * out;
	int slider = 1;
	char * runfuncInput = "tc_matrix input";
	char * runfunc = "";
	tc_matrix params, initVals, allParams, N;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			time = tc_getMatrixValue(input,1,0);
		if (input.rows > 2)
			maxsz = (int)tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			rateplot = (int)tc_getMatrixValue(input,3,0);
		if (input.rows > 4)
			slider = (int)tc_getMatrixValue(input,4,0);
	}
	
	if (time < 0) 
	{
		tc_errorReport("simulation time must be a positive");
		return;
	}
	
	if (maxsz < time) 
	{
		maxsz = time;
		return;
	}
	
	if (MAC_WITHOUT_GCC)
	{
		N = tc_simulateSSA(time);
		tc_plot(N,"Stochastic Simulation");
		tc_deleteMatrix(&N);
		return;
	}
	
	if (slider)
		slider = 0;
	else
		slider = 1;
	
	if (selection > 0)
	{
		A = tc_selectedItems();
		if (tc_getItem(A,0) == 0)
		{
			tc_deleteItemsArray(&A);
			tc_errorReport("No Model Selected\0");
			return;

		}
	}
	else
	{
		A = tc_allItems();
	}

	if (slider)
	{
		params = tc_getParameters(A);
		N = tc_getStoichiometry(A);
		B = tc_findItems(N.rownames);
		tc_deleteMatrix(&N);
		initVals = tc_getInitialValues(B);

		allParams = tc_createMatrix(initVals.rows+params.rows,2);

		for (i=0; i < params.rows; ++i)
		{
			tc_setRowName(allParams,i, tc_getRowName(params,i));
			tc_setMatrixValue(allParams,i,0,tc_getMatrixValue(params,i,0)/10.0);
			tc_setMatrixValue(allParams,i,1, 2*tc_getMatrixValue(params,i,0) - tc_getMatrixValue(allParams,i,0));
		}
		for (i=0; i < initVals.rows; ++i)
		{
			tc_setRowName(allParams,i+params.rows, tc_getRowName(initVals,i));
			tc_setMatrixValue(allParams,i+params.rows,0,tc_getMatrixValue(initVals,i,0)/10.0);
			tc_setMatrixValue(allParams,i+params.rows,1, 2*tc_getMatrixValue(initVals,i,0) - tc_getMatrixValue(allParams,i+params.rows,0));
		}
		
		tc_deleteMatrix(&initVals);
		tc_deleteMatrix(&params);
		tc_deleteItemsArray(&B);
		runfunc = runfuncInput;
	}
	
	if (tc_getItem(A,0) != 0)
	{
		k = tc_writeModel( "runssa", A );
		tc_deleteItemsArray(&A);
		if (!k)
		{
			tc_errorReport("No Model\0");
			if (slider)
				tc_deleteMatrix(&allParams);
			return;
		}
	}
	else
	{
		tc_deleteItemsArray(&A);
		if (slider)
			tc_deleteMatrix(&allParams);
		tc_errorReport("No Model\0");
		return;
	}

	out = fopen("runssa.c","a");
	
	if (!out)
	{
		tc_deleteItemsArray(&A);
		if (slider)
			tc_deleteMatrix(&allParams);
		tc_errorReport("Cannot write to file runssa.c in user directory\0");
		return;
	}
	
	fprintf( out , "\
#include \"TC_api.h\"\n\
#include \"ssa.h\"\n\n\
static double _time0_ = 0.0;\n\
void ssaFunc(double time, double * u, double * rates, void * data)\n\
{\n\
	TCpropensity(time, u, rates, data);\n\
	if (time > _time0_)\n\
	{\n\
			tc_showProgress((int)(100 * time/%lf));\n\
			_time0_ += %lf;\n\
	}\n\
}\n\
\n\
static void computeStats(double * mu, double * var, tc_matrix * values, void * data)\n\
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
			u[j] = tc_getMatrixValue((*values),i,j+1);\n\
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
TCAPIEXPORT void run(%s) \n\
{\n\
	initMTrand();\n\
	int sz = 0,i,j;\n\
	double * y, *y0, * mu, * var;\n\
	tc_matrix data;\n\
	tc_items A;\n\
	tc_strings names;\n\
	char s[100];\n\
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	(*model) = TC_initial_model;\n",time,time/20.0,runfunc);

if (slider)
{
	for (i=0; i < allParams.rows; ++i)
		fprintf(out, "    model->%s = tc_getMatrixValue(input,%i,0);\n",tc_getRowName(allParams,i),i);
}

fprintf(out, "\
	TCinitialize(model);\n\
	y = SSA(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, 0, %lf, %i, &sz, (void*)model, TCevents, TCtriggers, TCresponses);\n\
	if (!y) \
	{\n\
		tc_errorReport(\"Stochastic simulation failed! Try simulating for a short time to see what is going wrong. \");\n\
		free(model);\n\
		return;\n\
	}\n\
	data.rows = sz;\n\
	data.cols = 1+TCvars;\n\
	data.values = y;\n\
	mu = (double*)malloc((TCvars+TCreactions)*sizeof(double));\n\
	var = (double*)malloc((TCvars+TCreactions)*sizeof(double));\n\
	computeStats(mu,var,&data,(void*)model);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	A = tc_findItems(names);\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
	   sprintf(s, \"mean=%%.3lf \\nsd=%%.3lf\",mu[i],sqrt(var[i]));\n\
	   tc_displayText(tc_getItem(A,i),s);\n\
	}\n\
	tc_deleteItemsArray(&A);\n\
	names.length = TCreactions;\n\
	names.strings = TCreactionnames;\n\
	A = tc_findItems(names);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	for (i=0; i < TCreactions; ++i)\n\
	{\n\
	   sprintf(s, \"mean=%%.3lf \\nsd=%%.3lf\",mu[i+TCvars],sqrt(var[i+TCvars]));\n\
	   tc_displayText(tc_getItem(A,i),s);\n\
	}\n\
	tc_deleteItemsArray(&A);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	free(mu);\n\
	free(var);\n\
	if (%i)\n\
	{\n\
		y0 = getRatesFromSimulatedData(y, data.rows, TCvars , TCreactions , &(TCpropensity), (void*)model);\n\
		free(y);\n\
		y = y0;\n\
		names.length = TCreactions;\n\
		names.strings = TCreactionnames;\n\
	}\n\
	data.cols = 1+names.length;\n\
	data.values = y;\n\
	data.rownames = tc_createStringsArray(0);\n\
	data.colnames = tc_createStringsArray(names.length+1);\n\
	tc_setColumnName(data,0,\"time\\0\");\n\
	for(i=0; i<names.length; ++i) tc_setColumnName(data,1+i,tc_getString(names,i));\n\
//	tc_multiplot(2,1);\n\
	tc_plot(data,\"Stochastic Simulation\");\n\
//	tc_hist(data,\"Histogram\");\n\
	tc_deleteMatrix(&data);\n\
	free(model);\n",time,maxsz,rateplot);

	if (slider)
		fprintf(out, "    tc_deleteMatrix(&input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);

	if (slider)
	{
		tc_compileBuildLoadSliders("runssa.c -lssa\0","run\0","Gillespie algorithm\0",allParams);
		tc_deleteMatrix(&allParams);
	}
	else
		tc_compileBuildLoad("runssa.c -lssa\0","run\0","Gillespie algorithm\0");
	
	return;
}

void runCellSSA(tc_matrix input)
{
	double time = 50.0;
	int selection = 0, k;
	int numcells = 10;
	double replication = 0.05;
	double death = 0.0001;
	double mutants = 0.001;
	int gridsz = 100;
	FILE * out;
	tc_items A;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			time = tc_getMatrixValue(input,1,0);
		if (input.rows > 2)
			numcells = (int)tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			replication = tc_getMatrixValue(input,3,0);
		if (input.rows > 4)

			death = tc_getMatrixValue(input,4,0);
		if (input.rows > 5)
			mutants = (int)tc_getMatrixValue(input,5,0);
		if (input.rows > 6)
			gridsz = (int)tc_getMatrixValue(input,6,0);
	}

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (tc_getItem(A,0) == 0)
		{
			tc_deleteItemsArray(&A);
			tc_errorReport("No Model Selected\0");
			return;

		}
	}
	else
	{
		A = tc_allItems();
	}

	if (tc_getItem(A,0))
	{
		k = tc_writeModel( "cells_ssa", A );
		tc_deleteItemsArray(&A);
		if (!k)
		{
			tc_errorReport("No Model\0");
			return;
		}
	}
	else
	{
		tc_deleteItemsArray(&A);
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
				   tc_showProgress((int)(100 * time/%lf));\n\
				   _time0_ += %lf;\n\
				   }\n\
				   }\n\
				   \n\
				   \n\
				   TCAPIEXPORT void run() \n\
				   {\n\
				   initMTrand();\n\
				   TCinitialize();\n\
				   int sz = 0,i,j;\n\
				   double ** y = cells_ssa(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, %lf, %i, 0, %i, %lf, %lf, %lf);\n\
				   if (!y || !y[0] || !y[1]) \
				   {\n\
				   tc_errorReport(\"Simulation failed! Possible cause of failure: some values are becoming negative. Double check your model.\");\n\
				   return;\n\
				   }\n\
				   tc_matrix data1;\n\
				   tc_matrix data2;\n\
				   data2.rows = sz;\n\
				   data2.cols = 2;\n\
				   data2.values = y[0];\n\
				   data2.colnames.strings = malloc(2 * sizeof(char*));\n\
				   data2.colnames.length = 2;\n\
				   data2.colnames.strings[0] = \"time\";\n\
				   data2.colnames.strings[1] = \"cells\";\n\
				   data2.rownames.strings = 0;\n\
				   data1.rows = sz;\n\
				   char ** names = TCvarnames;\n\
				   data1.cols = 1+TCvars;\n\
				   data1.values = y[1];\n\
				   data1.rownames.strings = 0;\n\
				   data1.rownames.length = 0;\n\
				   data1.colnames.strings = malloc( (1+TCvars) * sizeof(char*) );\n\
				   data1.colnames.length = 1+TCvars;\n\
				   data1.colnames.strings[0] = \"time\\0\";\n\
				   for(i=0; i<TCvars; ++i) data1.colnames.strings[1+i] = names[i];\n\
				   tc_plot(data1,\"Multi-cell simulation\");\n\
				   tc_plot(data2,\"Cell growth\");\n\
				   free(data1.colnames.strings);\n\
				   free(data2.colnames.strings);\n\
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
	tc_matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "time", "max size", "plot", "show sliders", 0 };
	double values[] = { 0, 100, 100000, 0 , 1 };
	char * options1[] = { "Full model", "Selected only" };
	char * options2[] = { "Variables", "Rates"};
	char * options3[] = { "Yes", "No" };
	tc_strings a1 = { 2, options1 };
	tc_strings a2 = { 2, options2 };
	tc_strings a3 = { 2, options3 };

	m.rows = 5;
	if (MAC_WITHOUT_GCC)
	{
		m.rows = 2;
	}
	m.cols = 1;
	m.colnames.length = 1;
	m.colnames.strings = cols;
	m.rownames.length = 5;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Gillespie algorithm",&runSSA);
	if (!MAC_WITHOUT_GCC)
	{
		tc_addInputWindowOptions("Gillespie algorithm",0, 0, a1);
		tc_addInputWindowOptions("Gillespie algorithm",3, 0, a2);
		tc_addInputWindowOptions("Gillespie algorithm",4, 0, a3);
	}
}

void runLangevin(tc_matrix input)
{
	int i;
	double time = 50.0, dt = 0.1;
	int k, sz = 0, selection = 0, rateplot = 0;
	tc_items A,B;
	FILE * out;
	int slider = 1;
	char * runfuncInput = "tc_matrix input";
	char * runfunc = "";
	tc_matrix params, initVals, allParams, N;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			time = tc_getMatrixValue(input,1,0);
		if (input.rows > 2)
			dt = tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			rateplot = (int)tc_getMatrixValue(input,3,0);
		if (input.rows > 4)
			slider = (int)tc_getMatrixValue(input,4,0);
	}
	
	if (time < 0) 
	{
		tc_errorReport("select a positive time");
		return;
	}
	
	if (dt > time/2.0)
	{
		tc_errorReport("step size is too small");
	}
	
	if (slider)
		slider = 0;
	else
		slider = 1;
	
	if (selection > 0)
	{
		A = tc_selectedItems();
		if (tc_getItem(A,0) == 0)
		{
			tc_deleteItemsArray(&A);
			tc_errorReport("No Model Selected\0");
			return;

		}
	}
	else
	{
		A = tc_allItems();
	}

	if (slider)
	{
		params = tc_getParameters(A);
		N = tc_getStoichiometry(A);
		B = tc_findItems(N.rownames);
		tc_deleteMatrix(&N);
		initVals = tc_getInitialValues(B);

		allParams = tc_createMatrix(initVals.rows+params.rows,2);

		for (i=0; i < params.rows; ++i)
		{
			tc_setRowName(allParams,i, tc_getRowName(params,i));
			tc_setMatrixValue(allParams,i,0,tc_getMatrixValue(params,i,0)/10.0);
			tc_setMatrixValue(allParams,i,1, 2*tc_getMatrixValue(params,i,0) - tc_getMatrixValue(allParams,i,0));
		}
		for (i=0; i < initVals.rows; ++i)
		{
			tc_setRowName(allParams,i+params.rows, tc_getRowName(initVals,i));
			tc_setMatrixValue(allParams,i+params.rows,0,tc_getMatrixValue(initVals,i,0)/10.0);
			tc_setMatrixValue(allParams,i+params.rows,1, 2*tc_getMatrixValue(initVals,i,0) - tc_getMatrixValue(allParams,i+params.rows,0));
		}
		
		tc_deleteMatrix(&initVals);
		tc_deleteMatrix(&params);
		tc_deleteItemsArray(&B);
		runfunc = runfuncInput;
	}
	
	if (tc_getItem(A,0) != 0)
	{
		k = tc_writeModel( "runssa", A );
		tc_deleteItemsArray(&A);
		if (!k)
		{
			tc_errorReport("No Model\0");
			if (slider)
				tc_deleteMatrix(&allParams);
			return;
		}
	}
	else
	{
		tc_deleteItemsArray(&A);
		if (slider)
			tc_deleteMatrix(&allParams);
		tc_errorReport("No Model\0");
		return;
	}

	out = fopen("runssa.c","a");
	
	if (!out)
	{
		tc_deleteItemsArray(&A);
		if (slider)
			tc_deleteMatrix(&allParams);
		tc_errorReport("Cannot write to file runssa.c in user directory\0");
		return;
	}
	
	fprintf( out , "\
#include \"TC_api.h\"\n\
#include \"ssa.h\"\n\n\
static double _time0_ = 0.0;\n\
void ssaFunc(double time, double * u, double * rates, void * data)\n\
{\n\
	TCpropensity(time, u, rates, data);\n\
	if (time > _time0_)\n\
	{\n\
			tc_showProgress((int)(100 * time/%lf));\n\
			_time0_ += %lf;\n\
	}\n\
}\n\
\n\
static void computeStats(double * mu, double * var, tc_matrix * values, void * data)\n\
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
			u[j] = tc_getMatrixValue((*values),i,j+1);\n\
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
TCAPIEXPORT void run(%s) \n\
{\n\
	initMTrand();\n\
	int sz = 0,i,j;\n\
	double * y, *y0, * mu, * var;\n\
	tc_matrix data;\n\
	tc_items A;\n\
	tc_strings names;\n\
	char s[100];\n\
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	(*model) = TC_initial_model;\n",time,time/20.0,runfunc);

if (slider)
{
	for (i=0; i < allParams.rows; ++i)
		fprintf(out, "    model->%s = tc_getMatrixValue(input,%i,0);\n",tc_getRowName(allParams,i),i);
}

fprintf(out, "\
	TCinitialize(model);\n\
s	y = Langevin(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, %lf, %lf, (void*)model, TCevents, TCtriggers, TCresponses);\n\
	if (!y) \
	{\n\
		tc_errorReport(\"Stochastic simulation failed! Try simulating for a short time to see what is going wrong. \");\n\
		free(model);\n\
		return;\n\
	}\n\
	sz = %i;\n\
	data.rows = sz;\n\
	data.cols = 1+TCvars;\n\
	data.values = y;\n\
	mu = (double*)malloc((TCvars+TCreactions)*sizeof(double));\n\
	var = (double*)malloc((TCvars+TCreactions)*sizeof(double));\n\
	computeStats(mu,var,&data,(void*)model);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	A = tc_findItems(names);\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
	   sprintf(s, \"mean=%%.3lf \\nsd=%%.3lf\",mu[i],sqrt(var[i]));\n\
	   tc_displayText(tc_getItem(A,i),s);\n\
	}\n\
	tc_deleteItemsArray(&A);\n\
	names.length = TCreactions;\n\
	names.strings = TCreactionnames;\n\
	A = tc_findItems(names);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	for (i=0; i < TCreactions; ++i)\n\
	{\n\
	   sprintf(s, \"mean=%%.3lf \\nsd=%%.3lf\",mu[i+TCvars],sqrt(var[i+TCvars]));\n\
	   tc_displayText(tc_getItem(A,i),s);\n\
	}\n\
	tc_deleteItemsArray(&A);\n\
	free(mu);\n\
	free(var);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	if (%i)\n\
	{\n\
		y0 = getRatesFromSimulatedData(y, data.rows, TCvars , TCreactions , &(TCpropensity), (void*)model);\n\
		free(y);\n\
		y = y0;\n\
		names.length = TCreactions;\n\
		names.strings = TCreactionnames;\n\
	}\n\
	data.cols = 1+names.length;\n\
	data.values = y;\n\
	data.rownames = tc_createStringsArray(0);\n\
	data.colnames = tc_createStringsArray(names.length+1);\n\
	tc_setColumnName(data,0,\"time\\0\");\n\
	for(i=0; i<names.length; ++i) tc_setColumnName(data,1+i,tc_getString(names,i));\n\
//	tc_multiplot(2,1);\n\
	tc_plot(data,\"Stochastic Simulation\");\n\
//	tc_hist(data,\"Histogram\");\n\
	tc_deleteMatrix(&data);\n\
	free(model);\n",time,dt,(int)(time/dt),rateplot);

	if (slider)
		fprintf(out, "    tc_deleteMatrix(&input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);

	if (slider)
	{
		tc_compileBuildLoadSliders("runssa.c -lssa\0","run\0","Gillespie algorithm\0",allParams);
		tc_deleteMatrix(&allParams);
	}
	else
		tc_compileBuildLoad("runssa.c -lssa\0","run\0","Gillespie algorithm\0");
	
	return;
}

void setupCellSSA()
{
	tc_matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "time", "num. cells", "growth rate", "death rate", "mutation rate", "num. points" , 0 };
	double values[] = { 0, 100, 100, 0.05, 0.001, 0.001, 100 };
	char * options1[] = { "Full model", "Selected only" }; 
	tc_strings a1 = {2, options1};
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	m.rows = m.rownames.length = 7;
	m.cols = m.colnames.length = 1;

	tc_createInputWindow(m,"Multi-cell stochastic simulation",&runCellSSA);
	tc_addInputWindowOptions("Multi-cell stochastic simulation",0, 0,  a1);
}

void setupLangevin()
{
	tc_matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "time", "step size", "plot", "show sliders", 0 };
	double values[] = { 0, 100, 0.1, 0 , 1 };
	char * options1[] = { "Full model", "Selected only" };
	char * options2[] = { "Variables", "Rates"};
	char * options3[] = { "Yes", "No" };
	tc_strings a1 = {2, options1};
	tc_strings a2 = {2, options2};
	tc_strings a3 = {2, options3};

	m.rows = 5;
	m.cols = 1;
	m.colnames.length = 1;
	m.colnames.strings = cols;
	m.rownames.length = 5;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Langevin algorithm",&runLangevin);
	tc_addInputWindowOptions("Langevin algorithm",0, 0, a1);
	tc_addInputWindowOptions("Langevin algorithm",3, 0, a2);
	tc_addInputWindowOptions("Langevin algorithm",4, 0, a3);
}

TCAPIEXPORT void tc_main()
{
	MAC_WITHOUT_GCC = (MAC_WITHOUT_GCC * tc_isMac());
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setupSSA, "Stochastic simulation (Discrete)", "uses custom Gillespie algorithm (compiles to C program)", "Simulate", "stochastic.png", "", 1, 0, 0);
	
	if (!MAC_WITHOUT_GCC)
		tc_addFunction(&setupLangevin, "Stochastic simulation (Continuous)", "uses Langevin method (compiles to C program)", "Simulate", "stochastic.png", "", 1, 0, 0);
	
	//tc_addFunction(&setupCellSSA, "Multi-cell stochastic simulation", "uses custom Gillespie algorithm (compiles to C program)", "Simulate", "cells.png", "", 1, 0, 0);
}
