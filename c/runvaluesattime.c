/****************************************************************************

Asks user for a parameter or variable name (string), and then generates a code that generates
the steady state table by changing this value.

****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "TC_api.h"

static char selected_var[100];
static tc_strings allNames;
static void run(tc_matrix input);
static void setup();
static int selectedItemsOnly = 1;

void unload()
{
	tc_deleteStringsArray(allNames);
}

void loadAllNames()
{
	int i,len;
	tc_matrix params, N;
	tc_items A = tc_createItemsArray(0);

	if (selectedItemsOnly)
		A = tc_selectedItems();
	
	if (A.length < 1 || !tc_getItem(A,0))
		A = tc_allItems();

	tc_deleteStringsArray(allNames);

	if (tc_getItem(A,0))
	{
		params = tc_getParameters(A);
		N = tc_getStoichiometry(A);
		len = N.rows;
		allNames = tc_createStringsArray(len+params.rows);
		for (i=0; i < params.rows; ++i) 
			tc_setString(allNames,i,tc_getRowName(params,i));
		for (i=0; i < len; ++i) 
			tc_setString(allNames,i+params.rows,tc_getRowName(N,i));
		
		params.rownames = tc_createStringsArray(0);
		tc_deleteMatrix(params);
		tc_deleteMatrix(N);
		tc_deleteItemsArray(A);
	}
}

void callback()
{
	loadAllNames();
	tc_addInputWindowOptions("At Time T",2, 0, allNames);
}

TCAPIEXPORT void tc_main()
{
	allNames = tc_createStringsArray(0);

	strcpy(selected_var,"\0");
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup, "Values at time=T0", "uses repeated simulation to compute state of system at the given time", "Parameter scan", "steadystate.png", "", 1, 0, 0);
	tc_callback(&callback);
	tc_callWhenExiting(&unload);
}

void setup()
{
	tc_matrix m;
	char * cols[] = { "value", 0 };
	char * rows[] = { "model", "simulation", "variable", "start", "end", "increments", "time", "plot", "use sliders"};
	double values[] = { 0.0, 0.0, 0.0, 0.0, 10, 0.5 , 100.0, 0, 1  };
	char * options1[] = { "Full model", "Selected only"};
	char * options2[] = { "ODE", "Stochastic" };
	char * options3[] = { "Variables", "Rates" };
	char * options4[] = { "Yes", "No" };
	tc_strings a1 = {2, options1};
	tc_strings a2 = {2, options2};
	tc_strings a3 = {2, options3};
	tc_strings a4 = {2, options4};

	loadAllNames();

	m.rownames.length = m.rows = 9;
	m.colnames.length = m.cols = 1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	//tc_createInputWindow(m,"dlls/runvaluesattime","run2","At Time T");
	tc_createInputWindow(m,"At Time T",&run);
	tc_addInputWindowOptions("At Time T",0, 0, a1);
	tc_addInputWindowOptions("At Time T",1, 0, a2);
	tc_addInputWindowOptions("At Time T",2, 0, allNames);
	tc_addInputWindowOptions("At Time T",7, 0, a3);
	tc_addInputWindowOptions("At Time T",8, 0, a4);

	return;
}

void run(tc_matrix input)
{
	double start = 0.0, end = 50.0;
	double dt = 0.1, time = 100.0;
	int doStochastic = 0;
	int selection = 0, index = 0, sz = 0, rateplot = 0, slider = 1;
	tc_items A, B;
	const char * param;
	FILE * out;
	tc_matrix params, initVals, allParams, N;
	char * runfuncInput = "tc_matrix input";
	char * runfunc = "";
	int i;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectedItemsOnly = selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			doStochastic = (int)(tc_getMatrixValue(input,1,0) > 0);
		if (input.rows > 2)
			index = tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			start = tc_getMatrixValue(input,3,0);
		if (input.rows > 4)
			end = tc_getMatrixValue(input,4,0);
		if (input.rows > 5)
			dt = tc_getMatrixValue(input,5,0);
		if (input.rows > 6)
			time = tc_getMatrixValue(input,6,0);
		if (input.rows > 7)
			rateplot = tc_getMatrixValue(input,7,0);
		if (input.rows > 8)
			slider = tc_getMatrixValue(input,8,0);
	}
	
	if (slider == 0)
		slider = 1;
	else
		slider = 0;

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (tc_getItem(A,0) == 0)
		{
			tc_deleteItemsArray(A);
			A = tc_allItems();
		}
	}
	else
	{
		A = tc_allItems();
	}

	sz = (int)((end - start) / dt);

	if (tc_getItem(A,0) != 0)
	{
		tc_writeModel( "timet", A );
	}
	else
	{
		tc_deleteItemsArray(A);
		return;
	}

	if (index < 0)
	{
		tc_print("steady state: no variable selected\0");
		tc_deleteItemsArray(A);
		return;
	}

	param = tc_getString(allNames,index); //the parameter to vary
	strcpy(selected_var,param);
	
	if (slider)
	{
		params = tc_getParameters(A);
		N = tc_getStoichiometry(A);
		B = tc_findItems(N.rownames);
		tc_deleteMatrix(N);
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
		
		tc_deleteMatrix(initVals);
		tc_deleteMatrix(params);
		tc_deleteItemsArray(B);
		runfunc = runfuncInput;
	}
	
	tc_deleteItemsArray(A);

	out = fopen("timet.c","a");

	fprintf( out , "\
#include \"TC_api.h\"\n#include \"cvodesim.h\"\n#include \"ssa.h\"\n\
TCAPIEXPORT void run(%s) \n\
{\n    initMTrand();\n    tc_matrix dat;\n    int i,j;\n", runfunc );

	fprintf( out, "\
    dat.rows = (int)((%lf-%lf)/%lf);\n\
    double * y, * y0, *y1;\n\
    TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
    (*model) = TC_initial_model;\n\
    if (%i) \n\
    {\n\
        dat.cols = 1+TCreactions;\n\
        dat.colnames = tc_createStringsArray(TCreactions);\n\
        for(i=0; i<TCreactions; ++i) dat.colnames.strings[1+i] = TCreactionnames[i];\n\
    }\n\
    else\n\
    {\n\
        dat.cols = 1+TCvars;\n\
        dat.colnames = tc_createStringsArray(1+TCvars);\n\
        for(i=0; i<TCvars; ++i) dat.colnames.strings[1+i] = TCvarnames[i];\n\
	}\n\
	dat.values = malloc(dat.cols * dat.rows * sizeof(double));\n\
	dat.rownames = tc_createStringsArray(0);\n\
	dat.colnames.strings[0] = \"%s\";\n",end,start,dt,rateplot,param);

	fprintf( out, "\n\
    for (i=0; i < dat.rows; ++i)\n\
    {\n\
        (*model) = TC_initial_model;\n");
        
	if (slider)
	{
		for (i=0; i < allParams.rows; ++i)
			fprintf(out, "    model->%s = tc_getMatrixValue(input,%i,0);\n",tc_getRowName(allParams,i),i);
	}

    fprintf( out,"\
        model->%s = %lf + i * %lf;\n\
        tc_setMatrixValue(dat,i,0,model->%s);\n\
        TCinitialize(model);\n\
        double * y = 0;\n\
        int sz = (int)(%lf*10.0);\n\
        if (%i)\n\
            y = SSA(TCvars, TCreactions, TCstoic, &(TCpropensity), TCinit, 0, %lf, 200000, &sz, (void*)model , TCevents, TCtriggers, TCresponses);\n\
        else \n\
            y = ODEsim2(TCvars, TCreactions, TCstoic, &(TCpropensity),TCinit, 0, %lf, 0.1, (void*)model , TCevents, TCtriggers, TCresponses);\n\
        if (y)\n\
        {\n\
            y1 = malloc(TCvars * sizeof(double));\n\
			    for (j=0; j<TCvars; ++j)\n\
  				    y1[j] = y[ (TCvars+1)*(sz-1) + j + 1];\n\
  			free(y);\n\
  			y = y1;\n\
            if (%i)\n\
			{\n\
				y0 = malloc(TCreactions * sizeof(double));\n\
				TCpropensity(0.0, y1, y0, (void*)model);\n\
				free(y); \n\
				y = y0;\n\
				for (j=0; j<TCreactions; ++j)\n\
				    tc_setMatrixValue(dat,i,j+1,y[j]);\n\
			}\n\
			else\n\
			for (j=0; j<TCvars; ++j)\n\
				tc_setMatrixValue(dat,i,j+1,y[j]);\n\
			free(y);\n\
        }\n\
        else\n\
        {\n\
	        if (%i)\n\
				for (j=0; j<TCreactions; ++j)\n\
				   tc_setMatrixValue(dat,i,j+1,0.0);\n\
			else\n\
				for (j=0; j<TCvars; ++j)\n\
				   tc_setMatrixValue(dat,i,j+1,0.0);\n\
        }\n\
        tc_showProgress((100*i)/dat.rows);\n\
    }\n\
    free(model);\n\
    tc_plot(dat,\"At time=%lf\");\n\
    free(dat.colnames.strings);\n\
    free(dat.values);\n",param,start,dt,param,time,doStochastic,time,time,rateplot,rateplot,time);

	if (slider)
		fprintf(out, "    tc_deleteMatrix(input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("timet.c -lode -lssa\0","run\0","At Time T\0",allParams);
		tc_deleteMatrix(allParams);
	}
	else
		tc_compileBuildLoad("timet.c -lode -lssa\0","run\0","At Time T\0");
		
	return;
}

