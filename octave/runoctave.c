#ifndef TC_OCTAVE_EMBED_C
#define OCTINTERP_API

#include <stdlib.h>
#include <stdio.h>
#include <octave/octave.h>
#include "TC_api.h"

TCAPIEXPORT void exec(const char * input, const char * output)
{
	int argc = 1;
	char * argv[1];	
	FILE * save_stdout = stdout;
	stdout = fopen(output, "w");
	argv[0] = (char*)filename;
	octave_main(argc, argv, 1);
	close(stdout);
	stdout = save_stdout;
}

#endif

