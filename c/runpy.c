#import <Python.h>
#include "API/Python/TC_py.h"
//NOTE: (arnaudgelas)
//on Mac the following line must be uncommented
//#include "dlfcn.h"

PyObject* main_dict;
PyObject* dlfl_dict;
PyObject *errobj, *errdata, *errtraceback;

void initialize()
{
//  NOTE: (arnaudgelas)
//  On Mac the 2 following lines must be uncommented
//  Since the location/extension of this library is platform and machine
//  dependent why not using CMake with something like @{PYTHON_LIBRARY}@
//  or something like that???
//	dlopen("/usr/lib/libpython2.5.dylib", RTLD_LAZY | RTLD_GLOBAL);	
//	Py_SetProgramName("Tinkercell");
	Py_Initialize();
	
	Py_InitModule("pytc", pytcMethods);
	PyObject *mainmod = PyImport_AddModule("__main__");
	PyObject *dlfl = PyImport_AddModule("pytc");
	main_dict = PyModule_GetDict( mainmod );
	dlfl_dict = PyModule_GetDict( dlfl );
}

void exec(const char * code,const char * outfile)
{
	PyObject * rstring = PyRun_String( code, Py_file_input, main_dict, dlfl_dict );
	
	PyErr_Fetch (&errobj, &errdata, &errtraceback);
	
	tc_printFile(outfile);
	
	if (errdata != NULL) 
	{ 
		PyObject *s = PyObject_Str(errdata); 
		char * c = PyString_AS_STRING(s);
		tc_errorReport(c);
		Py_DECREF(s); 
	}
}

void finalize()
{
	Py_XDECREF(errobj); 
	Py_XDECREF(errdata); 
	Py_XDECREF(errtraceback); 

    Py_Finalize();
}
