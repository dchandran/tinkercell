#include <ruby.h>
#include "TC_structs.h"

BEGIN_C_DECLS

TCAPIEXPORT void initialize()
{
	ruby_init();
}

TCAPIEXPORT void finalize()
{
	ruby_finalize();
}

TCAPIEXPORT void exec(const char* s)
{
	rb_eval_string(s);
}

END_C_DECLS

