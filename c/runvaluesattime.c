/****************************************************************************

Asks user for a parameter or variable name (string), and then generates a code that generates
the steady state table by changing this value.

****************************************************************************/
#include <string.h>
#include "TC_api.h"

static char selected_var[100];
static ArrayOfStrings allNames;
static void run(TableOfReals input);
static void setup();
static int selectedItemsOnly = 1;

void unload()
{
	deleteArrayOfStrings(&allNames);
}

void loadAllNames()
{
	int i,len;
	TableOfReals params, N;
	ArrayOfItems A = newArrayOfItems(0);

	if (selectedItemsOnly)
		A = tc_selectedItems();
	
	if (A.length < 1 || !nthItem(A,0))
		A = tc_allItems();

	deleteArrayOfStrings(&allNames);

	if (nthItem(A,0))
	{
		params = tc_getParameters(A);
		N = tc_getStoichiometry(A);
		len = N.rows;
		allNames = newArrayOfStrings(len+params.rows);
		for (i=0; i < params.rows; ++i) 
			setNthString(allNames,i,getRowName(params,i));
		for (i=0; i < len; ++i) 
			setNthString(allNames,i+params.rows,getRowName(N,i));
		
		params.rownames = newArrayOfStrings(0);
		deleteTableOfReals(&params);
		deleteTableOfReals(&N);
		deleteArrayOfItems(&A);
	}
}

void callback()
{
	loadAllNames();
	tc_addInputWindowOptions("At Time T",2, 0, allNames);
}

TCAPIEXPORT void tc_main()
{
	allNames = newArrayOfStrings(0);

	strcpy(selected_var,"\0");
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup, "Values at time=T0", "uses repeated simulation to compute state of system at the given time", "Parameter scan", "plugins/c/steadystate.png", "", 1, 0, 0);
	tc_callback(&callback);
	tc_callWhenExiting(&unload);
}

void setup()
{
	TableOfReals m;
	char * cols[] = { "value", 0 };
	char * rows[] = { "model", "simulation", "variable", "start", "end", "increments", "time", "plot", "use sliders"};
	double values[] = { 0.0, 0.0, 0.0, 0.0, 10, 0.5 , 100.0, 0, 1  };
	char * options1[] = { "Full model", "Selected only"};
	char * options2[] = { "ODE", "Stochastic" };
	char * options3[] = { "Variables", "Rates" };
	char * options4[] = { "Yes", "No" };
	ArrayOfStrings a1 = {2, options1};
	ArrayOfStrings a2 = {2, options2};
	ArrayOfStrings a3 = {2, options3};
	ArrayOfStrings a4 = {2, options4};

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

void run(TableOfReals input)
{
	double start = 0.0, end = 50.0;
	double dt = 0.1, time = 100.0;
	int doStochastic = 0;
	int selection = 0, index = 0, sz = 0, rateplot = 0, slider = 1;
	ArrayOfItems A, B;
	const char * param;
	FILE * out;
	TableOfReals params, initVals, allParams, N;
	char * runfuncInput = "TableOfReals input";
	char * runfunc = "";
	int i;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectedItemsOnly = selection = (int)getValue(input,0,0);
		if (input.rows > 1)
			doStochastic = (int)(getValue(input,1,0) > 0);
		if (input.rows > 2)
			index = getValue(input,2,0);
		if (input.rows > 3)
			start = getValue(input,3,0);
		if (input.rows > 4)
			end = getValue(input,4,0);
		if (input.rows > 5)
			dt = getValue(input,5,0);
		if (input.rows > 6)
			time = getValue(input,6,0);
		if (input.rows > 7)
			rateplot = getValue(input,7,0);
		if (input.rows > 8)
			slider = getValue(input,8,0);
	}
	
	if (slider == 0)
		slider = 1;
	else
		slider = 0;

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (nthItem(A,0) == 0)
		{
			deleteArrayOfItems(&A);
			A = tc_allItems();
		}
	}
	else
	{
		A = tc_allItems();
	}

	sz = (int)((end - start) / dt);

	if (nthItem(A,0) != 0)
	{
		tc_writeModel( "timet", A );
	}
	else
	{
		deleteArrayOfItems(&A);
		return;
	}

	if (index < 0)
	{
		tc_print("steady state: no variable selected\0");
		deleteArrayOfItems(&A);
		return;
	}

	param = nthString(allNames,index); //the parameter to vary
	strcpy(selected_var,param);
	
	if (slider)
	{
		params = tc_getParameters(A);
		N = tc_getStoichiometry(A);
		B = tc_findItems(N.rownames);
		deleteTableOfReals(&N);
		initVals = tc_getInitialValues(B);

		allParams = newTableOfReals(initVals.rows+params.rows,2);

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
		
		deleteTableOfReals(&initVals);
		deleteTableOfReals(&params);
		deleteArrayOfItems(&B);
		runfunc = runfuncInput;
	}
	
	deleteArrayOfItems(&A);

	out = fopen("timet.c","a");

	fprintf( out , "\
#include \"TC_api.h\"\n#include \"cvodesim.h\"\n#include \"ssa.h\"\n\
TCAPIEXPORT void run(%s) \n\
{\n    initMTrand();\n    TableOfReals dat;\n    int i,j;\n", runfunc );

	fprintf( out, "\
    dat.rows = (int)((%lf-%lf)/%lf);\n\
    double * y, * y0, *y1;\n\
    TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
    (*model) = TC_initial_model;\n\
    if (%i) \n\
    {\n\
        dat.cols = 1+TCreactions;\n\
        dat.colnames = newArrayOfStrings(TCreactions);\n\
        for(i=0; i<TCreactions; ++i) dat.colnames.strings[1+i] = TCreactionnames[i];\n\
    }\n\
    else\n\
    {\n\
        dat.cols = 1+TCvars;\n\
        dat.colnames = newArrayOfStrings(1+TCvars);\n\
        for(i=0; i<TCvars; ++i) dat.colnames.strings[1+i] = TCvarnames[i];\n\
	}\n\
	dat.values = malloc(dat.cols * dat.rows * sizeof(double));\n\
	dat.rownames = newArrayOfStrings(0);\n\
	dat.colnames.strings[0] = \"%s\";\n",end,start,dt,rateplot,param);

	fprintf( out, "\n\
    for (i=0; i < dat.rows; ++i)\n\
    {\n\
        (*model) = TC_initial_model;\n");
        
	if (slider)
	{
		for (i=0; i < allParams.rows; ++i)
			fprintf(out, "    model->%s = getValue(input,%i,0);\n",getRowName(allParams,i),i);
	}

    fprintf( out,"\
        model->%s = %lf + i * %lf;\n\
        setValue(dat,i,0,model->%s);\n\
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
				    setValue(dat,i,j+1,y[j]);\n\
			}\n\
			else\n\
			for (j=0; j<TCvars; ++j)\n\
				setValue(dat,i,j+1,y[j]);\n\
			free(y);\n\
        }\n\
        else\n\
        {\n\
	        if (%i)\n\
				for (j=0; j<TCreactions; ++j)\n\
				   setValue(dat,i,j+1,0.0);\n\
			else\n\
				for (j=0; j<TCvars; ++j)\n\
				   setValue(dat,i,j+1,0.0);\n\
        }\n\
        tc_showProgress((100*i)/dat.rows);\n\
    }\n\
    free(model);\n\
    tc_plot(dat,0,\"At time=%lf\",0);\n\
    free(dat.colnames.strings);\n\
    free(dat.values);\n",param,start,dt,param,time,doStochastic,time,time,rateplot,rateplot,time);

	if (slider)
		fprintf(out, "    deleteTableOfReals(&input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("timet.c -lode -lssa\0","run\0","At Time T\0",allParams);
		deleteTableOfReals(&allParams);
	}
	else
		tc_compileBuildLoad("timet.c -lode -lssa\0","run\0","At Time T\0");
		
	return;
}

