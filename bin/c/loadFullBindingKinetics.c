/****************************************************************************
 **
 ** constructs all possible binding and unbinding events from basic koff/kon information
 **
 ****************************************************************************/
 
#include "TC_api.h"
#include "fullBindingKinetics.c"

void run();
void setup();

int functionMissing()
{
	if (!tc_addFunction || 
		!tc_getConnections ||
		!tc_getConnectedNodes ||
		!tc_getNames)
		
		return 1;		
		
	return 0;
}

void tc_main()
{
	if (functionMissing()) return;
	
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&run, "Load full binding kinetics", "use on the target of a binding reaction to generate all possible states", "Generate kinetics", "Plugins/c/fullBinding.PNG", "", 1, 0, 0);
}

void run()
{
  Array selected = tc_selectedItems();
  if (selected[0] == 0) return;

  OBJ p = selected[0];

  //if (! tc_isA(p,"Regulator")) return;

  OBJ* C = tc_getConnections(p);
  int i, j, k, N = 0;

  //count the number of repressors/activators
  for (i=0; C[i]!=0; ++i)
  {
     if (tc_isA(C[i],"Binding"))
     {
        ++N;
     }        
  }
  
  
  //double * kon = malloc(N*sizeof(double));
  //double * koff = malloc(N*sizeof(double));
  OBJ * js = malloc((N+1)*sizeof(OBJ));
  js[N] = 0;

  //get kon,koff,and trans.reg. connections
  j=0;
  for (i=0; C[i]!=0; ++i)
  {
     if (tc_isA(C[i],"Binding"))
     {
        //kon[j] = tc_getNumericalData(C[i],"kon");
        //koff[j] = tc_getNumericalData(C[i],"koff");
        js[j] = C[i];
        ++j;
     }         
  }

  //get the repressors/activators names

  OBJ * tfs = malloc((N+2)*sizeof(OBJ));
  tfs[0] = p;
  tfs[N+1] = 0; //very important - null terminated
  k = 1;
  for (i=0; C[i]!=0; ++i)
  {
     if (tc_isA(C[i],"Binding"))
     {
        OBJ* parts = tc_getConnectedNodes(C[i]);
        for (j=0; parts[j] != 0; ++j)
        {
           if (parts[j] != p)
           {
              tfs[k] = parts[j];  //save tfs
              ++k;
           }
        }
     }
  }
  char ** names = tc_getNames(tfs);  //get names of proteins
  char ** jnames = tc_getNames(js);  //get names of reactions

  //main function that generates the full stoichiometry and rates
  Matrix m = fullBindingKinetics(N,jnames,names);

  //output that matrix to screen and item
  tc_printTable(m);
  tc_setRates(js,m.colnames);
  if (m.colnames)  free(m.colnames);
  m.colnames = 0;
  tc_setStoichiometry(js,m);

  free(js);  
  free(tfs);  
  if (m.values)  free(m.values);
  if (m.rownames) free(m.rownames);
  if (m.colnames)  free(m.colnames);

  TCFreeChars(names);
  TCFreeChars(jnames);
  TCFreeArray(selected);
  return; 
}
