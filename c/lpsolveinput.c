#include "TC_api.h"

int run(Matrix input)
{
   int i,j,k;
   OBJ o;
   
   /***get selected items, if any****/
   Array selected = tc_selectedItems();
  
   k = 0;
   for (i=0; selected && selected[i]!=0; ++i)
   {
      if (tc_isA(selected[i],"connection"))
	  {
	     k = 1;
		 break;
 	  }
   }
  
   if (!k)
   {
      tc_print("Note: you may select a reaction to optimize that flux");
   }
  
   /***get the stoichiometry matrix**/
   Array all = tc_allItems();
   Matrix N = tc_getStoichiometry(all);
  
   if (N.rows < 1 || N.cols < 1) 
   {
      tc_errorReport("stoichiometry matrix is empty");
      return 0;
   }
   
   /***create the input matrix**/
   for (k=0; selected[k]!=0; ++k) {}
   
   Matrix M;
   M.cols = N.cols;
   M.colnames = N.colnames;
   M.rows = k;
   
   char * rownames[] = {"objective",0};
   M.rownames = rownames;
   M.values = malloc(M.rows * M.cols * sizeof(double));
   
   /**initialize the input matrix using selected reactions***/
   for (i=0; i < M.cols; ++i)
   {
     o = tc_find(N.colnames[i]);  //find the item with the column name
	 for (j=0; selected[j]!=0; ++j)
	 {
	    if (o && selected[j] == o)    //if this column is selected by user
		   valueAt(M,0,i) = 1.0;       //set it as an objective
		else
		   valueAt(M,0,i) = 0.0;
	 }
   }
   
   /***finished***/
   tc_createInputWindow(M,"dlls/lpsolve","run","Additional Constraints");
   
   
   free(M.values);
   TCFreeArray(all);
   TCFreeMatrix(N);
   
   return 1; 
}
