/****************************************************************************

Asks user for a parameter or variable name (string), and then generates a code that generates
the steady state table by changing this value.

****************************************************************************/

#include <string.h>

#include "TC_api.h"

char selected_var[100];
char selected_var1[100];
char selected_var2[100];
char target_var[100];
char ** allNames = 0;
int selectAll = 1;

void run(Matrix input);
void run2D(Matrix input);
void setup1();
void setup2();

void unload()
{
	if (allNames)
		TCFreeChars(allNames);
	allNames = 0;
}

void loadAllNames()
{
	int i,len;
	Matrix params, N;
	char ** names;
	Array A;

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
	}
}

void callback()
{
	loadAllNames();
	tc_addInputWindowOptions("Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("2-D Steady state analysis",5, 0, allNames);
}

void tc_main()
{
	allNames = 0;
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
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important
	char * options2[] = { "Variables", "Rates", 0 }; //null terminated -- very important
	char * options3[] = { "Yes", "No", 0 };

	loadAllNames();

	m.rows = 7;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	tc_createInputWindow(m,"Steady state analysis",&run);
	tc_addInputWindowOptions("Steady state analysis",0, 0, options1);
	tc_addInputWindowOptions("Steady state analysis",1, 0, allNames);
	tc_addInputWindowOptions("Steady state analysis",5, 0, options2);
	tc_addInputWindowOptions("Steady state analysis",6, 0, options3);
}

void setup2()
{
	Matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "x-variable","x-start", "x-end", "x-increment size", "y-variable","y-start", "y-end", "y-increments size", 0 };
	double values[] = { 0.0, 0.0, 0.0, 10, 1.0 , 0.0, 0.0, 10, 1.0 };
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important

	m.rows = 9;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;

	tc_createInputWindow(m,"2-D Steady state analysis",&run2D);
	tc_addInputWindowOptions("2-D Steady state analysis",0, 0, options1);
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
	Array A, B;
	int i;
	char * param;
	char * runfuncInput = "Matrix input";
	char * runfunc = "";
	FILE * out;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)valueAt(input,0,0);
		if (input.rows > 1)
			index = valueAt(input,1,0);
		if (input.rows > 2)
			start = valueAt(input,2,0);
		if (input.rows > 3)
			end = valueAt(input,3,0);
		if (input.rows > 4)
			dt = valueAt(input,4,0);
		if (input.rows > 5)
			rateplot = (int)valueAt(input,5,0);
		if (input.rows > 6)
			slider = (int)valueAt(input,6,0);
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

	if (A[0] != 0)
	{
		tc_writeModel( "ss", A );  //writes to ss.c and ss.py
	}
	else
	{
		TCFreeArray(A);
		if (slider)
			TCFreeMatrix(allParams);
		return;
	}

	TCFreeArray(A);

	if (index < 0)
	{
		tc_print("steady state: no valid variable selected\0");
		if (slider)
			TCFreeMatrix(allParams);
		return;
	}

	param = allNames[index]; //the parameter to vary
	strcpy(selected_var,param);

	out = fopen("ss.c","a");

	fprintf( out , "\
#include \"TC_api.h\"\n#include \"cvodesim.h\"\n\n\
void run(%s) \n\
{\n    Matrix dat;\n    int i,j;\n", runfunc);

	fprintf( out, "\
    dat.rows = (int)((%lf-%lf)/%lf);\n\
    double * y, * y0;\n\
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
	fprintf( out, "model->%s = %lf + i * %lf;\n\
					TCinitialize(model);\n\
					valueAt(dat,i,0) = model->%s;\n\
					y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit, (void*)model ,1E-4,100000.0,10);\n\
					if (y)\n\
					{\n\
						if (%i)\n\
						{\n\
							y0 = malloc(TCreactions * sizeof(double));\n\
							TCpropensity(0.0, y, y0, (void*)model);\n\
							free(y);\n\
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
					tc_showProgress(\"Steady state\",(100*i)/dat.rows);\n\
				}\n\
				free(model);\n\
				tc_plot(dat,0,\"Steady State Plot\",0);\n\
				free(dat.colnames);\n    free(dat.values);\n",param,start,dt,param,rateplot,rateplot);

	if (slider)
		fprintf(out, "    TCFreeMatrix(input);\n    return;\n}\n");
	else
		fprintf(out, "    return;\n}\n");

	fclose(out);
	
	if (slider)
	{
		tc_compileBuildLoadSliders("ss.c -lodesim\0","run\0","Steady state\0",allParams);
		TCFreeMatrix(allParams);
	}
	else
		tc_compileBuildLoad("ss.c -lodesim\0","run\0","Steady state\0");

	
	return;
}

void run2D(Matrix input)
{
	double startx = 0.0, endx = 50.0, starty = 0.0, endy = 50.0;
	double dx = 0.1, dy = 0.1;
	int selection = 0;
	int index1 = 0, index2 = 1, index3 = 2;
	int rateplot = 0;
	Array A;
	int i, len;
	Matrix params;
	char ** names;
	char * param1, * param2, * target;
	FILE * out;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selectAll = selection = (int)valueAt(input,0,0);
		if (input.rows > 1)
			index1 = valueAt(input,1,0);
		if (input.rows > 2)
			startx = valueAt(input,2,0);
		if (input.rows > 3)
			endx = valueAt(input,3,0);
		if (input.rows > 4)
			dx = valueAt(input,4,0);

		if (input.rows > 5)
			index2 = valueAt(input,5,0);
		if (input.rows > 6)
			starty = valueAt(input,6,0);
		if (input.rows > 7)
			endy = valueAt(input,7,0);
		if (input.rows > 8)
			dy = valueAt(input,8,0);
	}

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


	if (A[0] != 0)
	{
		tc_writeModel( "ss2D", A );  //writes to ss2D.c and ss2D.py
	}
	else
	{
		TCFreeArray(A);
		return;
	}

	params = tc_getModelParameters(A);
	names = tc_getNames(tc_itemsOfFamilyFrom("Node\0",A));

	//index1 = tc_getFromList("Select First Variable",allNames,selected_var1,0);
	//if (index1 >= 0)
	//index2 = tc_getFromList("Select Second Variable",allNames,selected_var2,0);

	if (index1 >= 0 && index2 >= 0 && (index1 == index2))
	{
		TCFreeArray(A);
		tc_errorReport("2D steady state: cannot choose the same variable twice\0");
		return;
	}

	if (index1 >= 0 && index2 >= 0)
		index3 = tc_getFromList("Select Target",names,target_var,0);

	TCFreeArray(A);

	if (index1 < 0 || index2 < 0 || index3 < 0)
	{
		TCFreeMatrix(params);
		tc_print("2D steady state: no valid variable selected\0");
		return;
	}

	param1 = allNames[index1]; //the first parameter to vary
	param2 = allNames[index2]; //the second parameter to vary
	target = names[index3]; //the target z-axis

	strcpy(selected_var1,param1);
	strcpy(selected_var2,param2);
	strcpy(target_var,target);

	out = fopen("ss2D.c","a");

	fprintf(out , "#include \"TC_api.h\"\n\n#include \"cvodesim.h\"\n\n\
  void run() \n\
  {\n\
      Matrix dat;\n");

	fprintf(out, "\
	  int rows = (int)((%lf-%lf)/%lf);\n\
      int cols = (int)((%lf-%lf)/%lf);\n\
      double * y, *y0;\n\
      int i,j;\n\
      char * colnames[] = {\"%s\", \"%s\", \"%s\", 0};\n\
	  TCmodel * model = (TCmodel*)malloc(sizeof(TCmodel));\n\
	  (*model) = TC_initial_model;\n\
      dat.cols = 3;\n\
      dat.rows = rows * cols;\n\
      dat.colnames = colnames;\n\
      dat.values = (double*)malloc(3 * cols * rows * sizeof(double));\n\
      dat.rownames = 0;\n",
      endx,startx,dx,endy,starty,dy,param1,param2,target);

 	  fprintf(out, "\n\
      for (i=0; i < rows; ++i)\n\
      {\n\
        for (j=0; j < cols; ++j)\n\
		{\n\
		   (*model) = TC_initial_model;\n\
		   valueAt(dat,i*cols + j,0) = model->%s = %lf + i * %lf;\n\
		   valueAt(dat,i*cols + j,1) = model->%s = %lf + j * %lf;\n\
		   TCinitialize(model);\n\
		   y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit, (void*)model ,1E-4,100000.0,10);\n\
		   valueAt(dat,i*cols + j,2) = model->%s;\n\
		   if (y)\n\
			  free(y);\n\
        }\n\
		tc_showProgress(\"2-parameter steady state\",(100*i)/rows);\n\
      }\n\
	  free(model);\n\
      tc_surface(dat,\"Steady State Plot\");\n    free(dat.values);\n}\n",param1,startx, dx, param2,starty, dy, target);

	fclose(out);

	tc_compileBuildLoad("ss2D.c -lodesim\0","run\0","2-parameter steady state\0");

	TCFreeMatrix(params);
	return;
}

