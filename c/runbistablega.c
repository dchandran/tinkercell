/****************************************************************************

This file goes hand-in-hand with the code in findBistability.c
This file generates the ode file. The other file assumes the existence of the ode file
and performs the bistabilty analysis. Then this file resumes and sends the output to TinkerCell.

****************************************************************************/

#include "TC_api.h"

void run();

int functionMissing()
{
	if (!tc_isWindows || 
		!tc_allItems ||
		!tc_addFunction ||
		!tc_printFile ||
		!tc_writeModel)

		return 1;


	return 0;
}

void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu?  
	tc_addFunction(&run, "Force Bistability", "uses genetic algorithms to find parameters to make system bistable", "Parameter scan", "Plugins/c/default.png", "", 1, 0, 0);
}

void run()
{
	char* appDir;
	int sz = 0;
	char* cmd;
	Array A = tc_allItems();
	tc_writeModel("ode.c",A); //generate ode model

	appDir = tc_appDir();
	TCFreeArray(A);   


	while (appDir[sz] != 0) ++sz;

	cmd = malloc((sz*8 + 200) * sizeof(char));


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

}




