/****************************************************************************

Asks user for a parameter or variable name (string), and then generates a code that generates
the steady state table by changing this value.

****************************************************************************/

#include "TC_api.h"

static char * selected_var;
static char ** allNames = 0;
void run(Matrix input);
void setup();
int selectAll = 1;

void unload()
{
	if (allNames)
		TCFreeChars(allNames);
}

void loadAllNames()
{
	int i,len;
	Matrix params,N;
	char ** names;
	Array A,B;

	if (selectAll)
		A = tc_selectedItems();
	
	if (!A || !A[0])
		A = tc_allItems();

	if (allNames)
		TCFreeChars(allNames);

	allNames = 0;

	if (A && A[0])
	{
		params = tc_getModelParameters(A);
		N = tc_getStoichiometry(A);
		names = N.rownames;
		len = 0;
		while (names[len]) ++len;
		allNames = (char**)malloc((len+params.rows+1)*sizeof(char*));
		for (i=0; i < params.rows; ++i) allNames[i] = params.rownames[i];
		for (i=0; i < len; ++i) allNames[i+params.rows] = names[i];
		allNames[(len+params.rows)] = 0;
		free(params.rownames);
		params.rownames = 0;
		TCFreeMatrix(params);
		free(N.rownames);
		N.rownames = 0;
		TCFreeMatrix(N);
		TCFreeArray(A);
		TCFreeArray(B);
	}
}

void callback()
{
	loadAllNames();
	tc_addInputWindowOptions("At Time T",2, 0, allNames);
}

void tc_main()
{
	allNames = 0;

	selected_var = "";
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup, "Values at time=T0", "uses repeated simulation to compute state of system at the given time", "Parameter scan", "Plugins/c/steadystate.png", "", 1, 0, 0);
	tc_callback(&callback);
	tc_callWhenExiting(&unload);
}

void setup()
{
	Matrix m;
	char * cols[] = { "value", 0 };
	char * rows[] = { "model", "simulation", "variable", "start", "end", "increments", "time", "plot", "use sliders", 0};
	double values[] = { 0.0, 0.0, 0.0, 0.0, 10, 0.5 , 100.0, 0, 1  };
	char * options1[] = { "Full model", "Selected only", 0};
	char * options2[] = { "ODE", "Stochastic", 0  }; //null terminated -- very very important
	char * options3[] = { "Variables", "Rates", 0 };
	char * options4[] = { "Yes", "No", 0 };

	loadAllNames();

	m.rows = 9;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	//tc_createInputWindow(m,"dlls/runvaluesattime","run2","At Time T");
	tc_createInputWindow(m,"At Time T",&run);
	tc_addInputWindowOptions("At Time T",0, 0, options1);
	tc_addInputWindowOptions("At Time T",1, 0, options2);
	tc_addInputWindowOptions("At Time T",2, 0, allNames);
	tc_addInputWindowOptions("At Time T",7, 0, options3);
	tc_addInputWindowOptions("At Time T",8, 0, options4);

	return;
}

void run(Matrix input)
{
	double start = 0.0, end = 50.0;
	double dt = 0.1, time = 100.0;
	int doStochastic = 0;
	int selection = 0, index = 0, sz = 0, rateplot = 0, slider = 1;
	Array A, B;
	char * param;
	FILE * out;
	Matrix params, initVals, allParams, N;
	char * runfuncInput = "Matrix input";
	char * runfunc = "";
	int i;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)valueAt(input,0,0);
		if (input.rows > 1)
			doStochastic = (int)(valueAt(input,1,0) > 0);
		if (input.rows > 2)
			index = valueAt(input,2,0);
		if (input.rows > 3)
			start = valueAt(input,3,0);
		if (input.rows > 4)
			end = valueAt(input,4,0);
		if (input.rows > 5)
			dt = valueAt(input,5,0);
		if (input.rows > 6)
			time = valueAt(input,6,0);
		if (input.rows > 7)
			rateplot = valueAt(input,7,0);
		if (input.rows > 8)
			slider = valueAt(input,8,0);
	}
	
	if (slider == 0)
		slider = 1;
	else
		slider = 0;

	if (selection > 0)
	{
		A = tc_selectedItems();
		if (A[0] == 0)
		{
			TCFreeArray(A);
			A = tc_allItems();
		}
	}
	else
	{
		A = tc_allItems();
	}

	sz = (int)((end - start) / dt);

	if (A[0] != 0)
	{
		tc_writeModel( "timet", A );
	}
	else
	{
		TCFreeArray(A);
		return;
	}

	if (index < 0)
	{
		tc_print("steady state: no variable selected\0");
		TCFreeArray(A);
		return;
	}

	param = allNames[index]; //the parameter to vary
	selected_var = param;
	
	if (slider)
	{
		params = tc_getModelParameters(A);
		N = tc_getStoichiometry(A);
		B = tc_findItems(N.rownames);
		TCFreeMatrix(N);
		
		initVals = tc_getInitialValues(A);
		
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
		runfunc = runfuncInput;
	}
	
	TCFreeArray(A);

	out = fopen("timet.c","a");

	fprintf( out , "\
#include \"TC_api.h\"\n#include \"cvodesim.h\"\n#include \"ssa.h\"\n\n\
void run(%s) \n\
{\n    initMTrand();\n    Matrix dat;\n    int i,j;\n", runfunc );

	fprintf( out, "\
    dat.rows = (int)((%lf-%lf)/%lf);\n\
    double * y, * y0, *y1;\n\
    TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
    (*model) = TC_initial_model;\n\
    if (%i) \n\
    {\n\
        dat.cols = 1+TCreactions;\n\
        dat.colnames = malloc( (1+TCreactions) * sizeof(char*) );\n\
        for(i=0; i<TCreactions; ++i) dat.colnames[1+i] = TCreactionnames[i];\n\
    }\n\
    else\n\
    {\n\
        dat.cols = 1+TCvars;\n\
        dat.colnames = malloc( (1+TCvars) * sizeof(char*) );\n\
        for(i=0; i<TCvars; ++i) dat.colnames[1+i] = TCvarnames[i];\n\
	}\n\
	dat.values = malloc(dat.cols * dat.rows * sizeof(double));\n\
	dat.rownames = 0;\n\
	dat.colnames[0] = \"%s\";\n",end,start,dt,rateplot,param);

	fprintf( out, "\n\
    for (i=0; i < dat.rows; ++i)\n\
    {\n\
        (*model) = TC_initial_model;\n");
        
	if (slider)
	{
		for (i=0; i < allParams.rows; ++i)
			fprintf(out, "    model->%s = valueAt(input,%i,0);\n",allParams.rownames[i],i);
	}

    fprintf( out,"\
        model->%s = %lf + i * %lf;\n\
        valueAt(dat,i,0) = model->%s;\n\
        TCinitialize(model);\n\
        double * y = 0;\n\
        int sz = (int)(%lf*10.0);\n\
        if (%i)\n\
            y = SSA(TCvars, TCreactions, TCstoic, &(TCpropensity), TCinit, 0, %lf, 200000, &sz, (void*)model );\n\
        else \n\
            y = ODEsim2(TCvars, TCreactions, TCstoic, &(TCpropensity),TCinit, 0, %lf, 0.1, (void*)model );\n\
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
				    valueAt(dat,i,j+1) = y[j];\n\
			}\n\
			else\n\
			for (j=0; j<TCvars; ++j)\n\
				valueAt(dat,i,j+1) = y[j];\n\
			free(y);\n\
        }\n\
        else\n\
        {\n\
	        if (%i)\n\
				for (j=0; j<TCreactions; ++j)\n\
				   valueAt(dat,i,j+1) = 0.0;\n\
			else\n\
				for (j=0; j<TCvars; ++j)\n\
				   valueAt(dat,i,j+1) = 0.0;\n\
        }\n\
        tc_showProgress(\"At Time T\",(100*i)/dat.rows);\n\
    }\n\
    free(model);\n\
    tc_plot(dat,0,\"At time=%lf\",0);\n\
    free(dat.colnames);\n\
    free(dat.values);\n",param,start,dt,param,time,doStochastic,time,time,rateplot,rateplot,time);

	if (slider)
		fprintf(out, "    TCFreeMatrix(input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("timet.c -lodesim -lssa\0","run\0","At Time T\0",allParams);
		TCFreeMatrix(allParams);
	}
	else
		tc_compileBuildLoad("timet.c -lodesim -lssa\0","run\0","At Time T\0");
		
	return;
}

