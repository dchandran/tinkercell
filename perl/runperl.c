#include <EXTERN.h>               /* from the Perl distribution     */
#include <perl.h>                 /* from the Perl distribution     */

static PerlInterpreter *my_perl;  /***    The Perl interpreter    ***/

int main(int argc, char **argv, char **env)
{
    my_perl = perl_alloc();
    perl_construct(my_perl);
    perl_parse(my_perl, NULL, argc, argv, (char **)NULL);
    perl_run(my_perl);
    perl_destruct(my_perl);
    perl_free(my_perl);
}

PyObject* main_dict;
PyObject* dlfl_dict;
PyObject * s;
PyObject *evalVal;
PyObject *errobj;
PyObject *errdata;
PyObject *errtraceback;

FILE * pmout;

void initialize()
{
    my_perl = perl_alloc();
    perl_construct(my_perl);
    perl_parse(my_perl, NULL, argc, argv, (char **)NULL);
    perl_run(my_perl);
    perl_destruct(my_perl);
    perl_free(my_perl);
	
	pmout = fopen("pm.out","w+");
}

void exec(const char * code)
{
	char *retString, 
	 	   *errString;
	PyObject * s,
			 *evalVal,
			 *errobj, 
			 *errdata, 
			 *errtraceback;
			 
	//PyRun_SimpleString( code );
	evalVal = PyRun_String( code, Py_file_input, main_dict, dlfl_dict );
	//evalVal = PyDict_GetItemString( main_dict, "_" );
	
	PyErr_Fetch (&errobj, &errdata, &errtraceback);
	
	/*if (evalVal != NULL)
	{
		s = PyObject_Str(evalVal); 
		retString = PyString_AsString(s);
		tc_print(retString);
		Py_DECREF(s); 
		Py_DECREF(evalVal);
	}*/

	tc_printFile(pmout);
	
	if (errdata != NULL) 
	{ 
		s = PyObject_Str(errdata); 
		errString = PyString_AsString(s);
		tc_errorReport(errString);
		Py_DECREF(s); 
	}
	
	Py_XDECREF(errobj); 
	Py_XDECREF(errdata); 
	Py_XDECREF(errtraceback); 
	Py_XDECREF(evalVal);
}

void finalize()
{
	Py_XDECREF(errobj); 
	Py_XDECREF(errdata); 
	Py_XDECREF(errtraceback); 
	
    Py_Finalize();
	
	fclose(pmout);
}
