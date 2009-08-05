/****************************************************************************
 
 Asks user for a parameter or variable name (string), and then generates a code that generates
 the steady state table by changing this value.
 
 ****************************************************************************/
 
#include <string.h>

#include "TC_api.h"

char selected_var[100];

void run();
void setup();

void tc_main()
{
	strcpy(selected_var,"\0");
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&setup, "Steady state analysis", "uses Sundials library (compiles to C program)", "Parameter scan", "Plugins/c/cvode.PNG", "", 1, 0, 0);
}

void setup()
{
   int i;
   Matrix m;
   m.rows = 5;
   m.cols = 1;
   char * cols[] = { "value" };
   //char * rows[] = { "variable", "model", "start", "end", "increments", 0 };
   //double values[] = { 0.0, 0.0, 0.0, 10, 0.5 };
   char * rows[] = { "model", "start", "end", "increments", "plot", 0 };
   double values[] = { 0.0, 0.0, 10, 0.1, 0 };
   m.colnames = cols;
   m.rownames = rows;
   m.values = values;
   
   char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important 
   char * options2[] = { "Variables", "Rates", 0 }; //null terminated -- very important 
   //tc_createInputWindow(m,"dlls/runsteadystate","run2","Steady State Plot");
   tc_createInputWindow(m,"Steady state analysis",&run);
   tc_addInputWindowOptions("Steady state analysis",0, 0, options1);
   tc_addInputWindowOptions("Steady state analysis",4, 0, options2);
   //tc_addInputWindowOptions("Steady State Plot",1, 0,  params.rownames);   
   
   //TCFreeMatrix(params);
   
   return; 
}

void run(Matrix input) 
{ 
   double start = 0.0, end = 50.0;
   double dt = 0.1;
   int selection = 0;
   int index = 0;
   int rateplot = 0;
   
   if (input.cols > 0)
   {
	  if (input.rows > 0)
	     selection = (int)valueAt(input,0,0);
      if (input.rows > 1)
         start = valueAt(input,1,0);
      if (input.rows > 2)
   	     end = valueAt(input,2,0);
	  if (input.rows > 3)
	     dt = valueAt(input,3,0);
	  if (input.rows > 4)
	     rateplot = (int)valueAt(input,4,0);
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
	   tc_writeModel( "ss", A );
   }
   else
   {
       TCFreeArray(A);
       return;  
   }
   
   Matrix params = tc_getParameters(A);
   char ** names = tc_getNames(tc_itemsOfFamilyFrom("Species\0",A));
   
   int len = 0;
   while (names[len]) ++len;
   
   char ** allNames = malloc((len+params.rows+1)*sizeof(char*));
   
   int i;
   
   for (i=0; i < len; ++i) allNames[i] = names[i];
   
   for (i=len; i < params.rows; ++i) allNames[i] = params.rownames[i+len];
   
   allNames[(len+params.rows)] = 0;
   
   
   index = tc_getFromList("Select Independent Variable",allNames,selected_var,0);
   
   TCFreeArray(A);   
   TCFreeChars(names);
   free(allNames);
   
   if (index < 0 || index > params.rows)
   {
       TCFreeMatrix(params);
	   tc_print("steady state: no valid variable selected\0");
	   return;
   }
   
   char * param = params.rownames[index]; //the parameter to vary
   strcpy(selected_var,param);
   
   FILE * out = fopen("ss.c","a");
   
   fprintf( out , "#include \"TC_api.h\"\n\n#include \"cvodesim.h\"\n\n\
void run(Matrix input) \n\
{\n   Matrix dat;\n" );
   
   fprintf( out, "   dat.rows = (int)((%lf-%lf)/%lf);\n\
   int i,j;\n\
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
   %s = %lf;\n\
   for (i=0; i < dat.rows; ++i)\n\
   {\n\
      valueAt(dat,i,0) = %s;\n\
	  TCinitialize();\n\
      double * y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit,0,1E-4,100000.0,10);\n\
      if (y)\n\
      {\n\
         if (%i)\n\
         {\n\
             double * y0 = malloc(TCreactions * sizeof(double));\n\
             TCpropensity(0.0, y, y0, 0);\n\
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
         for (j=0; j<TCvars; ++j)\n\
            valueAt(dat,i,j+1) = 0;\n\
      }\n\
      %s += %lf;\n\
	  tc_showProgress(\"Steady state\",(100*i)/dat.rows);\n\
   }\n\
   FILE * out = fopen(\"ss.tab\",\"w\");\n\
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
   tc_plot(dat,0,\"Steady State Plot\",0);\n\
   free(dat.colnames);\n}\n",param,start,param,rateplot,param,dt);

   fclose(out);

   char* appDir = tc_appDir();

   sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*3 + 50) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"ss.c \"%s\"/c/odesim.o -I\"%s\"/include -I\"%s\"/c\0",appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"ss.c -I%s/c -L%s/lib -lodesim\0",appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0","Steady state\0");
/*
   if (tc_isWindows())
   {
       tc_compileBuildLoad("c/odesim.o ss.c -I./include -I./c\0","run\0");
   }
   else
   {
       tc_compileBuildLoad("ss.c -I./c -L./lib -lodesim\0","run\0");
   }
*/   
   TCFreeMatrix(params);
   free(cmd);
   return;  
 }

