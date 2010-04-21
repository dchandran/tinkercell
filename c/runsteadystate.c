/****************************************************************************

Asks user for a parameter or variable name (string), and then generates a code that generates
the steady state table by changing this value.

****************************************************************************/

#include <string.h>

#include "TC_api.h"

static char selected_var[100];
static char selected_var1[100];
static char selected_var2[100];
static char target_var[100];
static ArrayOfStrings allNames;
static int selectAll = 1;

void run(Matrix input);
void run2D(Matrix input);
void setup1();
void setup2();

void unload()
{
	deleteArrayOfStrings(&allNames);
}

void loadAllNames()
{
	int i,len;
	Matrix params, N;
	char ** names;
	ArrayOfItems A = newArrayOfItems(0);

	if (selectAll)
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
		deleteMatrix(&params);
		deleteMatrix(&N);
		deleteArrayOfItems(&A);
	}
}

void callback()
{
	loadAllNames();
	tc_addInputWindowOptions("Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",5, 0, allNames);
}

TCAPIEXPORT void tc_main()
{
	allNames = newArrayOfStrings(0);
	target_var[0] = 0;

	strcpy(selected_var,"\0");
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup1, "Steady state analysis", "uses Sundials library (compiles to C program)", "Steady state", "Plugins/c/cvode.png", "", 1, 0, 0);
	tc_addFunction(&setup2, "2-Parameter Steady state analysis", "uses Sundials library (compiles to C program)", "Steady state", "Plugins/c/cvode.png", "", 1, 0, 0);
	tc_callback(&callback);
	tc_callWhenExiting(&unload);
}

void setup1()
{
	Matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "variable", "start", "end", "increments", "plot", "use sliders", 0 };
	double values[] = { 0.0, 0.0, 0.0, 10, 0.1, 0, 1 };
	char * options1[] = { "Full model", "Selected only" };
	char * options2[] = { "Variables", "Rates" }; 
	char * options3[] = { "Yes", "No" };
	ArrayOfStrings a1 = {2, options1};
	ArrayOfStrings a2 = {2, options2};
	ArrayOfStrings a3 = {2, options3};

	loadAllNames();

	m.rownames.length = m.rows = 7;
	m.colnames.length = m.cols = 1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Steady state analysis",&run);
	tc_addInputWindowOptions("Steady state analysis",0, 0, a1);
	tc_addInputWindowOptions("Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("Steady state analysis",5, 0, a2);
	tc_addInputWindowOptions("Steady state analysis",6, 0, a3);
}

void setup2()
{
	Matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "x-variable","x-start", "x-end", "x-increment size", "y-variable","y-start", "y-end", "y-increments size", "use sliders" };
	double values[] = { 0.0, 0.0, 0.0, 10, 1.0 , 0.0, 0.0, 10, 1.0, 1.0 };
	char * options1[] = { "Full model", "Selected only"}; 
	char * options2[] = { "Yes", "No"};
	ArrayOfStrings a1 = {2, options1};
	ArrayOfStrings a2 = {2, options2};
	
	loadAllNames();

	m.rownames.length = m.rows = 10;
	m.colnames.length = m.cols = 1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"2-D Steady state analysis",&run2D);
	tc_addInputWindowOptions("2-D Steady state analysis",0, 0, a1);
	tc_addInputWindowOptions("2-D Steady state analysis",9, 0, a2);
	tc_addInputWindowOptions("2-D Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",5, 0, allNames);
}

void run(Matrix input)
{
	Matrix params, initVals, allParams, N;
	double start = 0.0, end = 50.0;
	double dt = 0.1;
	int selection = 0, slider = 1;
	int index = 0;
	int rateplot = 0;
	ArrayOfItems A, B;
	int i;
	const char * param;
	char * runfuncInput = "Matrix input";
	char * runfunc = "";
	FILE * out;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)getValue(input,0,0);
		if (input.rows > 1)
			index = getValue(input,1,0);
		if (input.rows > 2)
			start = getValue(input,2,0);
		if (input.rows > 3)
			end = getValue(input,3,0);
		if (input.rows > 4)
			dt = getValue(input,4,0);
		if (input.rows > 5)
			rateplot = (int)getValue(input,5,0);
		if (input.rows > 6)
			slider = (int)getValue(input,6,0);
	}
	
	if (end < start) 
	{
		tc_errorReport("end value is less than start value");
		return;
	}
	
	if (dt < 0.0)
	{
		tc_errorReport("increment size must be positive");
		return;
	}

	if ((end-start) < dt*2.0)
	{
		tc_errorReport("increment size is too large. Either change the start/end values or decrease the increment step size.");
		return;
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
	
	if (slider)
	{
		params = tc_getParameters(A);
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

	if (nthItem(A,0) != 0)
	{
		tc_writeModel( "ss", A );  //writes to ss.c and ss.py
	}
	else
	{
		deleteArrayOfItems(&A);
		if (slider)
			deleteMatrix(&allParams);
		return;
	}

	deleteArrayOfItems(&A);

	if (index < 0)
	{
		tc_print("steady state: no valid variable selected\0");
		if (slider)
			deleteMatrix(&allParams);
		return;
	}

	param = nthString(allNames,index); //the parameter to vary
	strcpy(selected_var,param);

	out = fopen("ss.c","a");

	fprintf( out , "\
#include \"TC_api.h\"\n#include \"cvodesim.h\"\n\n\
TCAPIEXPORT void run(%s) \n\
{\n    Matrix dat;\n    int i,j;\n", runfunc);

	fprintf( out, "\
    dat.rows = (int)((%lf-%lf)/%lf);\n\
    double * y, * y0;\n\
    TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
    (*model) = TC_initial_model;\n\
    if (%i) \n\
    {\n\
        dat.cols = 1+TCreactions;\n\
        dat.colnames = newArrayOfStrings(1+TCreactions);\n\
        for(i=0; i<TCreactions; ++i) setColumnName(dat,1+i,TCreactionnames[i]);\n\
    }\n\
    else\n\
    {\n\
        dat.cols = 1+TCvars;\n\
        dat.colnames = newArrayOfStrings(1+TCvars);\n\
        for(i=0; i<TCvars; ++i) setColumnName(dat,1+i,TCvarnames[i]);\n\
	}\n\
	dat.values = (double*)malloc(dat.cols * dat.rows * sizeof(double));\n\
	dat.rownames = newArrayOfStrings(0);\n\
	setColumnName(dat,0,\"%s\");\n",end,start,dt,rateplot,param);

	fprintf( out, "\n\
				 for (i=0; i < dat.rows; ++i)\n\
				 {\n\
				    (*model) = TC_initial_model;\n");
	if (slider)
	{
		for (i=0; i < allParams.rows; ++i)
			fprintf(out, "    model->%s = getValue(input,%i,0);\n",getRowName(allParams,i),i);
	}
	fprintf( out, "\
					model->%s = %lf + i * %lf;\n\
					TCinitialize(model);\n\
					setValue(dat,i,0,model->%s);\n\
					y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit, (void*)model ,1E-4,100000.0,10, TCevents, TCtriggers, TCresponses);\n\
					if (y)\n\
					{\n\
						if (%i)\n\
						{\n\
							y0 = malloc(TCreactions * sizeof(double));\n\
							TCpropensity(0.0, y, y0, (void*)model);\n\
							free(y);\n\
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
				tc_plot(dat,0,\"Steady State Plot\",0);\n\
				deleteMatrix(&dat);\n",param,start,dt,param,rateplot,rateplot);

	if (slider)
		fprintf(out, "    deleteMatrix(&input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("ss.c -lode\0","run\0","Steady state\0",allParams);
		deleteMatrix(&allParams);
	}
	else
		tc_compileBuildLoad("ss.c -lode\0","run\0","Steady state\0");

	
	return;
}

void run2D(Matrix input)
{
	Matrix params, initVals, allParams, N;
	double startx = 0.0, endx = 50.0, starty = 0.0, endy = 50.0;
	double dx = 0.1, dy = 0.1;
	int selection = 0;
	int index1 = 0, index2 = 1, index3 = 2;
	int rateplot = 0;
	ArrayOfItems A = newArrayOfItems(0), B;
	int i, slider = 1;
	ArrayOfStrings names;
	const char * param1, * param2, * target;
	FILE * out;
	char * runfuncInput = "Matrix input";
	char * runfunc = "";

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)getValue(input,0,0);
		if (input.rows > 1)
			index1 = getValue(input,1,0);
		if (input.rows > 2)
			startx = getValue(input,2,0);
		if (input.rows > 3)
			endx = getValue(input,3,0);
		if (input.rows > 4)
			dx = getValue(input,4,0);

		if (input.rows > 5)
			index2 = getValue(input,5,0);
		if (input.rows > 6)
			starty = getValue(input,6,0);
		if (input.rows > 7)
			endy = getValue(input,7,0);
		if (input.rows > 8)
			dy = getValue(input,8,0);
		
		if (input.rows > 9)
			slider = getValue(input,9,0);
	}
		
	if (endx < startx || endy < starty) 
	{
		tc_errorReport("end value is less than start value");
		return;
	}
	
	if (dx < 0.0 || dy < 0.0)
	{
		tc_errorReport("increment size must be positive");
		return;
	}

	if ((endx-startx) < dx*2.0 || (endy-starty) < dy*2.0)
	{
		tc_errorReport("increment size is too large. Either change the start/end values or decrease the increment step size.");
		return;
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


	if (nthItem(A,0) != 0)
	{
		tc_writeModel( "ss2D", A );  //writes to ss2D.c and ss2D.py
	}
	else
	{
		deleteArrayOfItems(&A);
		return;
	}

	params = tc_getParameters(A);
	names = tc_getUniqueNames(tc_itemsOfFamilyFrom("Node\0",A));

	if (index1 >= 0 && index2 >= 0 && (index1 == index2))
	{
		deleteArrayOfItems(&A);
		deleteArrayOfStrings(&names);
		deleteMatrix(&params);
		tc_errorReport("2D steady state: cannot choose the same variable twice\0");
		return;
	}

	if (index1 >= 0 && index2 >= 0)
		index3 = tc_getStringFromList("Select Target",names,target_var);
	
	allParams = newMatrix(0,0);
	
	if (slider)
	{
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
		deleteArrayOfItems(&B);
		runfunc = runfuncInput;
	}

	deleteArrayOfItems(&A);

	if (index1 < 0 || index2 < 0 || index3 < 0)
	{
		deleteMatrix(&params);
		deleteMatrix(&allParams);
		deleteArrayOfStrings(&names);
		tc_print("2D steady state: no valid variable selected\0");
		return;
	}

	param1 = nthString(allNames,index1); //the first parameter to vary
	param2 = nthString(allNames,index2); //the second parameter to vary
	target = nthString(names,index3); //the target z-axis

	strcpy(selected_var1,param1);
	strcpy(selected_var2,param2);
	strcpy(target_var,target);

	out = fopen("ss2D.c","a");

	fprintf( out , "\
	#include \"TC_api.h\"\n    #include \"cvodesim.h\"\n\n\
	TCAPIEXPORT void run(%s) \n\	{\n    Matrix dat;\n", runfunc);

	fprintf(out, "\
	  int rows = (int)((%lf-%lf)/%lf);\n\
      int cols = (int)((%lf-%lf)/%lf);\n\
      double * y, *y0;\n\
      int i,j;\n\
      char * colnames[] = {\"%s\", \"%s\", \"%s\", 0};\n\
	  TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	  (*model) = TC_initial_model;\n\
      dat.colnames.length = dat.cols = 3;\n\
      dat.rows = rows * cols;\n\
      dat.colnames.strings = colnames;\n\
      dat.values = (double*)malloc(3 * cols * rows * sizeof(double));\n\
      dat.rownames = newArrayOfStrings(0);\n",
      endx,startx,dx,endy,starty,dy,param1,param2,target);

 	  fprintf(out, "\n\
      for (i=0; i < rows; ++i)\n\
      {\n\
        for (j=0; j < cols; ++j)\n\
		{\n\
		   (*model) = TC_initial_model;\n");
	
	  if (slider)
	  {
		for (i=0; i < allParams.rows; ++i)
			fprintf(out, "    model->%s = getValue(input,%i,0);\n",getRowName(allParams,i),i);
	  }
	
	  fprintf(out,"\
		   setValue(dat,i*cols + j,0,model->%s = %lf + i * %lf);\n\
		   setValue(dat,i*cols + j,1,model->%s = %lf + j * %lf);\n\
		   TCinitialize(model);\n\
		   y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit, (void*)model ,1E-4,100000.0,10, TCevents, TCtriggers, TCresponses);\n\
		   setValue(dat,i*cols + j,2,model->%s);\n\
		   if (y)\n\
			  free(y);\n\
        }\n\
		tc_showProgress((100*i)/rows);\n\
      }\n\
	  free(model);\n\
      tc_surface(dat,\"Steady State Plot\");\n    free(dat.values);\n",param1,startx, dx, param2,starty, dy, target);
      
      if (slider)
		fprintf(out, "    deleteMatrix(&input);\n    return;\n}\n");
	  else
		fprintf(out, "    return;\n}\n");

	  fclose(out);
	
	  if (slider)
	  {
		  tc_compileBuildLoadSliders("ss2D.c -lodesim\0","run\0","2-parameter steady state\0",allParams);
		  deleteMatrix(&allParams);
  	  }
	  else
		  tc_compileBuildLoad("ss2D.c -lodesim\0","run\0","2-parameter steady state\0");


	deleteMatrix(&params);
	return;
}

