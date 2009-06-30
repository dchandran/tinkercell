#include "TC_api.h"

int run(Matrix M)
{
	Array A;
	A = tc_selectedItems();
	if (A[0] == 0)
		A = tc_allItems();
    
	if (A[0] != 0)
	{
	   int k = tc_writeModel( "runloops", A );
       TCFreeArray(A);
	   if (!k)
	   {
			tc_errorReport("No Model\0");
			return 0;
	   }
	}
	else
	{
       TCFreeArray(A);
	   tc_errorReport("No Model\0");
       return 0;  
	}
   
   FILE * out = fopen("runloops.c","a");

   fprintf( out , "\
#include \"TC_api.h\"\n\
#include \"cvodesim.h\"\n\
#include \"loops.h\"\n\
int run(Matrix M)\n\
{\n\
   int i,j;\n\
   TCinitialize();\n\
   double * J = jacobian2(TCvars,TCreactions,TCstoic,&(TCpropensity),TCinit,0);\n\
   LoopsInformation info = getLoops(J,TCvars);\n\
    for (i=0; i < info.numLoops; ++i)\n\
	{\n\
		if (info.loopTypes[i] > 0)\n\
			printf(\"negative loop:\");\n\
		else\n\
			printf(\"positive loop:\");\n\
		for (j=0; j < info.loopLengths[i]; ++j)\n\
		{\n\
			printf(\"\\t%s\",TCvarnames[ info.nodes[i][j] ]);\n\
			if (info.loopTypes[i] > 0) \n\
				tc_highlight( tc_find(TCvarnames[ info.nodes[i][j] ]), 255, 0, 0 );\n\
			else\n\
				tc_highlight( tc_find(TCvarnames[ info.nodes[i][j] ]), 0, 0, 255 );\n\
		}\n\
		printf(\"\\n\");\n\
	}\n\
   freeLoopsInfo(info);\n\
   free(J);\n\
   return 1;\n\
}");


/*for (i=0; i < info.numLoops; ++i)
      for (j=0; j < info.loopLengths[i]; ++j)
	    tc_print("%i\t",info.nodes[i][j]);

   tc_print("type=%i\tsame=%i\n",info.loopTypes[i],info.loopHomogeneous[i]);
   */
   fclose(out);
/*
   char* appDir = tc_appDir();

   int sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*4 + 50) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"\"%s\"/c/odesim.o \"%s\"/c/loops.c runloops.c -I\"%s\"/include -I\"%s\"/c\0",appDir,appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"%s/c/loops.c runloops.c -I%s/c -L%s/lib -lodesim\0",appDir,appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0");
  */ 
   return 0;
}