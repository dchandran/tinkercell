#include "TC_api.h"

void run();
void setup();

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&run, "Add N intermediate steps", "converts a single step reaction into N reactions using mass-action kinetics", "Generate kinetics", "Plugins/c/tabasco_like.PNG", "Elongation", 1, 1, 0);
}

void run()
{
   int i,j,k;
   Array A = tc_selectedItems();
   int numSteps = (int)(tc_getNumber("number of steps:\0"));
   char* rxnname, ** partnames, ** rates;
   Array parts;
   OBJ flux[] = { 0 , 0 };
   
   if (numSteps > 0) 
   for (i=0; A[i]!=0; ++i)
   {
        if (tc_isA(A[i],"Connection"))
        {
            parts = tc_getConnectedNodes(A[i]);
            if (parts && parts[0] && parts[1] && (parts[2] == 0))
            {
                Matrix newN;
                newN.cols = newN.rows = numSteps + 1;
                if (newN.rows > 1)
                {
                     rxnname = tc_getName(A[i]);
                     partnames = tc_getNames(parts);
                     newN.colnames = malloc ( (1+newN.cols) * sizeof(char*) );
                     newN.colnames[newN.cols] = 0;
                     newN.rownames = malloc( (1+newN.rows) * sizeof(char*) );
                     newN.rownames[newN.rows] = 0;
                     newN.values = malloc( newN.rows * newN.cols * sizeof(double) );

                     newN.rownames[0] = partnames[0];
                     newN.rownames[ newN.rows-1 ] = partnames[1];
					 
                     for (j=0; j < newN.rows; ++j)
					     for (k=0; k < newN.cols; ++k)
					            valueAt(newN,j,k) = 0.0;
					
                     for (k=0; k < newN.cols; ++k)
                     {
						if ((k+1) < newN.rows)
						{
							valueAt(newN,k,k) = -1.0;														
							valueAt(newN,k+1,k) =  1.0;
						}
						else
						{
							valueAt(newN,k-1,k) = -1.0;
						}
						
						newN.colnames[k] = malloc(100 * sizeof(char));
						
						if (k > 0 && (k+1) < newN.rows)
						{
							newN.rownames[k] = malloc(100 * sizeof(char));
							sprintf(newN.rownames[k], "%s.I%i\0",rxnname,k);
						}
						
						if ((k+1) < newN.rows)
							sprintf(newN.colnames[k], "%s.k0*%s\0",rxnname,newN.rownames[k]);
						else
							sprintf(newN.colnames[k], "%s.leak*%s\0",rxnname,newN.rownames[k-1]);
						
					 }
					 if (tc_isA(parts[0],"Regulator\0"))
						valueAt(newN,0,0) = 0.0;
					 if (tc_isA(parts[1],"Terminator\0") && newN.cols > 2)
						valueAt(newN,newN.rows-1,newN.cols-2) = 0.0;
					 tc_setParameter(A[i],"k0",0.1);
					 tc_setParameter(A[i],"leak",0.01);
                     flux[0] = A[i];
                     rates = newN.colnames;
                     newN.colnames = 0;
                     tc_setStoichiometry(flux , newN);
                     newN.colnames = rates;
					 tc_setRates(flux,rates);
                     TCFreeMatrix(newN);
                }
            }
            TCFreeArray(parts);
        }
   }

  TCFreeArray(A);

  return; 
}
