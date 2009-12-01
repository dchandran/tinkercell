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
void getSS();

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup, "Deterministic simulation", "uses Sundials library (compiles to C program)", "Simulate", "Plugins/c/cvode.PNG", "", 1, 0, 1);
	tc_addFunction(&getSS, "Get steady state", "Bring the system to nearest steady state and bring Jacobian", "Steady state", "Plugins/c/cvode.PNG", "", 1, 0, 0);
}

void getSS()
{
    FILE * out;
    int k;
    Array A = tc_allItems();

	if (A[0] != 0)
	{
		k = tc_writeModel( "getss", A );
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

	out = fopen("getss.c","a");

	fprintf( out, "\n\#include \"TC_api.h\"\n#include \"cvodesim.h\"\n\n\
    void run()\n\
    {\n\
         int i;\n\
         char c[100];\n\
         Matrix J, ss;\n\
         Array A;\n\
         double * y, *eigim, *eigr;\n\
         TCinitialize();\n\
         J.rownames = J.colnames = TCvarnames;\n\
         J.rows = J.cols = TCvars;\n\
         y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit,0,1E-4,100000.0,10);\n\
         if (y)\n\
         {\n\
            ss.rownames = 0;\n\
            ss.colnames = 0;\n\
            ss.rows = TCvars;\n\
            ss.cols = 1;\n\
            ss.values = y;\n\
            A = tc_findItems(TCvarnames);\n\
            tc_setInitialValues(A,ss);\n\
            eigr = (double*)malloc(TCvars*sizeof(double));\n\
            eigim = (double*)malloc(TCvars*sizeof(double));\n\
            J.values = jacobian2(TCvars,TCreactions,TCstoic, &(TCpropensity), y, 0, eigr, eigim);\n\
            tc_print(\"Jacobian = \");\n\
            tc_printTable(J);\n\
            tc_print(\"eigenvalues = [ \");\n\
            for (i=0; i < TCvars; ++i)\n\
            {\n\
                if (i < (TCvars-1))\n\
                    sprintf(c,\"%%lf +/- %%lf i, \\0\",eigr[i],eigim[i]);\n\
                else\n\
                    sprintf(c,\"%%lf +/- %%lf i] \\n\",eigr[i],eigim[i]);\n\
                tc_print(c);\n\
            }\n\
            tc_print(\"\\n\");\n\
            free(J.values);\n\
            free(eigr);\n\
            free(eigim);\n\
            free(y);\n\
         }\n\
         else\n\
         {\n\
             tc_print(\"No steady state found\");\n\
         }\n}\n");

	fclose(out);

	tc_compileBuildLoad("getss.c -lodesim\0","run\0","Get steady state\0");
	return;
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
	Array A;
	FILE * out;
	double start = 0.0, end = 50.0;
	double dt = 0.1;
	int xaxis = 0;
	int selection = 0;
	int rateplot = 0;
	int sz = 0, k = 0, update = 0;

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
					void run() \n\
					{\n\
						int i,j;\n\
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
						if (%i)\n\
						{\n\
						   ss.rownames = 0;\n\
				           ss.colnames = 0;\n\
				           ss.values = malloc(TCvars * sizeof(double));\n\
				           ss.rows = TCvars;\n\
				           ss.cols = 1;\n\
						   for (i=0; i < TCvars; ++i)\n\
				           {\n\
				              valueAt(ss,i,0) = valueAt(data,(data.rows-1),i+1);\n\
				           }\n\
						   A = tc_findItems(TCvarnames);\n\
						   tc_setInitialValues(A,ss);\n\
				           free(A);\n\
						   free(ss.values);\n\
						}\n\
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
						for (i=0; i<TCvars; ++i) data.colnames[1+i] = names[i];\n\
						tc_plot(data,%i,\"Time Course Simulation\",0);\n\
						free(data.colnames);  free(y);\n\
						return;\n}\n",
						(end-start), (end-start)/20.0, start, end, dt, sz, update, rateplot, xaxis);
	fclose(out);

	tc_compileBuildLoad("ode.c -lodesim -lssa\0","run\0","Deterministic simulation\0");
	return;

}




