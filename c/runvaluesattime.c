/****************************************************************************
 
 Asks user for a parameter or variable name (string), and then generates a code that generates
 the steady state table by changing this value.
 
 ****************************************************************************/
 
#include "cvodesim.h"
#include "TC_api.h"

char * selected_var;
void run(Matrix input);
void setup();

void tc_main()
{
	selected_var = "";
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&setup, "Values at time=T0", "uses repeated simulation to compute state of system at the given time", "Parameter scan", "Plugins/c/steadystate.PNG", "", 1, 0, 0);
}

void setup()
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
   
   //tc_createInputWindow(m,"dlls/runvaluesattime","run2","At Time T");
   tc_createInputWindow(m,"At Time T",&run);
   tc_addInputWindowOptions("At Time T",0, 0, options1);
   tc_addInputWindowOptions("At Time T",1, 0, options2);
   
   return; 
}

void run(Matrix input) 
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
       return;  
   }
   
   Matrix params = tc_getParametersAndFixedVariables(A);
   TCFreeArray(A);
   
   int index = tc_getFromList("Select Independent Variable",params.rownames,selected_var,0);
   
   if (index < 0 || index > params.rows)
   {
       TCFreeMatrix(params);
	   tc_print("steady state: no variable selected\0");
	   return;
   }
   
   char * param = params.rownames[index]; //the parameter to vary
   selected_var = param;
   
   FILE * out = fopen("timet.c","a");
   
   fprintf( out , "#include \"TC_api.h\"\n#include \"cvodesim.h\"\n#include \"ssa.h\"\n\n\
void run(Matrix input) \n\
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
	  tc_showProgress(\"At Time T\",(100*i)/dat.rows);\n\
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
   
   char* cmd = malloc(80 * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"timet.c odesim.o cells_ssa.o\0");
   }
   else
   {
       sprintf(cmd,"timet.c -lodesim -lcells_ssa\0");
   }
   tc_compileBuildLoad(cmd,"run\0","At Time T\0");

   free(cmd);
   TCFreeMatrix(params);
   return;
 }

