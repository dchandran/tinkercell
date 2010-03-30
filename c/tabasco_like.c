#include "TC_api.h"

void run();
void setup();

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&run, "Add N intermediate steps", "converts a single step reaction into N reactions using mass-action kinetics", "Generate kinetics", "Plugins/c/tabasco_like.png", "Elongation", 1, 1, 0);
}

void run()
{
   int i,j,k;
   ArrayOfItems A = tc_selectedItems(), parts;
   int numSteps = (int)(tc_getNumber("number of steps:\0"));
   const char* rxnname;
   ArrayOfStrings partnames, rates;
   Matrix newN;
   ArrayOfItems flux = newArrayOfItems(1);
   
   if (numSteps > 0) 
	   for (i=0; ithItem(A,i)!=0; ++i)
	   {
		    if (tc_isA( ithItem(A,i),"Connection"))
		    {
		        parts = tc_getConnectedNodes( ithItem(A,i) );
		        if (ithItem(parts,0) && ithItem(parts,1) && parts.length == 2)
		        {
		        	newN = newMatrix(numSteps + 1,numSteps + 1);
		            
					rxnname = tc_getName(ithItem(A,i));
	                partnames = tc_getNames(parts);
	                setRowName(newN,0, ithString(partnames,0) );
	                setRowName(newN,newN.rows-1,ithString(partnames,1));
					 
	                 for (j=0; j < newN.rows; ++j)
						 for (k=0; k < newN.cols; ++k)
						        setValue(newN,j,k,0.0);
				
	                 for (k=0; k < newN.cols; ++k)
	                 {
						if ((k+1) < newN.rows)
						{
							setValue(newN,k,k,-1.0);														
							setValue(newN,k+1,k,1.0);
						}
						else
						{
							setValue(newN,k-1,k,-1.0);
						}
					
						newN.colnames.strings[k] = malloc(100 * sizeof(char));
					
						if (k > 0 && (k+1) < newN.rows)
						{
							newN.rownames.strings[k] = malloc(100 * sizeof(char));
							sprintf(newN.rownames.strings[k], "%s.I%i\0",rxnname,k);
						}
					
						if ((k+1) < newN.rows)
							sprintf(newN.colnames.strings[k], "%s.k0*%s\0",rxnname,getRowName(newN,k));
						else
							sprintf(newN.colnames.strings[k], "%s.leak*%s\0",rxnname,getRowName(newN,k-1));
					
					 }
					 if (tc_isA( ithItem(parts,0),"Promoter\0") )
						setValue(newN,0,0, 0.0);
					 if ((tc_isA( ithItem(parts,1),"Terminator\0") || tc_isA( ithItem(parts,1),"Empty\0") )&& newN.cols > 2)
						setValue(newN,newN.rows-1,newN.cols-2,0.0);
					 tc_setParameter( ithItem(A,i),"k0",0.1);
					 tc_setParameter( ithItem(A,i),"leak",0.01);
	                 ithItemSet(flux, 0, ithItem(A,i));
	                 rates = newN.colnames;
	                 newN.colnames = newArrayOfStrings(0);
	                 tc_setStoichiometry(flux , newN);
	                 newN.colnames = rates;
					 tc_setRates(flux,rates);
	                 deleteMatrix(newN);
		        }
		        deleteArrayOfItems(parts);
		    }
   	}

  deleteArrayOfItems(A);
  deleteArrayOfItems(flux);

  return; 
}
