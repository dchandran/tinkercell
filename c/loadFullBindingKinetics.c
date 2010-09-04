/****************************************************************************
 **
 ** constructs all possible binding and unbinding events from basic koff/kon information
 **
 ****************************************************************************/
 
#include "TC_api.h"
#include "fullBindingKinetics.c"

void run();
void setup();

TCAPIEXPORT void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?
	tc_addFunction(&run, "Load full binding kinetics", "use on the target of a binding reaction to generate all possible states", "Generate kinetics", "plugins/c/fullBinding.png", "", 1, 0, 0);
}

void run()
{
  tc_items selected = tc_selectedItems();
  void* p; 
  tc_items C;
  int i, j, k, N = 0;
  tc_items js, tfs, parts;
  tc_strings names, jnames;
  tc_matrix m;
  
  p = tc_getItem(selected,0);
  if (p == 0) return;

  //if (! tc_isA(p,"Regulator")) return;

  C = tc_getConnections(p);

  //count the number of repressors/activators
  for (i=0; i < C.length; ++i)
  {
     if (tc_isA(tc_getItem(C,i),"Binding"))
     {
        ++N;
     }
  }
  
  
  js = tc_createItemsArray(N);

  //get kon,koff,and trans.reg. connections
  j = 0;
  for (i=0; i < C.length; ++i)
  {
     if (tc_isA(tc_getItem(C,i),"Binding"))
     {
        tc_setItem(js,j, tc_getItem(C,i));
        ++j;
     }         
  }

  //get the repressors/activators names

  tfs = tc_createItemsArray(N+1);
  tc_setItem(tfs,0,p);
  k = 1;
  for (i=0; i < C.length; ++i)
  {
     if (tc_isA(tc_getItem(C,i),"Binding"))
     {
        parts = tc_getConnectedNodes(tc_getItem(C,i));
        for (j=0; i < parts.length; ++j)
        {
           if (tc_getItem(parts,j) != p)
           {
              tc_setItem( tfs, k, tc_getItem(parts,j));  //save tfs
              ++k;
           }
        }
        tc_deleteItemsArray(&parts);
     }
  }
  
  names = tc_getUniqueNames(tfs);  //get names of proteins
  jnames = tc_getUniqueNames(js);  //get names of reactions

  //main function that generates the full stoichiometry and rates
  m = fullBindingKinetics(N,jnames.strings,names.strings);

  //output that matrix to screen and item
  tc_printTable(m);
  tc_setRates(js,m.colnames);
  if (m.colnames.strings)  free(m.colnames.strings);
  m.colnames = tc_createStringsArray(0);
  tc_setStoichiometry(js,m);

  tc_deleteItemsArray(&js); 
  tc_deleteItemsArray(&tfs);  
  tc_deleteMatrix(&m);

  tc_deleteStringsArray(&names);
  tc_deleteStringsArray(&jnames);
  tc_deleteItemsArray(&selected);
  return; 
}
