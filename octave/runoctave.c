#define OCTINTERP_API

#include <octave/octave.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include "TC_api.h"

void tcexec(const char * input, const char * output)
{
	int argc = 2;
	char * argv[2];	
	/*FILE * save_stdout = stdout;
	stdout = fopen(output, "w");*/
	argv[0] = "octave";
	argv[1] = (char*)input;
	octave_main(argc, argv, 1);
	//close(stdout);
	//stdout = save_stdout;
}

