#include <stdlib.h>
#include <stdio.h>
#include "TC_api.h"

void run();
void setup();

TCAPIEXPORT void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&run, "Add N intermediate steps", "converts a single step reaction into N reactions using mass-action kinetics", "Generate kinetics", "tabasco_like.png", "PoPS", 1, 1, 0);
}

void run()
{
   int i,j,k;
   tc_items A = tc_selectedItems(), parts;
   int numSteps = (int)(tc_getNumber("number of steps:\0"));
   const char* rxnname;
   tc_strings partnames, rates;
   tc_matrix newN;
   tc_items flux = tc_createItemsArray(1);
   
   if (numSteps > 0) 
	   for (i=0; tc_getItem(A,i)!=0; ++i)
	   {
		    if (tc_isA( tc_getItem(A,i),"Connection"))
		    {
		        parts = tc_getConnectedNodes( tc_getItem(A,i) );
		        if (tc_getItem(parts,0) && tc_getItem(parts,1) && parts.length == 2)
		        {
		        	newN = tc_createMatrix(numSteps + 1,numSteps + 1);
		            
					rxnname = tc_getUniqueName(tc_getItem(A,i));
	                partnames = tc_getUniqueNames(parts);
	                tc_setRowName(newN,0, tc_getString(partnames,0) );
	                tc_setRowName(newN,newN.rows-1,tc_getString(partnames,1));
					 
	                 for (j=0; j < newN.rows; ++j)
						 for (k=0; k < newN.cols; ++k)
						        tc_setMatrixValue(newN,j,k,0.0);
				
	                 for (k=0; k < newN.cols; ++k)
	                 {
						if ((k+1) < newN.rows)
						{
							tc_setMatrixValue(newN,k,k,-1.0);														
							tc_setMatrixValue(newN,k+1,k,1.0);
						}
						else
						{
							tc_setMatrixValue(newN,k-1,k,-1.0);
						}
					
						newN.colnames.strings[k] = malloc(100 * sizeof(char));
					
						if (k > 0 && (k+1) < newN.rows)
						{
							newN.rownames.strings[k] = malloc(100 * sizeof(char));
							sprintf(newN.rownames.strings[k], "%s.I%i\0",rxnname,k);
						}
					
						if ((k+1) < newN.rows)
							sprintf(newN.colnames.strings[k], "%s.k0*%s\0",rxnname,tc_getRowName(newN,k));
						else
							sprintf(newN.colnames.strings[k], "%s.leak*%s\0",rxnname,tc_getRowName(newN,k-1));
					
					 }
					 if (tc_isA( tc_getItem(parts,0),"Part\0") )
						tc_setMatrixValue(newN,0,0, 0.0);
					 if ((tc_isA( tc_getItem(parts,1),"Terminator\0") || tc_isA( tc_getItem(parts,1),"Empty\0") )&& newN.cols > 2)
						tc_setMatrixValue(newN,newN.rows-1,newN.cols-2,0.0);
					 tc_setParameter( tc_getItem(A,i),"k0",0.1);
					 tc_setParameter( tc_getItem(A,i),"leak",0.01);
	                 tc_setItem(flux, 0, tc_getItem(A,i));
	                 rates = newN.colnames;
	                 newN.colnames = tc_createStringsArray(0);
	                 tc_setStoichiometry(flux , newN);
	                 newN.colnames = rates;
					 tc_setRates(flux,rates);
	                 tc_deleteMatrix(&newN);
		        }
		        tc_deleteItemsArray(&parts);
		    }
   	}

  tc_deleteItemsArray(&A);
  tc_deleteItemsArray(&flux);

  return; 
}
