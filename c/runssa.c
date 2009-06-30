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

int run(Matrix input)
{

   Matrix m;
   m.rows = 4;
   m.cols = 1;
   char * cols[] = { "value" };
   char * rows[] = { "model", "time", "max size", "plot", 0 };
   double values[] = { 0.0, 100, 100000, 0 };
   char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important 
   char * options2[] = { "Variables", "Rates", 0 }; //null terminated -- very important 
   m.colnames = cols;
   m.rownames = rows;
   m.values = values;
   
   tc_createInputWindow(m,"dlls/runssa","run2","Stochastic Simulation");
   tc_addInputWindowOptions("Stochastic Simulation",0, 0,  options1);
   tc_addInputWindowOptions("Stochastic Simulation",3, 0,  options2);
   
   return 1; 
}


int run2(Matrix input)
{
   int maxsz = 100000;
   double time = 50.0;
   int xaxis = 0;
   int selection = 0;
   int rateplot = 0;

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
   
   Array A;
   if (selection > 0)
   {
	   A = tc_selectedItems();
	   if (A[0] == 0)
	   {
			TCFreeArray(A);
			//A = tc_allItems();
			tc_errorReport("No Model Selected\0");
			return 0;
			
	   }
   }
   else
   {
		A = tc_allItems();
   }
   
   if (A[0] != 0)
   {
	   int k = tc_writeModel( "ssa", A );
       TCFreeArray(A);
	   if (!k)
	   {
			tc_errorReport("No Model\0");
			return 0;
	   }
   }
   else
   {
       TCFreeArray(A);
	   tc_errorReport("No Model\0");
       return 0;  
   }
   
   FILE * out = fopen("ssa.c","a");
   
   fprintf( out , "#include \"TC_api.h\"\n#include \"ssa.h\"\n\n\
static double _time0_ = 0.0;\n\
void ssaFunc(double time, double * u, double * rates, void * data)\n\
{\n\
   TCpropensity(time, u, rates, data);\n\
   if (time > _time0_)\n\
   {\n\
     tc_showProgress(\"ssa\",(int)(100 * time/%lf));\n\
	 _time0_ += %lf;\n\
   }\n\
}\n\
   \n\
   \n\
int run(Matrix input) \n\
{\n\
   initMTrand();\n\
   TCinitialize();\n\
   int sz = 0;\n\
   double * y = SSA(TCvars, TCreactions, TCstoic, &(ssaFunc), TCinit, 0, %lf, %i, &sz, 0);\n\
   if (!y) \
   {\n\
      tc_errorReport(\"SSA failed! Possible cause of failure: some values are becoming negative. Double check your model.\");\n\
      return 0;\n\
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
   return 1;\n}\n",time,time/20.0,time,maxsz,rateplot,xaxis);

   fclose(out);
   
   char* appDir = tc_appDir();
   
   int sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*3 + 50) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"ssa.c \"%s\"/c/ssa.o -I\"%s\"/include -I\"%s\"/c\0",appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"ssa.c -I%s/c -L%s/lib -lssa\0",appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0");
/*   
   if (tc_isWindows())
   {
       tc_compileBuildLoad("c/ssa.o ssa.c -I./include -I./c\0","run\0");
   }
   else
   {
       tc_compileBuildLoad("ssa.c -I./c -L./lib -lssa\0","run\0");
   }
*/   
   free(cmd);
   return 1; 
}
