#include <ruby.h>
#include "TC_structs.h"

BEGIN_C_DECLS

VALUE rb_protect_wrap(VALUE arg) 
{
    return 0;
}

TCAPIEXPORT void initialize()
{
	int error;
	ruby_init();
	rb_protect(&rb_protect_wrap, 0, &error);
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

