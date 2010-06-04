#ifndef TC_OCTAVE_EMBED_C
#define OCTINTERP_API

#include <octave/octave.h>

int exec(const char * filename)
{
	int argc = 1;
	char * argv[1];
	argv[0] = (char*)filename;
	octave_main(argc, argv, 1);
}

#endif

