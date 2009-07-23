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

void run();
void setup();

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&setup, "ODE simulation", "uses Sundials library (compiles to C program)", "Simulate", "Plugins/c/cvode.PNG", "", 1, 0, 1);
}

void setup()
{
   Matrix m;
   m.rows = 4;
   m.cols = 1;
   char * cols[] = { "value",0 };
   char * rows[] = { "model", "time", "step size", "plot", 0 };
   double values[] = { 0, 100, 0.1, 0 };
   char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important 
   char * options2[] = { "Variables", "Rates", 0 }; //null terminated -- very important 
   
   m.colnames = cols;
   m.rownames = rows;
   m.values = values;
   
   tc_createInputWindow(m,"ODE simulation (CVODE)",&run);
   tc_addInputWindowOptions("ODE Simulation (CVODE)",0, 0,  options1);
   tc_addInputWindowOptions("ODE Simulation (CVODE)",3, 0,  options2);
   
   return; 
}

void run(Matrix input) 
{

   double start = 0.0, end = 50.0;
   double dt = 0.1;
   int xaxis = 0;
   int selection = 0;
   int rateplot = 0;
      
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
   }
   
   int sz = (int)((end - start) / dt);
   
   Array A;
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
	   int k = tc_writeModel( "ode", A );
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
   
   FILE * out = fopen("ode.c","a");
   
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
   TCinitialize();\n\
   rates = malloc(TCreactions * sizeof(double));\n\
   double * y = ODEsim(TCvars, TCinit, &(odeFunc), %lf, %lf, %lf, 0);\n\
   free(rates);\n\
   if (!y) \
   {\n\
      tc_errorReport(\"CVode failed! Possible cause of failure: some values are reaching infinity. Double check your model.\");\n\
      return;\n\
   }\n\
   Matrix data;\n\
   data.rows = %i;\n\
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
   for (i=0; i<TCvars; ++i) data.colnames[1+i] = names[i];\n\
   FILE * out = fopen(\"ode.tab\",\"w\");\n\
   for (i=0; i < data.cols; ++i)\n\
   {\n\
      fprintf( out, data.colnames[i] );\n\
      if (i < (data.cols-1)) fprintf( out, \"\\t\" );\n\
   }\n\
   fprintf ( out, \"\\n\");\n\
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
      tc_print(\"Warning: plot is large. It can slow down TinkerCell.\\noutput written to Tinkercell/ode.tab in your home directory\");\n\
   else\n\
      tc_print(\"output written to Tinkercell/ode.tab in your home directory\");\n\
   tc_plot(data,%i,\"Time Course Simulation\",0);\n\
   free(data.colnames);  free(y);\n\
   return;\n}\n", (end-start), (end-start)/20.0, start, end, dt, sz, rateplot, xaxis);
   fclose(out);

   char* appDir = tc_appDir();

   sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*4 + 50) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"ode.c \"%s\"/c/ssa.o \"%s\"/c/odesim.o -I\"%s\"/include -I\"%s\"/c\0",appDir,appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"ode.c -I%s/c -L%s/lib -lodesim -lssa\0",appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0","Deterministic simulation\0");
/*   
   if (tc_isWindows())
   {
       tc_compileBuildLoad("c/odesim.o ode.c -I./include -I./c\0","run\0");
   }
   else
   {
       tc_compileBuildLoad("ode.c -I./c -L./lib -lodesim\0","run\0");
   }
*/   
   free(cmd);
   return;
 
 }
 
 
   
   
