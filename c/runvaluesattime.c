/****************************************************************************
 
 Asks user for a parameter or variable name (string), and then generates a code that generates
 the steady state table by changing this value.
 
 ****************************************************************************/
 
#include "cvodesim.h"
#include "TC_api.h"
int run(Matrix input)
{
   Matrix m;
   m.rows = 6;
   m.cols = 1;
   char * cols[] = { "value", 0 };
   char * rows[] = { "model", "simulation", "start", "end", "increments", "time" };
   double values[] = { 0.0, 0.0, 0.0, 10, 0.5 , 100.0  };
   char * options1[] = { "Full model", "Selected only", 0};
   char * options2[] = { "ODE", "Stochastic", 0  }; //null terminated -- very very important 
   m.colnames = cols;
   m.rownames = rows;
   m.values = values;
   
   tc_createInputWindow(m,"dlls/runvaluesattime","run2","At Time T");
   tc_addInputWindowOptions("At Time T",0, 0, options1);
   tc_addInputWindowOptions("At Time T",1, 0, options2);
   
   return 1; 
}

int run2(Matrix input) 
{ 
   double start = 0.0, end = 50.0;
   double dt = 0.1, time = 100.0;
   int doStochastic = 0;
   int selection = 0;
   
   if (input.cols > 0)
   {
	  if (input.rows > 0)
	     selection = (int)valueAt(input,0,0);
	  if (input.rows > 1)
	     doStochastic = (int)(valueAt(input,1,0) > 0);
      if (input.rows > 2)
         start = valueAt(input,2,0);
      if (input.rows > 3)
   	     end = valueAt(input,3,0);
	  if (input.rows > 4)
	     dt = valueAt(input,4,0);
	  if (input.rows > 5)
	     time = valueAt(input,5,0);
   }
   
   Array A;
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
   
   int sz = (int)((end - start) / dt);
   
   if (A[0] != 0)
   {
	   tc_writeModel( "timet", A );
   }
   else
   {
       TCFreeArray(A);
       return 0;  
   }
   
   Matrix params = tc_getParametersAndFixedVariables(A);
   TCFreeArray(A);
   
   int index = tc_getFromList("Select Independent Variable",params.rownames,0);
   
   if (index < 0 || index > params.rows)
   {
       TCFreeMatrix(params);
	   tc_print("steady state: no variable selected\0");
	   return 0;
   }
   
   char * param = params.rownames[index]; //the parameter to vary
   
   FILE * out = fopen("timet.c","a");
   
   fprintf( out , "#include \"TC_api.h\"\n#include \"cvodesim.h\"\n#include \"ssa.h\"\n\n\
int run(Matrix input) \n\
{\n   initMTrand();\n   Matrix dat;\n" );
   
   fprintf( out, "   dat.rows = (int)((%lf-%lf)/%lf);\n\
   dat.cols = 1+TCvars;\n\
   double * values = malloc(dat.cols * dat.rows * sizeof(double));\n\
   dat.values = values;\n\
   dat.rownames = 0;\n\
   dat.colnames = malloc( (1+TCvars) * sizeof(char*) );\n\
   int i,j;\n\
   for(i=0; i<TCvars; ++i) dat.colnames[1+i] = TCvarnames[i];\n\
   dat.colnames[0] = \"%s\";\n",end,start,dt,param);
   
   fprintf( out, "\n\
   %s = %lf;\n\
   for (i=0; i < dat.rows; ++i)\n\
   {\n\
      valueAt(dat,i,0) = %s;\n\
	  TCinitialize();\n\
      double * y = 0;\n\
	  int sz = (int)(%lf*10.0);\n\
	  if (%i)\n\
	      y = SSA(TCvars, TCreactions, TCstoic, &(TCpropensity), TCinit, 0, %lf, 200000, &sz, 0);\n\
	  else \n\
	      y = ODEsim2(TCvars, TCreactions, TCstoic, &(TCpropensity),TCinit, 0, %lf, 0.1, 0);\n\
      if (y)\n\
      {\n\
         for (j=0; j<TCvars; ++j)\n\
            valueAt(dat,i,j+1) = getValue(y,1+TCvars,sz-1,j+1);\n\
         free(y);\n\
      }\n\
      else\n\
      {\n\
         for (j=0; j<TCvars; ++j)\n\
            valueAt(dat,i,j+1) = 0;\n\
      }\n\
      %s += %lf;\n\
	  tc_showProgress(\"timet\",(100*i)/dat.rows);\n\
   }\n\
   FILE * out = fopen(\"valuet.tab\",\"w\");\n\
   for (i=0; i < dat.cols; ++i)\n\
   {\n\
      fprintf( out, dat.colnames[i] );\n\
      fprintf( out, \"\\t\" );\n\
   }\n\
   fprintf( out, \"\\n\");\n\
   for (i=0; i < dat.rows; ++i)\n\
   {\n\
      for (j=0; j < dat.cols; ++j)\n\
	  {\n\
	    if (j==0)\n\
	       fprintf( out, \"%%lf\", valueAt(dat,i,j) );\n\
		else   \n\
		   fprintf( out, \"\\t%%lf\", valueAt(dat,i,j) );\n\
	  }\n\
	  fprintf( out, \"\\n\");\n\
   }\n\
   fclose(out);\n\
   tc_plot(dat,0,\"At time=%lf\",0);\n\
   free(dat.colnames);\n}\n",param,start,param,time,doStochastic,time,time,param,dt,time);

   fclose(out);
   
   char* appDir = tc_appDir();

   sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*4 + 80) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"timet.c \"%s\"/c/odesim.o \"%s\"/c/ssa.o -I\"%s\"/include -I\"%s\"/c\0",appDir,appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"timet.c -I%s/c -L%s/lib -lodesim -lssa\0",appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0");
/*   
   if (tc_isWindows())
   {
       tc_compileBuildLoad("c/odesim.o c/ssa.o timet.c -I./include -I./c\0","run\0");
   }
   else
   {
       tc_compileBuildLoad("timet.c -I./c -L./lib -lodesim -lssa\0","run\0");
   }
*/
   free(cmd);
   TCFreeMatrix(params);
   return 1;
 }

