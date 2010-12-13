#include <stdlib.h>
#include <stdio.h>
#include "TC_api.h"

void run();
void setup();


TCAPIEXPORT void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&run, "Find loops", "loops in the Jacobian can sometimes indicate bistability or oscillations", "Network structure", "nodedges.png", "", 1, 0, 0);
}


void run()
{
	int k;
	tc_items A;
	FILE * out;
	
	A = tc_selectedItems();
	
	if (tc_getItem(A,0) == 0)
		A = tc_allItems();
    
	if (tc_getItem(A,0) != 0)
	{
	   k = tc_writeModel( "runloops", A );
       tc_deleteItemsArray(A);
	   if (!k)
	   {
			tc_errorReport("No Model\0");
			return;
	   }
	}
	else
	{
       tc_deleteItemsArray(A);
	   tc_errorReport("No Model\0");
       return;  
	}
   
   out = fopen("runloops.c","a");

   fprintf( out , "\
#include \"TC_api.h\"\n\
#include \"cvodesim.h\"\n\
#include \"loops.c\"\n\
TCAPIEXPORT void run()\n\
{\n\
   int i,j;\n\
   TCinitialize(&TC_initial_model);\n\
   double * J = jacobian2(TCvars,TCreactions,TCstoic,&(TCpropensity),TCinit,&TC_initial_model,0,0);\n\
   LoopsInformation info = getLoops(J,TCvars);\n\
   FILE * file = fopen(\"loops.out\",\"w\");\n\
    for (i=0; i < info.numLoops; ++i)\n\
	{\n\
		if (info.loopTypes[i] > 0)\n\
			fprintf(file,\"negative loop:\\n\");\n\
		else\n\
			fprintf(file,\"positive loop:\\n\");\n\
		for (j=0; j < info.loopLengths[i]; ++j)\n\
		{\n\
			fprintf(file,\"%%s    \",TCvarnames[ info.nodes[i][j] ]);\n\
			if (info.loopTypes[i] > 0) \n\
				tc_highlight( tc_find(TCvarnames[ info.nodes[i][j] ]), \"#FF0000\" );\n\
			else\n\
				tc_highlight( tc_find(TCvarnames[ info.nodes[i][j] ]), \"#00FF00\" );\n\
		}\n\
		fprintf(file,\"\\n\");\n\
	}\n\
	fclose(file);\n\
	tc_printFile(\"loops.out\");\n\
   freeLoopsInfo(info);\n\
   free(J);\n\
   return;\n\
}");


   fclose(out);
   tc_compileBuildLoad("runloops.c -lode\0","run\0","Find loops\0");
   return;
}

