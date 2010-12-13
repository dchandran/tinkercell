/****************************************************************************

Asks user for a parameter or variable name (string), and then generates a code that generates
the steady state table by changing this value.

****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "TC_api.h"

static char selected_var[100];
static char selected_var1[100];
static char selected_var2[100];
static char target_var[100];
static tc_strings allNames;
static int selectAll = 1;

void run(tc_matrix input);
void run2D(tc_matrix input);
void setup1();
void setup2();

void unload()
{
	tc_deleteStringsArray(allNames);
}

void loadAllNames()
{
	int i,len;
	tc_matrix params, N;
	char ** names;
	tc_items A = tc_createItemsArray(0);

	if (selectAll)
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
	tc_addInputWindowOptions("Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",5, 0, allNames);
}

TCAPIEXPORT void tc_main()
{
	allNames = tc_createStringsArray(0);
	target_var[0] = 0;

	strcpy(selected_var,"\0");
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&setup1, "Steady state analysis", "uses Sundials library (compiles to C program)", "Steady state", "cvode.png", "", 1, 0, 0);
	tc_addFunction(&setup2, "2-Parameter Steady state analysis", "uses Sundials library (compiles to C program)", "Steady state", "cvode.png", "", 1, 0, 0);

	tc_callback(&callback);
	tc_callWhenExiting(&unload);
}

void setup1()
{
	tc_matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "variable", "start", "end", "increments", "plot", "use sliders", 0 };
	double values[] = { 0.0, 0.0, 0.0, 10, 0.1, 0, 1 };
	char * options1[] = { "Full model", "Selected only" };
	char * options2[] = { "Variables", "Rates" }; 
	char * options3[] = { "Yes", "No" };
	tc_strings a1 = {2, options1};
	tc_strings a2 = {2, options2};
	tc_strings a3 = {2, options3};

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
	tc_matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "x-variable","x-start", "x-end", "x-increment size", "y-variable","y-start", "y-end", "y-increments size", "use sliders" };
	double values[] = { 0.0, 0.0, 0.0, 10, 1.0 , 0.0, 0.0, 10, 1.0, 1.0 };
	char * options1[] = { "Full model", "Selected only"}; 
	char * options2[] = { "Yes", "No"};
	tc_strings a1 = {2, options1};
	tc_strings a2 = {2, options2};
	
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

void run(tc_matrix input)
{
	tc_matrix params, initVals, allParams, N;
	double start = 0.0, end = 50.0;
	double dt = 0.1;
	int selection = 0, slider = 1;
	int index = 0;
	int rateplot = 0;
	tc_items A, B;
	int i;
	const char * param;
	char * runfuncInput = "tc_matrix input";
	char * runfunc = "";
	FILE * out;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			index = tc_getMatrixValue(input,1,0);
		if (input.rows > 2)
			start = tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			end = tc_getMatrixValue(input,3,0);
		if (input.rows > 4)
			dt = tc_getMatrixValue(input,4,0);
		if (input.rows > 5)
			rateplot = (int)tc_getMatrixValue(input,5,0);
		if (input.rows > 6)
			slider = (int)tc_getMatrixValue(input,6,0);
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

	if (tc_getItem(A,0) != 0)
	{
		tc_writeModel( "ss", A );  //writes to ss.c and ss.py
	}
	else
	{
		tc_deleteItemsArray(A);
		if (slider)
			tc_deleteMatrix(allParams);
		return;
	}

	tc_deleteItemsArray(A);

	if (index < 0)
	{
		tc_print("steady state: no valid variable selected\0");
		if (slider)
			tc_deleteMatrix(allParams);
		return;
	}

	param = tc_getString(allNames,index); //the parameter to vary
	strcpy(selected_var,param);

	out = fopen("ss.c","a");

	fprintf( out , "\
#include \"TC_api.h\"\n#include \"cvodesim.h\"\n\n\
TCAPIEXPORT void run(%s) \n\
{\n    tc_matrix dat;\n    int i,j;\n", runfunc);

	fprintf( out, "\
    dat.rows = (int)((%lf-%lf)/%lf);\n\
    double * y, * y0;\n\
    TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
    (*model) = TC_initial_model;\n\
    if (%i) \n\
    {\n\
        dat.cols = 1+TCreactions;\n\
        dat.colnames = tc_createStringsArray(1+TCreactions);\n\
        for(i=0; i<TCreactions; ++i) tc_setColumnName(dat,1+i,TCreactionnames[i]);\n\
    }\n\
    else\n\
    {\n\
        dat.cols = 1+TCvars;\n\
        dat.colnames = tc_createStringsArray(1+TCvars);\n\
        for(i=0; i<TCvars; ++i) tc_setColumnName(dat,1+i,TCvarnames[i]);\n\
	}\n\
	dat.values = (double*)malloc(dat.cols * dat.rows * sizeof(double));\n\
	dat.rownames = tc_createStringsArray(0);\n\
	tc_setColumnName(dat,0,\"%s\");\n",end,start,dt,rateplot,param);

	fprintf( out, "\n\
				 for (i=0; i < dat.rows; ++i)\n\
				 {\n\
				    (*model) = TC_initial_model;\n");
	if (slider)
	{
		for (i=0; i < allParams.rows; ++i)
			fprintf(out, "    model->%s = tc_getMatrixValue(input,%i,0);\n",tc_getRowName(allParams,i),i);
	}
	fprintf( out, "\
					model->%s = %lf + i * %lf;\n\
					TCinitialize(model);\n\
					tc_setMatrixValue(dat,i,0,model->%s);\n\
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
							   tc_setMatrixValue(dat,i,j+1,tc_getMatrixValue(dat,i-1,j+1));\n\
						else\n\
							for (j=0; j<TCvars; ++j)\n\
							   tc_setMatrixValue(dat,i,j+1,tc_getMatrixValue(dat,i-1,j+1));\n\
					}\n\
					tc_showProgress((100*i)/dat.rows);\n\
				}\n\
				free(model);\n\
				tc_plot(dat,\"Steady State Plot\");\n\
				tc_deleteMatrix(dat);\n",param,start,dt,param,rateplot,rateplot);

	if (slider)
		fprintf(out, "    tc_deleteMatrix(input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("ss.c -lode\0","run\0","Steady state\0",allParams);
		tc_deleteMatrix(allParams);
	}
	else
		tc_compileBuildLoad("ss.c -lode\0","run\0","Steady state\0");

	
	return;
}

void run2D(tc_matrix input)
{
	tc_matrix params, initVals, allParams, N;
	double startx = 0.0, endx = 50.0, starty = 0.0, endy = 50.0;
	double dx = 0.1, dy = 0.1;
	int selection = 0;
	int index1 = 0, index2 = 1, index3 = 2;
	int rateplot = 0;
	tc_items A = tc_createItemsArray(0), B;
	int i, slider = 1;
	tc_strings names;
	const char * param1, * param2, * target;
	FILE * out;
	char * runfuncInput = "tc_matrix input";
	char * runfunc = "";

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)tc_getMatrixValue(input,0,0);
		if (input.rows > 1)
			index1 = tc_getMatrixValue(input,1,0);
		if (input.rows > 2)
			startx = tc_getMatrixValue(input,2,0);
		if (input.rows > 3)
			endx = tc_getMatrixValue(input,3,0);
		if (input.rows > 4)
			dx = tc_getMatrixValue(input,4,0);

		if (input.rows > 5)
			index2 = tc_getMatrixValue(input,5,0);
		if (input.rows > 6)
			starty = tc_getMatrixValue(input,6,0);
		if (input.rows > 7)
			endy = tc_getMatrixValue(input,7,0);
		if (input.rows > 8)
			dy = tc_getMatrixValue(input,8,0);
		
		if (input.rows > 9)
			slider = tc_getMatrixValue(input,9,0);
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


	if (tc_getItem(A,0) != 0)
	{
		tc_writeModel( "ss2D", A );  //writes to ss2D.c and ss2D.py
	}
	else
	{
		tc_deleteItemsArray(A);
		return;
	}

	params = tc_getParameters(A);
	names = tc_getUniqueNames(tc_itemsOfFamilyFrom("Node\0",A));

	if (index1 >= 0 && index2 >= 0 && (index1 == index2))
	{
		tc_deleteItemsArray(A);
		tc_deleteStringsArray(names);
		tc_deleteMatrix(params);
		tc_errorReport("2D steady state: cannot choose the same variable twice\0");
		return;
	}

	if (index1 >= 0 && index2 >= 0)
		index3 = tc_getStringFromList("Select Target",names,target_var);
	
	allParams = tc_createMatrix(0,0);
	
	if (slider)
	{
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
		tc_deleteItemsArray(B);
		runfunc = runfuncInput;
	}

	tc_deleteItemsArray(A);

	if (index1 < 0 || index2 < 0 || index3 < 0)
	{
		tc_deleteMatrix(params);
		tc_deleteMatrix(allParams);
		tc_deleteStringsArray(names);
		tc_print("2D steady state: no valid variable selected\0");
		return;
	}

	param1 = tc_getString(allNames,index1); //the first parameter to vary
	param2 = tc_getString(allNames,index2); //the second parameter to vary
	target = tc_getString(names,index3); //the target z-axis

	strcpy(selected_var1,param1);
	strcpy(selected_var2,param2);
	strcpy(target_var,target);

	out = fopen("ss2D.c","a");

	fprintf( out , "\
	#include \"TC_api.h\"\n    #include \"cvodesim.h\"\n\n\
	TCAPIEXPORT void run(%s) \n\
	{\n    tc_matrix dat;\n", runfunc);

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
      dat.rownames = tc_createStringsArray(0);\n",
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
			fprintf(out, "    model->%s = tc_getMatrixValue(input,%i,0);\n",tc_getRowName(allParams,i),i);
	  }
	
	  fprintf(out,"\
		   tc_setMatrixValue(dat,i*cols + j,0,model->%s = %lf + i * %lf);\n\
		   tc_setMatrixValue(dat,i*cols + j,1,model->%s = %lf + j * %lf);\n\
		   TCinitialize(model);\n\
		   y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit, (void*)model ,1E-4,100000.0,10, TCevents, TCtriggers, TCresponses);\n\
		   tc_setMatrixValue(dat,i*cols + j,2,model->%s);\n\
		   if (y)\n\
			  free(y);\n\
        }\n\
		tc_showProgress((100*i)/rows);\n\
      }\n\
	  free(model);\n\
      tc_surface(dat,\"Steady State Plot\");\n    free(dat.values);\n",param1,startx, dx, param2,starty, dy, target);
      
      if (slider)
		fprintf(out, "    tc_deleteMatrix(input);\n    return;\n}\n");
	  else
		fprintf(out, "    return;\n}\n");

	  fclose(out);
	
	  if (slider)
	  {
		  tc_compileBuildLoadSliders("ss2D.c -lode\0","run\0","2-parameter steady state\0",allParams);
		  tc_deleteMatrix(allParams);
  	  }
	  else
		  tc_compileBuildLoad("ss2D.c -lode\0","run\0","2-parameter steady state\0");


	tc_deleteMatrix(params);
	return;
}

