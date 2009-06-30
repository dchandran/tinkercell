/****************************************************************************
 This function generates a new file with the propensity function given
 the stoichiometry matrix, rates, expressions, etc... see ssaFunction.c for use case.
 
 ****************************************************************************/
#ifndef MATRIX_TO_PROPENSITY_FUNCTION
#define MATRIX_TO_PROPENSITY_FUNCTION
 
#include "TCstructs.h"

int matrixToRates(FILE * out, Matrix N, char** rates, Matrix params, Matrix defaultParams, char** itemnames, char** expressions, double time, int maxsz, int xaxis, int p)
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

       //print header and beginning of ode function f
       fprintf( out ,"\nvoid TCssaFunc(double time, double * u, double * rates, void * data)\n{\n");
   
       //declare variables
       for (i = 0; i < m; ++i)
       {
          fprintf( out, "   double %s = u[%i];\n", N.rownames[i], i );
       }

       //set expressions values
       for (i = 0; expressions[i] != 0 && itemnames[i] != 0; ++i)
       {
          if (expressions[i][0] != '\0' && itemnames[i][0] != '\0')
             fprintf( out,"   double %s_Rate = %s;\n",itemnames[i],expressions[i]);
       }

       //print the rates
       for (i = 0; i < n; ++i)
       {
          if (i < n-1)
             fprintf( out ,"   rates[%i] = %s;\n", i, rates[i]);
          else
             fprintf( out ,"   rates[%i] = %s;\n", i, rates[i]);
       }

       fprintf( out , "}\n\n");

       fprintf( out, "double TCstoic[] = { " );
       for (i = 0; i < (n*m); ++i)
       {
          if (i < (m*n-1))
             fprintf( out ,"%lf , ", N.values[i]);
          else
             fprintf( out ,"%lf };\n", N.values[i]);
       }
   
       fprintf( out, "int TCvars = %i;\n",m);
	   fprintf( out, "int TCparams = %i;\n",params.rows);
       fprintf( out, "int TCreacs = %i;\n",n);
       fprintf( out, "double TCtime = %lf;\n",time);
       fprintf( out, "int TCmaxsz = %i;\n",maxsz);
	   fprintf( out, "int TCxAxis = %i;\n",xaxis);
       fprintf( out, "void * TCdat = 0;\n\n");
	   
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

