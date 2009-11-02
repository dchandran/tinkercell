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
	int maxsz = 100000;
	double time = 50.0;
	int xaxis = 0, k, sz = 0, selection = 0, rateplot = 0;
	Array A;
	FILE * out;
	char* appDir;

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

	if (A[0] != 0)
	{
		k = tc_writeModel( "runssa", A );
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

	out = fopen("runssa.c","a");

	fprintf( out , "#include \"TC_api.h\"\n#include \"ssa.h\"\n\n\
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
					\n\
					void run() \n\
					{\n\
						initMTrand();\n\
						TCinitialize();\n\
						int sz = 0;\n\
						double * y = SSA(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, 0, %lf, %i, &sz, 0);\n\
						if (!y) \
						{\n\
							tc_errorReport(\"SSA failed! Possible cause of failure: some values are becoming negative. Double check your model.\");\n\
							return;\n\
						}\n\
						Matrix data;\n\
						data.rows = sz;\n\
						char ** names = TCvarnames;\n\
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
						int i,j;\n\
						for(i=0; i<TCvars; ++i) data.colnames[1+i] = names[i];\n\
						FILE * out = fopen(\"ssa.tab\",\"w\");\n\
						for (i=0; i < data.cols; ++i)\n\
						{\n\
							fprintf( out, data.colnames[i] );\n\
							if (i < (data.cols-1)) fprintf( out, \"\\t\" );\n\
						}\n\
						fprintf( out, \"\\n\");\n\
						for (i=0; i < data.rows; ++i)\n\
						{\n\
						for (j=0; j < data.cols; ++j)\n\
						{\n\
							if (j==0)\n\
								fprintf( out, \"%%lf\", valueAt(data,i,j) );\n\
							else   \n\
								fprintf( out, \"\\t%%lf\", valueAt(data,i,j) );\n\
						}\n\
						fprintf( out, \"\\n\");\n\
					}\n\
					fclose(out);\n\
					if (data.rows > 10000)\n\
						tc_print(\"Warning: plot is large. It can slow down TinkerCell.\\noutput written to Tinkercell/ssa.tab in your home directory\");\n\
					else\n\
						tc_print(\"output written to Tinkercell/ssa.tab in your home directory\");\n\
					tc_plot(data,%i,\"Stochastic Simulation\",0);\n\
					free(data.colnames);\n\
					free(y);\n\
					return;\n}\n",time,time/20.0,time,maxsz,rateplot,xaxis);

	fclose(out);

	tc_compileBuildLoad("runssa.c -lssa\0","run\0","Gillespie algorithm\0");
	
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

int functionMissing()
{
	if (!tc_addFunction || 
		!tc_createInputWindow ||
		!tc_addInputWindowOptions ||
		!tc_selectedItems ||
		!tc_allItems ||
		!tc_errorReport ||
		!tc_getModelParameters ||
		!tc_getNames || 
		!tc_itemsOfFamilyFrom ||
		!tc_getFromList ||
		!tc_writeModel ||
		!tc_print ||
		!tc_getFromList ||
		!tc_compileBuildLoad ||
		!tc_plot ||
		!tc_isWindows)

		return 1;


	return 0;
}

void tc_main()
{
	if (functionMissing()) return;
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&setupSSA, "Discreet stochastic simulation", "uses custom Gillespie algorithm (compiles to C program)", "Simulate", "Plugins/c/stochastic.PNG", "", 1, 0, 0);
	tc_addFunction(&setupCellSSA, "Multi-cell stochastic simulation", "uses custom Gillespie algorithm (compiles to C program)", "Simulate", "Plugins/c/cells.PNG", "", 1, 0, 0);
}
