#define OCTINTERP_API

#include <octave/octave.h>

int exec(const char * filename)
{
	int argc = 1;
	char * argv[1];
	argv[0] = filename;
	octave_main(argc, argv, 1);
}

#endif

