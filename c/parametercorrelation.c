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

char selected_var1[100];
char selected_var2[100];
char target_x_var[100];
char target_y_var[100];

void run(Matrix input);
void setup();

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
	tc_addFunction(&setup, "Correlation Test", "check how varying a parameter affects the qualitative behavior of the steady state (uses 20 points for steady state curve)", "Parameter scan", "Plugins/c/steadyState.PNG", "", 1, 0, 0);
}

void setup()
{
	int i;
	Matrix m;
	char * cols[] = { "value" };
	char * rows[] = { "model", "var start", "var end", "parameter 1 start", "parameter 1 end", "parameter 2 start", "parameter 2 end", "increments size", 0 };
	double values[] = { 0.0, 0.0, 10.0, 0.0, 10.0, 0.0, 10.0, 1.0 };
	char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important 
	
	m.rows = 8;
	m.cols = 1;
	m.colnames = cols;
	m.rownames = rows;
	m.values = values;


	tc_createInputWindow(m,"Correlation Test",&run);
	tc_addInputWindowOptions("Correlation Test",0, 0,  options1);
	
	return; 
}

void run(Matrix input) 
{
	double startx = 0.0, endx = 50.0, starty = 0.0, endy = 50.0, startvar = 0.0, endvar = 100.0;
	double dx = 0.1, dy = 0.1, dvar = 5.0;
	int selection = 0;
	int index1 = 0, index2 = 1, index3 = 2, index4 = 3;
	int minsz = 10, arraysz = 20;
	int rateplot = 0;
	Array A;
	int i, len;
	Matrix params;
	char ** names, ** allNames;
	char * param1, * param2, * target, * var, * cmd;
	FILE * out;

	if (input.cols > 0)
	{
		if (input.rows > 0)
			selection = (int)valueAt(input,0,0);
		
		if (input.rows > 1)
			startvar = valueAt(input,1,0);
		if (input.rows > 2)
		{
			endvar = valueAt(input,2,0);
			dvar = (endvar - startvar)/((double)arraysz);
		}
			
		if (input.rows > 3)
			startx = valueAt(input,3,0);
		if (input.rows > 4)
			endx = valueAt(input,4,0);
			
		if (input.rows > 5)
			starty = valueAt(input,5,0);
		if (input.rows > 6)
			endy = valueAt(input,6,0);
		if (input.rows > 7)
			dy = dx = valueAt(input,7,0);
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
		tc_writeModel( "corr", A );  //writes to ss2D.c and ss2D.py
	}
	else
	{
		TCFreeArray(A);
		return;  
	}

	params = tc_getModelParameters(A);
	names = tc_getNames(tc_itemsOfFamilyFrom("Node\0",A));

	len = 0;
	while (names[len]) ++len;

	allNames = malloc((len+params.rows+1)*sizeof(char*));

	for (i=0; i < params.rows; ++i) allNames[i] = params.rownames[i];

	for (i=0; i < len; ++i) allNames[i+params.rows] = names[i];

	allNames[(len+params.rows)] = 0;   

	index1 = tc_getFromList("Select First Parameter",allNames,selected_var1,0); 
	if (index1 >= 0)
		index2 = tc_getFromList("Select Second Parameter",allNames,selected_var2,0);
	if (index1 >= 0 && index2 >= 0)
		index3 = tc_getFromList("Select Variable for Steady State Analysis",allNames,target_x_var,0);
	if (index1 >= 0 && index2 >= 0 && index3 >= 0 &&
		(index1 == index2 || index1 == index3 || index2 == index3))
	
	{
		TCFreeArray(A);   
		TCFreeMatrix(params);
		tc_errorReport("Correlation Text: cannot choose the same variable twice\0");
		return;
	}
	
	if (index1 >= 0 && index2 >= 0 && index3 >= 0)
		index4 = tc_getFromList("Select Target for Steady State Analysis",names,target_y_var,0);

	TCFreeArray(A);   

	if (index1 < 0 || index1 >= (params.rows+len) || 
		index2 < 0 || index2 >= (params.rows+len) || 
		index3 < 0 || index3 >= (params.rows+len) ||
		index4 < 0 || index4 > len)
	{
		TCFreeMatrix(params);
		tc_print("Correlation Text: no valid variable selected\0");
		return;
	}


	param1 = allNames[index1]; //the first parameter to vary
	param2 = allNames[index2]; //the second parameter to vary
	var = allNames[index3]; //the var for steady state
	target = names[index4]; //the target for steady state

	strcpy(selected_var1,param1);
	strcpy(selected_var2,param2);
	strcpy(target_x_var,var);
	strcpy(target_y_var,target);

	out = fopen("corr.c","a");

	fprintf( out , "#include \"TC_api.h\"\n#include \"cvodesim.h\"\n#include \"correlation.c\"\n\n\
				   void run() \n\
				   {\n\
					Matrix dat;\n\
					int i,j,k;\n" );

	fprintf( out, "   \
				  dat.rows = (int)((%lf-%lf)/%lf);\n\
				  dat.cols = (int)((%lf-%lf)/%lf);\n\
				  dat.colnames = malloc( (1+dat.cols) * sizeof(char*) );\n\
				  for(i=0; i<=dat.cols; ++i) dat.colnames[i] = 0;\n\
				  if (dat.cols > 3) \n\
				  {\n\
					dat.colnames[0] = \"%s\";\n\
					dat.colnames[1] = \"%s\";\n\
					dat.colnames[2] = \"R\";\n\
				  }\n\
				  dat.values = malloc(dat.cols * dat.rows * sizeof(double));\n\
				  dat.rownames = 0;\n",
				  endx,startx,dx,endy,starty,dy,param1,param2);

	fprintf( out, "\n\
					double * __Y = (double*)malloc(%i * sizeof(double*));\n\
					TCinitialize();\n\
					for(i=0; i < %i; ++i)\n\
					{\n\
						%s = %lf + i * %lf;\n\
						TCreinitialize();\n\
						double * y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit,0,1E-4,100000.0,10);\n\
						__Y[i] = %s;\n\
						if (y)\n\
							free(y);\n\
						TCinitialize();\n\
					}\n",arraysz,arraysz,var,startvar,dvar,target);

	fprintf( out, "\n\
				  for (i=0; i < dat.rows; ++i)\n\
				  {\n\
					  for (j=0; j < dat.cols; ++j)\n\
					  {\n\
						  %s = %lf + i * %lf;\n\
						  %s = %lf + j * %lf;\n\
						  double * __X = (double*)malloc(%i * sizeof(double*));\n\
						  TCinitialize();\n\
						  for(k=0; k < %i; ++k)\n\
						  {\n\
							%s = %lf + k * %lf;\n\
							TCreinitialize();\n\
							double * y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit,0,1E-4,100000.0,10);\n\
							__X[k] = %s;\n\
							if (y)\n\
								free(y);\n\
							TCinitialize();\n\
						  }\n\
						  valueAt(dat,i,j) = maxCorrelation(__X,__Y,%i,%i);\n\
						  if (__X)\n\
							free(__X);\n\
						}\n\
						tc_showProgress(\"2-Parameter Correlation Test\",(100*i)/dat.rows);\n\
				  }\n\
				  FILE * out = fopen(\"corr.tab\",\"w\");\n\
				  fprintf( out, \"\\n\");\n\
				  for (i=0; i < dat.rows; ++i)\n\
				  {\n\
					for (j=0; j < dat.cols; ++j)\n\
					{\n\
						fprintf( out, \"\\t%%lf\", valueAt(dat,i,j) );\n\
					}\n\
					fprintf( out, \"\\n\");\n\
				  }\n\
				  fclose(out);\n\
				  if (__Y) free(__Y);\n\
				  tc_surface(dat,\"Correlation Test\",100,100);\n\
				  free(dat.colnames);\n}\n",param1,startx, dx, param2,starty,dy,arraysz, arraysz, var,startvar, dvar, target, arraysz, minsz);

	fclose(out);

	cmd = malloc( 50 * sizeof(char));

	if (tc_isWindows())
	{
		sprintf(cmd,"corr.c odesim.o\0");
	}
	else
	{
		sprintf(cmd,"corr.c -lodesim\0");
	}
	tc_compileBuildLoad(cmd,"run\0","2-Parameter Correlation Test\0");

	free(allNames);
	TCFreeChars(names);
	TCFreeMatrix(params);
	free(cmd);
	return;  
}




