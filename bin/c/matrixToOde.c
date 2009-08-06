/****************************************************************************
 
 This function generates a new file with the differential equation model given
 the stoichiometry matrix, rates, expressions, etc.. see odeFunctions.c for use case
 
 ****************************************************************************/
 
#ifndef MATRIX_TO_ODE_FUNCTION
#define MATRIX_TO_ODE_FUNCTION

#include "TCstructs.h"

void matrixToOde(FILE * out, Matrix N, char** rates, Matrix params, Matrix defaultParams, char** itemnames, char** expressions,double start, double end, double dt, int xaxis, int p)
{
	   if (!out || !rates || !itemnames || !expressions) return;
	   
	   int m = N.rows, n = N.cols;
	   
       int i = 0, j = 0;
       OBJ temp;
       //set parameter values
       
	   for (i = 0; i < params.rows; ++i)
       {
           fprintf( out,"double %s = %lf;\n",params.rownames[i],valueAt(params,i,0));
       }
	   
	   for (i = 0; i < defaultParams.rows; ++i)
       {
           fprintf( out,"double %s = %lf;\n",defaultParams.rownames[i],valueAt(defaultParams,i,0));
       }
	   
	   //write assign parameters function (not needed for simulation, but could be useful for some purpose)
		fprintf( out , "\nvoid assignParameters( double * k )\n { \n" );
		for (i = 0; i < params.rows; ++i)
		{
		  fprintf( out,"   %s = k[%i];\n",params.rownames[i],i);
		}
		fprintf( out , "}\n\n" );
		
		//declare variables
       for (i = 0; i < m; ++i)
       {
          fprintf( out, "double %s = 0.0;\n", N.rownames[i]);
       }
	   
       fprintf( out , "\nvoid TCodeFunc( double time, double * u, double * du, void * udata )\n { \n" );

       //declare variables
       for (i = 0; i < m; ++i)
       {
          fprintf( out, "   double %s = u[%i];\n", N.rownames[i],i );
       }
	   
	   //set expressions values
       for (i = 0; expressions[i] != 0 && itemnames[i] != 0; ++i)
       {
          if (expressions[i][0] != '\0' && itemnames[i][0] != '\0')
             fprintf( out,"   double %s_Rate = %s;\n",itemnames[i],expressions[i]);
       }
	
       fprintf( out,"   double rates[] = {");

	   //print the rates
	   for (i = 0; i < n; ++i)
	   {
		   if (i < n-1)
			  fprintf( out ,"%s , ", rates[i]);
		   else
			  fprintf( out ,"%s };\n", rates[i]);
	   }

	   //print odes
	   for (i = 0; i < m; ++i)
	   {
		  fprintf(out, "   du[%i] = 0",i);
		  for (j = 0; j < n; ++j)
		  {
			 fprintf( out ," + (%lf)*rates[%i]", valueAt(N,i,j), j);
		  }
		  fprintf(out , ";\n");
	   }
       fprintf( out , "}\n\n");
	   
	   //write simulation information
	   fprintf( out, "int TCvars = %i;\n",m);
	   fprintf( out, "int TCparams = %i;\n",params.rows);
	   fprintf( out, "double TCstart = %lf;\n",start);
	   fprintf( out, "double TCend = %lf;\n",end);
	   fprintf( out, "double TCstep = %lf;\n",dt);
	   fprintf( out, "void * TCdat = 0;\n");
	   fprintf( out, "int TCxAxis = %i;\n",xaxis);
	   //col names
       fprintf( out, "char * TCvarnames[] = {%s","\"time\\0\"");
  
       for (i=0; i< m; ++i)
          fprintf( out, ",\"%s\\0\"",N.rownames[i]);
   
       fprintf( out, "};\n\n"); //end of colnames
	   
	   fprintf( out, "char * TCparamnames[] = {");
   
       for (i=0; i < params.rows; ++i)
         if (i == 0)
	        fprintf( out, "\"%s\\0\"",params.rownames[i]);
	     else
            fprintf( out, ",\"%s\\0\"",params.rownames[i]);

       fprintf( out, "};\n\n"); //end of param names
	   
	   fprintf( out , "double TCinit[%i];\n", m);
	   
	   fprintf( out ,"\nvoid TCinitialize()\n{\n");
	   
	   for (i = 0; i < defaultParams.rows && itemnames[i] != 0; ++i)
       {
		    if (defaultParams.rownames[i][0] != '\0' && itemnames[i][0] != '\0')
			{
				fprintf( out ,"    double %s =",itemnames[i]);
	            fprintf( out ," %s;\n", defaultParams.rownames[i]);
			}
       }
	   
	   for (i = 0; i < N.rows; ++i)
	   {
            fprintf( out ,"    TCinit[%i] = %s;\n", i, N.rownames[i]);
       }
	   
	   fprintf(out,"}\n\n");
}
#endif
