#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_insert(PyObject *self, PyObject *args)
{
	char * s;
	char * f = "Species\0";
	if(!PyArg_ParseTuple(args, "s|s", &s, &f) || (tc_insert == 0))
        return NULL;
	
	void * o = tc_insert(s,f);
	 
	return Py_BuildValue("i",(int)o);
}
