/****************************************************************************
 **
 ** constructs all possible binding and unbinding events from basic koff/kon information
 **
 ****************************************************************************/
 
#include "TC_api.h"
#include "fullBindingKinetics.c"

void run();
void setup();

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&run, "Load full binding kinetics", "use on the target of a binding reaction to generate all possible states", "Generate kinetics", "Plugins/c/fullBinding.png", "", 1, 0, 0);
}

void run()
{
  ArrayOfItems selected = tc_selectedItems();
  void* p; 
  ArrayOfItems C;
  int i, j, k, N = 0;
  ArrayOfItems js, tfs, parts;
  ArrayOfStrings names, jnames;
  Matrix m;
  
  p = ithValue(selected,0);
  if (p == 0) return;

  //if (! tc_isA(p,"Regulator")) return;

  C = tc_getConnections(p);

  //count the number of repressors/activators
  for (i=0; i < C.length; ++i)
  {
     if (tc_isA(ithItem(C,i),"Binding"))
     {
        ++N;
     }
  }
  
  
  js = newArrayOfItems(N);

  //get kon,koff,and trans.reg. connections
  j = 0;
  for (i=0; i < C.length; ++i)
  {
     if (tc_isA(ithItem(C,i),"Binding"))
     {
        ithItemSet(js,j, ithItem(C,i));
        ++j;
     }         
  }

  //get the repressors/activators names

  tfs = newArrayOfItems(N+1);
  ithItemSet(tfs,0,p);
  k = 1;
  for (i=0; i < C.length; ++i)
  {
     if (tc_isA(ithItem(C,i),"Binding"))
     {
        parts = tc_getConnectedNodes(ithItem(C,i));
        for (j=0; i < parts.length; ++j)
        {
           if (ithItem(parts,j) != p)
           {
              ithItemSet( tfs, k, ithItem(parts,j));  //save tfs
              ++k;
           }
        }
        deleteArrayOfItems(parts);
     }
  }
  
  names = tc_getNames(tfs);  //get names of proteins
  jnames = tc_getNames(js);  //get names of reactions

  //main function that generates the full stoichiometry and rates
  m = fullBindingKinetics(N,jnames.strings,names.strings);

  //output that matrix to screen and item
  tc_printTable(m);
  tc_setRates(js,m.colnames);
  if (m.colnames.strings)  free(m.colnames.strings);
  m.colnames = newArrayOfStrings(0);
  tc_setStoichiometry(js,m);

  deleteArrayOfItems(js); 
  deleteArrayOfItems(tfs);  
  deleteMatrix(m);

  deleteArrayOfString(names);
  deleteArrayOfString(jnames);
  deleteArrayOfItems(selected);
  return; 
}
