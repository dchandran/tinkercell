#include "TC_api.h"
int main()
{
   int numSpecies = 5;
   char * speciesNames[] = { "A", "B", "C", "D", "E" };
   int numReactions = 5;
   char * reactionNames[] = { "j1", "j2", "j3", "j4", "j5" };
   int numReactants[] = { 2, 3, 1, 2, 1 };
   int numProducts[] = { 2, 1, 1, 1, 3 };
   char * reactionRates[] = { "k1*A", "k2*B", "k3*C", "k4*D", "k5*E" };
   char * reactantNames[] = { "A", "B" , "A", "C", "D" , "B", "A", "C" ,  "C"  };
   char * productNames[] = { "C", "D" , "D" , "A", "A", "B", "C", "D"  };

   tc_openNewWindow("Antimony");

   int i,j,k;
   for (i=0; i < numSpecies; ++i)
       tc_insert(speciesNames[i],"Species");

   int k1 = 0, k2 = 0;
   for (i=0; i < numReactions; ++i)
   {
       OBJ * reactants = malloc( (1+numReactants[i]) * sizeof(OBJ));
       reactants[ numReactants[i] ] = 0;
       OBJ * products = malloc( (1+numProducts[i]) * sizeof(OBJ));
       products[ numProducts[i] ] = 0;
       for (j=0; j < numReactants[i]; ++j)
            reactants[j] = tc_find(reactantNames[k1+j]);
       for (j=0; j < numProducts[i]; ++j)
            products[j] = tc_find(productNames[k2+j]);
       k1 += numReactants[i];
       k2 += numProducts[i];
       OBJ J = tc_insertConnection(reactants,products, reactionNames[i], "Biochemical");
       tc_setRate(J,reactionRates[i]);
       free(reactants); 
       free(products);
   }

   tc_runPythonFile("py/nxAutoLayout.py");

   return 1; 
}
