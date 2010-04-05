#include <EXTERN.h>
#include <perl.h>
#include "TC_api.h"

static PerlInterpreter *my_perl;

TCAPIEXPORT void initialize()
{
    my_perl = perl_alloc();
    perl_construct(my_perl);
}

TCAPIEXPORT int exec(const char * code, const char * file)
{
	char * codes[] = {code};
	
    my_perl = perl_alloc();
    perl_construct(my_perl);
    perl_parse(my_perl, NULL, 1, codes, (char **)NULL);
    perl_run(my_perl);    
    tc_printFile(file);
}

TCAPIEXPORT void finalize()
{
    perl_destruct(my_perl);
    perl_free(my_perl);
}

