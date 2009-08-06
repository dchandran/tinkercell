/****************************************************************************
 This function generates a new file with the propensity function for
 the system that is being displayed in Tinkercell. This function is to be used
 in conjunction with another function that can utilize the propensity funtion
 file to perform some other analysis.
 
 ****************************************************************************/
#ifndef TC_GENERATE_SSA_FUNCTION
#define TC_GENERATE_SSA_FUNCTION
 
#include "TC_api.h"
#include "matrixToRates.c"

int generateSSAfunction(Matrix input,char * filename)
{
   int maxsz = 100000;
   double time = 50.0;
   int xaxis = 0;
   
   if (input.cols > 0)
   {
      if (input.rows > 0)
         time = valueAt(input,0,0);
	  if (input.rows > 1)
	     maxsz = (int)valueAt(input,1,0);
	  if (input.rows > 2)
	     xaxis = (int)valueAt(input,2,0);
   }

   Array A = tc_selectedItems();
   Matrix N = tc_getStoichiometry(A);
   
   if (N.rows < 1 || N.cols < 1)
   {
       TCFreeArray(A);
	   TCFreeMatrix(N);
	   A = tc_allItems();
	   N = tc_getStoichiometry(A);
   }
   char ** rates = tc_getRates(A);
   
   int m = N.rows, n = N.cols;
   if (m>0 && n>0)
   {
	   char * exclude[3] = { "Concentration\0", "PoPS\0" ,0 };
	   Matrix params = tc_getParametersExcept(A,exclude);
	   Array A2 = tc_itemsOfFamily("Part");
	   
	   Matrix defaultParams = tc_getDefaultParameters(A2);
	   
	   char * texts[2] = { "Rate\0", 0 };
	   char** expressions = tc_getAllTextNamed(A2, texts);
	   char** itemnames = tc_getNames(A2); 
   
       FILE * out = fopen(filename,"w");
       fprintf( out ,"#include \"TC_api.h\"\n\n");
       
	   matrixToRates(out, N, rates, params, defaultParams, itemnames, expressions, time, maxsz, xaxis, 0);
	   
	   
       fclose(out);
	   
	   TCFreeArray(A2);
	   TCFreeMatrix(defaultParams);
	   TCFreeMatrix(params);
       TCFreeChars(expressions);
       TCFreeChars(itemnames);
   
   }
   
   /*tc_deleteMatrix(N);
   tc_deleteMatrix(params);
   tc_deleteArray(A);
   tc_deleteStrings(rates);
   tc_deleteStrings(pops);
   tc_deleteStrings(itemnames);*/
   
   TCFreeArray(A);
   TCFreeMatrix(N);
   TCFreeChars(rates);

   return 1; 
}
#endif
