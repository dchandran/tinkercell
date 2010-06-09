#define OCTINTERP_API

#include <octave/octave.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include "TC_api.h"

extern int octave_main(int n,char * args[],int a);

void tcexec(const char * input, const char * output)
{
	int argc = 1;
	char * argv[1];	
	/*FILE * save_stdout = stdout;
	stdout = fopen(output, "w");
	argv[0] = (char*)input;*/
	octave_main(argc, argv, 1);
	//close(stdout);
	//stdout = save_stdout;
}

