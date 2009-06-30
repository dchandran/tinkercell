/****************************************************************************
 
 This file goes hand-in-hand with the code in findBistability.c
 This file generates the ode file. The other file assumes the existence of the ode file
 and performs the bistabilty analysis. Then this file resumes and sends the output to TinkerCell.
 
 ****************************************************************************/
 
 #include "TC_api.h"

int run(Matrix input) 
{

   Array A = tc_allItems();
   tc_writeModel("ode.c",A); //generate ode model
   TCFreeArray(A);
   
   char* appDir = tc_appDir();

   int sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*8 + 200) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"gcc -o a.exe \"%s/c/\"mtrand.c \"%s/c/\"ga.c \"%s/c/\"ga_bistable.c \"%s/c/\"mat.c \"%s/c/\"neldermead.c \"%s/c/\"findBistability.c -I. -I\"%s\"/c -L\"%s\"/lib -lm -lodesim\0",appDir,appDir,appDir,appDir,appDir,appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"gcc -o a.out %s/c/mtrand.c %s/c/ga.c %s/c/ga_bistable.c %s/c/mat.c %s/c/neldermead.c %s/c/findBistability.c -I. -I%s/c -L%s/lib -lm -lodesim\0",appDir,appDir,appDir,appDir,appDir,appDir,appDir,appDir);
   }
   
   if (tc_isWindows())
   {  
       system(cmd);
	   system("a.exe");
   }
   else
   {
       system(cmd);
	   system("./a.out");
   }
   
   tc_printFile("temp.out");
   
   return 1;
 
}
 
 
   
   
