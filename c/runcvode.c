/****************************************************************************
**
** This file creates an input window which allows users to run the runcvode.c code
** with specific inputs for start time, end time, step size, and x-axis
** and...
** gets information from TinkerCell, generates a differential equation model, runs
** the simulation, and outputs the data to TinkerCell
**
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "TC_api.h"

void run(tc_matrix input);
void setup();

TCAPIEXPORT void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup, "Deterministic simulation", "uses Sundials library (compiles to C program)", "Simulate", "cvode.png", "", 1, 0, 1);
	//tc_addFunction(&getSS, "Get steady state", "Bring the system to nearest steady state and bring Jacobian", "Steady state", "cvode.png", "", 1, 0, 0);
}

void setup()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "model", "time", "step size", "plot", "update model", "use sliders", 0 };
	double values[] = { 0, 100, 0.1, 0, 1, 1 };
	char * options1[] = { "Full model", "Selected only" };
	char * options2[] = { "Variables", "Rates" };
	char * options3[] = { "Yes", "No"};
	FILE * file;
	tc_strings a1 = {2,options1};
	tc_strings a2 = {2,options2};
	tc_strings a3 = {2,options3};

	m.rows = m.rownames.length = 6;
	if (tc_isMac())
	{
		m.rows = m.rownames.length = 3;
		options1[1] = "Not available in Mac";
	}
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Deterministic simulation (CVODE)",&run);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",0, 0,  a1);		
	if (!tc_isMac())
	{
		tc_addInputWindowOptions("Deterministic Simulation (CVODE)",3, 0,  a2);
		tc_addInputWindowOptions("Deterministic Simulation (CVODE)",4, 0,  a3);
		tc_addInputWindowOptions("Deterministic Simulation (CVODE)",5, 0,  a3);
	}
	return;
}

void run(tc_matrix input)
{
	tc_items A,B;
	FILE * out;
	double start = 0.0, end = 50.0;
	double dt = 0.1;
	int selection = 0;
	int rateplot = 0;
	int slider = 1;
	int i=0, sz = 0, k = 0, update = 1;
	char * runfuncInput = "tc_matrix input";
	char * runfunc = "";
	tc_matrix params, initVals, allParams, N;
	
	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			end = tc_getMatrixValue(input,1,0);
		if (input.rows > 2)
			dt = tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			rateplot = (int)tc_getMatrixValue(input,3,0);
		if (input.rows > 4)
			update = (int)tc_getMatrixValue(input,4,0);
		if (input.rows > 5)
			slider = (int)tc_getMatrixValue(input,5,0);
	}
	
	if (end < 0) 
	{
		tc_errorReport("select a positive time");
		return;
	}
	
	if (dt > end/2.0) 
	{
		tc_errorReport("step size is too small");
		return;
	}
	
	if (tc_isMac())
	{
		N = tc_simulateODE(end,dt);
		tc_plot(N,"Time Course Simulation");
		tc_deleteMatrix(&N);
		return;
	}
	
	if (slider)
		slider = 0;
	else
		slider = 1;
		
	if (update)
		update = 0;
	else
		update = 1;

	sz = (int)((end - start) / dt);

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
	
	if (tc_getItem(A,0))
	{
		k = tc_writeModel( "ode", A );
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
	
	out = fopen("ode.c","a");
	
	if (!out)
	{
		tc_deleteItemsArray(&A);
		if (slider)
			tc_deleteMatrix(&allParams);
		tc_errorReport("Cannot write to file ode.c in user directory\0");
		return;
	}
	

	fprintf( out , "\
#include \"TC_api.h\"\n\
#include \"cvodesim.h\"\n\
#include \"ssa.h\"\n\n\
static double _time0_ = 0.0;\n\
static double * rates = 0;\n\
#define valueAt(array, N, i, j) ( array[ (i)*(N) + (j) ] )\n\
void odeFunc( double time, double * u, double * du, void * udata )\n\
{\n\
	int i,j;\n\
	TCpropensity(time, u, rates, udata);\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
		du[i] = 0;\n\
		for (j=0; j < TCreactions; ++j)\n\
		{\n\
			if (valueAt(TCstoic,TCreactions,i,j) != 0)\n\
			du[i] += rates[j]*valueAt(TCstoic,TCreactions,i,j);\n\
		}\n\
	}\n\
	if (time > _time0_)\n\
	{\n\
		tc_showProgress((int)(100 * time/%lf));\n\
		_time0_ += %lf;\n\
	}\n\
}\n\
\n\
\n\
TCAPIEXPORT void run(%s) \n\
{\n\
	int i,j;\n\
	double mx=0;\n\
	long x;\n\
	tc_items A;\n\
	tc_matrix data, ss1, ss2;\n\
	tc_strings names;\n\
	double * y, *y0;\n\
	rates = malloc(TCreactions * sizeof(double));\n\
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	(*model) = TC_initial_model;\n\
	\n", (end-start), (end-start)/20.0, runfunc);

if (slider)
{
	for (i=0; i < allParams.rows; ++i)
		fprintf(out, "    model->%s = tc_getMatrixValue(input,%i,0);\n",tc_getRowName(allParams,i),i);
}

fprintf( out , "\
    TCinitialize(model);\n\
	y = ODEsim(TCvars, TCinit, &(odeFunc), %lf, %lf, %lf, (void*)model, TCevents, TCtriggers, TCresponses);\n\
	free(rates);\n\
	if (!y) \
	{\n\
		tc_errorReport(\"Integration failed! Current model is too difficult to solve. Try changing parameters or simulating for a short time.\");\n\
		free(model);\n\
		return;\n\
	}\n\
	data.rows = %i;\n\
	data.cols = TCvars;\n\
	data.values = y;\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	A = tc_findItems(names);\n\
	ss1 = ss2 = tc_createMatrix(0,0);\n\
	ss1.values = TCgetVars(model);\n\
	ss1.rows = TCvars;\n\
	ss1.cols = 1;\n\
	ss2.values = TCgetRates(model);\n\
	ss2.rows = TCreactions;\n\
	ss2.cols = 1;\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
	   x = tc_getItem(A,i);\n\
	   tc_displayNumber(x,tc_getMatrixValue(ss1,i,0));\n\
	}\n\
	if (%i)\n\
	{\n\
	   tc_setInitialValues(A,ss1);\n\
	}\n\
	tc_deleteItemsArray(&A);\n\
	names.length = TCreactions;\n\
	names.strings = TCreactionnames;\n\
	A = tc_findItems(names);\n\
	for (i=0; i < TCreactions; ++i)\n\
	{\n\
	   x = tc_getItem(A,i);\n\
	   tc_displayNumber(x,tc_getMatrixValue(ss2,i,0));\n\
	}\n\
	tc_deleteItemsArray(&A);\n\
	tc_deleteMatrix(&ss1);\n\
	tc_deleteMatrix(&ss2);\n\
	names.length = TCvars;\n\
	names.strings = TCvarnames;\n\
	if (%i)\n\
	{\n\
		y0 = getRatesFromSimulatedData(y, data.rows, TCvars , TCreactions , &(TCpropensity), (void*)model);\n\
		free(y);\n\
		y = y0;\n\
		TCvars = TCreactions;\n\
		names.length = TCvars;\n\
		names.strings = TCreactionnames;\n\
	}\n\
	data.cols = 1+TCvars;\n\
	data.values = y;\n\
	data.rownames = tc_createStringsArray(0);\n\
	data.colnames = tc_createStringsArray(data.cols);\n\
	tc_setColumnName(data,0,\"time\\0\");\n\
	for (i=0; i<TCvars; ++i)\n\
	{\n\
		tc_setColumnName(data,1+i,tc_getString(names,i));\n\
	}\n\
	tc_plot(data,\"Time Course Simulation\");\n\
	tc_deleteMatrix(&data);\n\
	free(model);\n", start, end, dt, sz, update, rateplot);
	

	if (slider)
		fprintf(out, "    tc_deleteMatrix(&input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("ode.c -lode -lssa\0","run\0","Deterministic simulation\0",allParams);
		tc_deleteMatrix(&allParams);
	}

	else
		tc_compileBuildLoad("ode.c -lode -lssa\0","run\0","Deterministic simulation\0");
	
	
	return;

}

