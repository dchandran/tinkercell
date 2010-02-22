/****************************************************************************
**
**	This file creates an input window which allows users to run the runcvode.c code
**	with specific inputs for start time, end time, step size, and x-axis
** And...
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
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important
	char * options2[] = { "Variables", "Rates", 0 };
	char * options3[] = { "Yes", "No", 0 };
	FILE * file;

	m.rows = 6;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	tc_createInputWindow(m,"Deterministic simulation (CVODE)",&run);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",0, 0,  options1);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",3, 0,  options2);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",4, 0,  options3);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",5, 0,  options3);

	return;
}

void run(Matrix input)
{
	Array A, B;
	FILE * out;
	double start = 0.0, end = 50.0;
	double dt = 0.1;
	int xaxis = 0;
	int selection = 0;
	int rateplot = 0;
	int slider = 1;
	int i=0, sz = 0, k = 0, update = 0;
	char * runfuncInput = "Matrix input";
	char * runfunc = "";
	Matrix params, initVals, allParams;
	
	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)valueAt(input,0,0);
		if (input.rows > 1)
			end = valueAt(input,1,0);
		if (input.rows > 2)
			dt = valueAt(input,2,0);
		if (input.rows > 3)
			rateplot = (int)valueAt(input,3,0);
		if (input.rows > 4)
			update = (int)valueAt(input,4,0);
		if (input.rows > 5)
			slider = (int)valueAt(input,5,0);
	}
	
	if (slider)
		slider = 0;
	else
		slider = 1;
	
	if (slider)
		runfunc = runfuncInput;

	sz = (int)((end - start) / dt);

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (A[0] == 0)
		{
			TCFreeArray(A);
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
		k = tc_writeModel( "ode", A );
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
		tc_errorReport("No Model\0");
		return;
	}
	
	out = fopen("ode.c","a");
	
	if (!out)
	{
		TCFreeArray(A);
		TCFreeMatrix(allParams);
		tc_errorReport("Cannot write to file ode.c in user directory\0");
		return;
	}
	
	fprintf(out, "void assignInputs(double * k, TCmodel * model)\n{\n" );
	
	for (i=0; i < allParams.rows; ++i)
	{
		fprintf(out, "    model->%s = k[%i];\n", allParams.rownames[i], i );
	}
	
	fprintf( out , "}\n\
#include \"TC_api.h\"\n\
#include \"cvodesim.h\"\n\n\
static double _time0_ = 0.0;\n\
static double * rates = 0;\n\
void odeFunc( double time, double * u, double * du, void * udata )\n\
{\n\
	int i,j;\n\
	TCpropensity(time, u, rates, udata);\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
		du[i] = 0;\n\
		for (j=0; j < TCreactions; ++j)\n\
		{\n\
			if (getValue(TCstoic,TCreactions,i,j) != 0)\n\
			du[i] += rates[j]*getValue(TCstoic,TCreactions,i,j);\n\
		}\n\
	}\n\
	if (time > _time0_)\n\
	{\n\
		tc_showProgress(\"Deterministic simulation\",(int)(100 * time/%lf));\n\
		_time0_ += %lf;\n\
	}\n\
}\n\
\n\
\n\
void run(%s) \n\
{\n\
	int i,j;\n\
	double mx=0;\n\
	OBJ x;\n\
	Array A;\n\
	Matrix data, ss1, ss2;\n\
	char ** names;\n\
	rates = malloc(TCreactions * sizeof(double));\n\
	TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	(*model) = TC_initial_model;\n\
	\n", (end-start), (end-start)/20.0, runfunc);

if (slider)
	fprintf(out, "    if (input.rows > TCparams)\n    assignInputs(input.values,model);\n");

fprintf( out , "\
    TCinitialize(model);\n\
	double * y = ODEsim(TCvars, TCinit, &(odeFunc), %lf, %lf, %lf, (void*)model);\n\
	free(rates);\n\
	if (!y) \
	{\n\
		tc_errorReport(\"Numerical integration (CVODE) failed! Some values might be reaching Inf. Double check your model.\");\n\
		return;\n\
	}\n\
	data.rows = %i;\n\
	data.cols = (TCvars+1);\n\
	data.values = y;\n\
	names = TCvarnames;\n\
	A = tc_findItems(TCvarnames);\n\
	ss1.rownames = 0;\n\
	ss1.colnames = 0;\n\
	ss1.values = TCgetVars(model);\n\
	ss1.rows = TCvars;\n\
	ss1.cols = 1;\n\
	ss2.rownames = 0;\n\
	ss2.colnames = 0;\n\
	ss2.values = TCgetRates(model);\n\
	ss2.rows = TCreactions;\n\
	ss2.cols = 1;\n\
	for (i=0; i < TCvars; ++i)\n\
	{\n\
	   x = A[i];\n\
	   tc_displayNumber(x,valueAt(ss1,i,0));\n\
	}\n\
	if (%i)\n\
	{\n\
	   tc_setInitialValues(A,ss1);\n\
	}\n\
	free(A);\n\
	A = tc_findItems(TCreactionnames);\n\
	for (i=0; i < TCreactions; ++i)\n\
	{\n\
	   x = A[i];\n\
	   tc_displayNumber(x,valueAt(ss2,i,0));\n\
	}\n\
	free(A);\n\
	free(ss1.values);\n\
	free(ss2.values);\n\
	free(model);\n\
	if (%i)\n\
	{\n\
		double * y0 = getRatesFromSimulatedData(y, data.rows, TCvars , TCreactions , 1 , &(TCpropensity), 0);\n\
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
	for (i=0; i<TCvars; ++i)\n\
	{\n\
		data.colnames[1+i] = names[i];\n\
	}\n\
	tc_plot(data,%i,\"Time Course Simulation\",0);\n\
	free(data.colnames);\n\
	free(y);", start, end, dt, sz, update, rateplot, xaxis);
	

	if (slider)
		fprintf(out, "    TCFreeMatrix(input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
		tc_compileBuildLoadSliders("ode.c -lodesim -lssa\0","run\0","Deterministic simulation\0",allParams);
	else
		tc_compileBuildLoad("ode.c -lodesim -lssa\0","run\0","Deterministic simulation\0");
	
	TCFreeMatrix(allParams);
	return;

}




