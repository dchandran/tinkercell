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
	tc_addFunction(&setup, "Deterministic simulation", "uses Sundials library (compiles to C program)", "Simulate", "Plugins/c/cvode.PNG", "", 1, 0, 1);
	//tc_addFunction(&getSS, "Get steady state", "Bring the system to nearest steady state and bring Jacobian", "Steady state", "Plugins/c/cvode.PNG", "", 1, 0, 0);
}

void setup()
{
	Matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "model", "time", "step size", "plot", "update model", 0 };
	double values[] = { 0, 100, 0.1, 0, 0 };
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important
	char * options2[] = { "Variables", "Rates", 0 };
	char * options3[] = { "No", "Yes", 0 };
	FILE * file;

	m.rows = 5;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	tc_createInputWindow(m,"Deterministic simulation (CVODE)",&run);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",0, 0,  options1);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",3, 0,  options2);
	tc_addInputWindowOptions("Deterministic Simulation (CVODE)",4, 0,  options3);

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
	int i=0, sz = 0, k = 0, update = 0;
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
	}

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
	allParams.cols = 3;
	allParams.rownames = (char**)malloc((initVals.rows+params.rows+1)*sizeof(char*));
	allParams.values = (double*)malloc(3*allParams.rows*sizeof(double));
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

	fprintf( out , "\
				   #include \"TC_api.h\"\n#include \"cvodesim.h\"\n\n\
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
					void run(Matrix input) \n\
					{\n\
						int i,j;\n\
						double mx=0;\n\
						OBJ x;\n\
					    Array A;\n\
						Matrix data,ss;\n\
						char ** names;\n\
						TCinitialize();\n\
						rates = malloc(TCreactions * sizeof(double));\n\
						double * y = ODEsim(TCvars, TCinit, &(odeFunc), %lf, %lf, %lf, 0);\n\
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
						ss.rownames = 0;\n\
			            ss.colnames = 0;\n\
			            ss.values = malloc(TCvars * sizeof(double));\n\
			            ss.rows = TCvars;\n\
			            ss.cols = 1;\n\
						for (i=0; i < TCvars; ++i)\n\
						{\n\
							if (i == 0 || valueAt(data,(data.rows-1),i+1) > mx)\n\
								mx = valueAt(data,(data.rows-1),i+1);\n\
						}\n\
					    for (i=0; i < TCvars; ++i)\n\
			            {\n\
			               valueAt(ss,i,0) = valueAt(data,(data.rows-1),i+1);\n\
						   x = tc_find(names[i]);\n\
						   tc_displayNumber(x,valueAt(ss,i,0));\n\
						   if (mx > 0) tc_setColor(x,200*(1.0 - valueAt(ss,i,0)/mx),200*(1.0 - valueAt(ss,i,0)/mx),200*(1.0 - valueAt(ss,i,0)/mx),0); \n\
			            }\n\
						if (%i)\n\
						{\n\
						   tc_setInitialValues(A,ss);\n\
						}\n\
						free(A);\n\
						free(ss.values);\n\
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
						free(y);\n\
						return;\n}\n",
						(end-start), (end-start)/20.0, start, end, dt, sz, update, rateplot, xaxis);
	fclose(out);
	
	tc_compileBuildLoadSliders("ode.c -lodesim -lssa\0","run\0","Deterministic simulation\0",allParams);
	
	TCFreeMatrix(allParams);
	return;

}




