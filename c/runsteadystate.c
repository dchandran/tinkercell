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

void run(Matrix input);
void run2D(Matrix input);
void setup1();
void setup2();

void tc_main()
{
	strcpy(selected_var,"\0");
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&setup1, "Steady state analysis", "uses Sundials library (compiles to C program)", "Parameter scan", "Plugins/c/cvode.PNG", "", 1, 0, 0);
	tc_addFunction(&setup2, "2-Parameter Steady state analysis", "uses Sundials library (compiles to C program)", "Parameter scan", "Plugins/c/cvode.PNG", "", 1, 0, 0);
}

void setup1()
{
   int i;
   Matrix m;
   m.rows = 5;
   m.cols = 1;
   char * cols[] = { "value" };
   char * rows[] = { "model", "start", "end", "increments", "plot", 0 };
   double values[] = { 0.0, 0.0, 10, 0.1, 0 };
   m.colnames = cols;
   m.rownames = rows;
   m.values = values;
   
   char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important 
   char * options2[] = { "Variables", "Rates", 0 }; //null terminated -- very important 
   tc_createInputWindow(m,"Steady state analysis",&run);
   tc_addInputWindowOptions("Steady state analysis",0, 0, options1);
   tc_addInputWindowOptions("Steady state analysis",4, 0, options2);
   return; 
}

void setup2()
{
   int i;
   Matrix m;
   m.rows = 7;
   m.cols = 1;
   char * cols[] = { "value" };
   char * rows[] = { "model", "x-start", "x-end", "x-increment size", "y-start", "y-end", "y-increments size", 0 };
   double values[] = { 0.0, 0.0, 10, 0.1, 0.0, 10, 0.1 };
   m.colnames = cols;
   m.rownames = rows;
   m.values = values;
   
   char * options1[] = { "Full model", "Selected only", 0 }; //null terminated -- very important 
   tc_createInputWindow(m,"2-D Steady state analysis",&run2D);
   tc_addInputWindowOptions("2-D Steady state analysis",0, 0, options1);
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
   
   if (A[0] != 0)
   {
	   tc_writeModel( "ss", A );  //writes to ss.c and ss.py
   }
   else
   {
       TCFreeArray(A);
       return;  
   }
   
   Matrix params = tc_getModelParameters(A);
   char ** names = tc_getNames(tc_itemsOfFamilyFrom("Species\0",A));
   
   int len = 0;
   while (names[len]) ++len;
   
   char ** allNames = malloc((len+params.rows+1)*sizeof(char*));
   
   int i;
   
   for (i=0; i < params.rows; ++i) allNames[i] = params.rownames[i];
   
   for (i=0; i < len; ++i) allNames[i+params.rows] = names[i];
   
   allNames[(len+params.rows)] = 0;
   
   
   index = tc_getFromList("Select Independent Variable",allNames,selected_var,0);
   
   TCFreeArray(A);   
   
   if (index < 0 || index >= (params.rows+len))
   {
       TCFreeMatrix(params);
	   tc_print("steady state: no valid variable selected\0");
	   return;
   }
   
   char * param = allNames[index]; //the parameter to vary
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
   TCinitialize();\n\
   for (i=0; i < dat.rows; ++i)\n\
   {\n\
      valueAt(dat,i,0) = %s;\n\
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
	  TCinitialize();\n\
      %s = (i+1) * %lf;\n\
	  TCreinitialize();\n\
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
   
   char* cmd = malloc( 50 * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"ss.c odesim.o\0");
   }
   else
   {
       sprintf(cmd,"ss.c -lodesim\0");
   }
   tc_compileBuildLoad(cmd,"run\0","Steady state\0");

   free(allNames);
   TCFreeChars(names);
   TCFreeMatrix(params);
   free(cmd);
   return;  
 }

void run2D(Matrix input) 
{ 
   double startx = 0.0, endx = 50.0, starty = 0.0, endy = 50.0;
   double dx = 0.1, dy = 0.1;
   int selection = 0;
   int index1 = 0, index2 = 1, index3 = 2;
   int rateplot = 0;
   
   if (input.cols > 0)
   {
	  if (input.rows > 0)
	     selection = (int)valueAt(input,0,0);
      if (input.rows > 1)
         startx = valueAt(input,1,0);
      if (input.rows > 2)
   	     endx = valueAt(input,2,0);
	  if (input.rows > 3)
	     dx = valueAt(input,3,0);
		 
	  if (input.rows > 4)
         starty = valueAt(input,4,0);
      if (input.rows > 5)
   	     endy = valueAt(input,5,0);
	  if (input.rows > 6)
	     dy = valueAt(input,6,0);
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
   
   
   if (A[0] != 0)
   {
	   tc_writeModel( "ss2D", A );  //writes to ss2D.c and ss2D.py
   }
   else
   {
       TCFreeArray(A);
       return;  
   }
   
   Matrix params = tc_getModelParameters(A);
   char ** names = tc_getNames(tc_itemsOfFamilyFrom("Node\0",A));
   
   int len = 0;
   while (names[len]) ++len;
   
   char ** allNames = malloc((len+params.rows+1)*sizeof(char*));
   
   int i;
   
   for (i=0; i < params.rows; ++i) allNames[i] = params.rownames[i];
   
   for (i=0; i < len; ++i) allNames[i+params.rows] = names[i];
   
   allNames[(len+params.rows)] = 0;   
   
   index1 = tc_getFromList("Select First Variable",allNames,selected_var1,0);   
   index2 = tc_getFromList("Select Second Variable",allNames,selected_var2,0);
   index3 = tc_getFromList("Select Target",allNames,names,0);
   
   TCFreeArray(A);   
   
   if (index1 < 0 || index1 >= (params.rows+len) || index2 < 0 || index2 >= (params.rows+len) || index3 < 0 || index3 > len)
   {
       TCFreeMatrix(params);
	   tc_print("2D steady state: no valid variable selected\0");
	   return;
   }
   
   char * param1 = allNames[index1]; //the first parameter to vary
   char * param2 = allNames[index2]; //the second parameter to vary
   char * target = names[index3]; //the target z-axis
   
   strcpy(selected_var1,param1);
   strcpy(selected_var2,param2);
   strcpy(target_var,target);
   
   FILE * out = fopen("ss2D.c","a");
   
   fprintf( out , "#include \"TC_api.h\"\n\n#include \"cvodesim.h\"\n\n\
void run(Matrix input) \n\
{\n   Matrix dat;\n" );
   
   fprintf( out, "   \
   dat.rows = (int)((%lf-%lf)/%lf);\n\
   dat.cols = (int)((%lf-%lf)/%lf);\n\
   int i,j;\n\
   dat.colnames = malloc( (1+dat.cols) * sizeof(char*) );\n\
   for(i=0; i<TCvars; ++i) dat.colnames[i] = \"\";\n\
   if (dat.cols > 3) \n\
   {\n\
      dat.colnames[0] = %s;\n\
	  dat.colnames[1] = %s;\n\
	  dat.colnames[2] = %s;\n\
   }\n\
   dat.values = malloc(dat.cols * dat.rows * sizeof(double));\n\
   dat.rownames = 0;\n\";\n",
   endx,startx,dx,endy,starty,dy,param1,param2,target);
   
   fprintf( out, "\n\
   %s = %lf;\n\
   for (i=0; i < dat.rows; ++i)\n\
   {\n\
      %s = %lf;\n\
      for (j=0; j < dat.cols; ++j)\n\
      {\n\
          TCinitialize();\n\
          double * y = steadyState2(TCvars,TCreactions,TCstoic, &(TCpropensity), TCinit,0,1E-4,100000.0,10);\n\
          valueAt(dat,i,j) = %s;\n\
          if (y)\n\
             free(y);\n\
          TCinitialize();\n\
          %s = (i+1) * %lf;\n\
		  %s = (j+1) * %lf;\n\
          TCreinitialize();\n\
      }\n\
	  tc_showProgress(\"2-parameter steady state\",(100*i)/dat.rows);\n\
   }\n\
   FILE * out = fopen(\"ss2D.tab\",\"w\");\n\
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
   tc_surface(dat,0,\"Steady State Plot\",0);\n\
   free(dat.colnames);\n}\n",param1,startx, param2,starty, target, param1,dx, param2, dy);

   fclose(out);
   
   char* cmd = malloc( 50 * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"ss.c odesim.o\0");
   }
   else
   {
       sprintf(cmd,"ss.c -lodesim\0");
   }
   tc_compileBuildLoad(cmd,"run\0","2-parameter steady state\0");

   free(allNames);
   TCFreeChars(names);
   TCFreeMatrix(params);
   free(cmd);
   return;  
 }

