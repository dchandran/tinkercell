/****************************************************************************
**
** This file creates an input window which allows users to run the runcvode.c code
** with specific inputs for start time, end time, step size, and x-axis
** and...
** gets information from TinkerCell, generates a differential equation model, runs
** the simulation, and outputs the data to TinkerCell
**
****************************************************************************/

#include "TC_api.h"

void run(Matrix input);
void setup();

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup, "Deterministic simulation", "uses Sundials library (compiles to C program)", "Simulate", "Plugins/c/cvode.png", "", 1, 0, 1);
	//tc_addFunction(&getSS, "Get steady state", "Bring the system to nearest steady state and bring Jacobian", "Steady state", "Plugins/c/cvode.png", "", 1, 0, 0);
}

void setup()
{
	Matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "model", "time", "step size", "plot", "update model", "use sliders", 0 };
	double values[] = { 0, 100, 0.1, 0, 1, 1 };
	char * options1[] = { "Full model", "Selected only" };
	char * options2[] = { "Variables", "Rates" };
	char * options3[] = { "Yes", "No"};
	FILE * file;

	m.rows = m.rownames.length = 6;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Deterministic simulation (CVODE)",&run);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",0, 0,  (ArrayOfStrings){2,options1});
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",3, 0,  (ArrayOfStrings){2,options2});
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",4, 0,  (ArrayOfStrings){2,options3});
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",5, 0,  (ArrayOfStrings){2,options3});

	return;
}

void run(Matrix input)
{
	ArrayOfItems A,B;
	FILE * out;
	double start = 0.0, end = 50.0;
	double dt = 0.1;
	int xaxis = 0;
	int selection = 0;
	int rateplot = 0;
	int slider = 1;
	int i=0, sz = 0, k = 0, update = 1;
	char * runfuncInput = "Matrix input";
	char * runfunc = "";
	Matrix params, initVals, allParams, N;
	
	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)getValue(input,0,0);
		if (input.rows > 1)
			end = getValue(input,1,0);
		if (input.rows > 2)
			dt = getValue(input,2,0);
		if (input.rows > 3)
			rateplot = (int)getValue(input,3,0);
		if (input.rows > 4)
			update = (int)getValue(input,4,0);
		if (input.rows > 5)
			slider = (int)getValue(input,5,0);
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
		if (nthItem(A,0) == 0)
		{
			deleteArrayOfItems(&A);
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
		params = tc_getModelParameters(A);
		N = tc_getStoichiometry(A);
		B = tc_findItems(N.rownames);
		deleteMatrix(&N);
		initVals = tc_getInitialValues(B);

		allParams = newMatrix(initVals.rows+params.rows,2);

		for (i=0; i < params.rows; ++i)
		{
			setRowName(allParams,i, getRowName(params,i));
			setValue(allParams,i,0,getValue(params,i,0)/10.0);
			setValue(allParams,i,1, 2*getValue(params,i,0) - getValue(allParams,i,0));
		}
		for (i=0; i < initVals.rows; ++i)
		{
			setRowName(allParams,i+params.rows, getRowName(initVals,i));
			setValue(allParams,i+params.rows,0,getValue(initVals,i,0)/10.0);
			setValue(allParams,i+params.rows,1, 2*getValue(initVals,i,0) - getValue(allParams,i+params.rows,0));
		}
		
		deleteMatrix(&initVals);
		deleteMatrix(&params);
		deleteArrayOfItems(&B);
		runfunc = runfuncInput;
	}
	
	if (nthItem(A,0))
	{
		k = tc_writeModel( "ode", A );
		deleteArrayOfItems(&A);
		if (!k)
		{
			tc_errorReport("No Model\0");
			if (slider)
				deleteMatrix(&allParams);
			return;
		}
	}
	else
	{
		deleteArrayOfItems(&A);
		if (slider)
			deleteMatrix(&allParams);
		tc_errorReport("No Model\0");
		return;
	}
	
	out = fopen("ode.c","a");
	
	if (!out)
	{
		deleteArrayOfItems(&A);
		if (slider)
			deleteMatrix(&allParams);
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
void run(%s) \n\
{\n\
	int i,j;\n\
	double mx=0;\n\
	void * x;\n\
	ArrayOfItems A;\n\
	Matrix data, ss1, ss2;\n\
	ArrayOfStrings names;\n\
	double * y, *y0;\n\
	rates = malloc(TCreactions * sizeof(double));\n\
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	(*model) = TC_initial_model;\n\
	\n", (end-start), (end-start)/20.0, runfunc);

if (slider)
{
	for (i=0; i < allParams.rows; ++i)
		fprintf(out, "    model->%s = getValue(input,%i,0);\n",getRowName(allParams,i),i);
}

fprintf( out , "\
    TCinitialize(model);\n\
	y = ODEsim(TCvars, TCinit, &(odeFunc), %lf, %lf, %lf, (void*)model);\n\
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
	ss1 = ss2 = newMatrix(0,0);\n\
	ss1.values = TCgetVars(model);\n\
	ss1.rows = TCvars;\n\
	ss1.cols = 1;\n\
	ss2.values = TCgetRates(model);\n\
	ss2.rows = TCreactions;\n\
	ss2.cols = 1;\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
	   x = nthItem(A,i);\n\
	   tc_displayNumber(x,getValue(ss1,i,0));\n\
	}\n\
	if (%i)\n\
	{\n\
	   tc_setInitialValues(A,ss1);\n\
	}\n\
	deleteArrayOfItems(&A);\n\
	names.length = TCreactions;\n\
	names.strings = TCreactionnames;\n\
	A = tc_findItems(names);\n\
	for (i=0; i < TCreactions; ++i)\n\
	{\n\
	   x = nthItem(A,i);\n\
	   tc_displayNumber(x,getValue(ss2,i,0));\n\
	}\n\
	deleteArrayOfItems(&A);\n\
	deleteMatrix(&ss1);\n\
	deleteMatrix(&ss2);\n\
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
	data.rownames = newArrayOfStrings(0);\n\
	data.colnames = newArrayOfStrings(data.cols);\n\
	setColumnName(data,0,\"time\\0\");\n\
	for (i=0; i<TCvars; ++i)\n\
	{\n\
		setColumnName(data,1+i,nthString(names,i));\n\
	}\n\
	tc_plot(data,%i,\"Time Course Simulation\",0);\n\
	deleteMatrix(&data);\n\
	free(model);\n", start, end, dt, sz, update, rateplot, xaxis);
	

	if (slider)
		fprintf(out, "    deleteMatrix(&input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("ode.c -lodesim -lssa\0","run\0","Deterministic simulation\0",allParams);
		deleteMatrix(&allParams);
	}
	else
		tc_compileBuildLoad("ode.c -lodesim -lssa\0","run\0","Deterministic simulation\0");
	
	
	return;

}




