#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_writeModel(PyObject *self, PyObject *args)
{
	char * s;

	PyObject * pylist = 0;
	if(!PyArg_ParseTuple(args, "s|O", &s, &pylist) || (tc_allItems == 0) || (tc_writeModel == 0))
        return NULL;

	void ** array = 0;
	if (pylist)
	{
		int isList = PyList_Check(pylist);
		int N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	
		void ** array = malloc( (1+N) * sizeof(void*) );
		array[N] = 0;
	
		int i;
		for(i=0; i<N; ++i ) 
		{ 
			array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
		} 
	}
	else
	{
		array = tc_allItems();
	}
	
	tc_writeModel(s,array);
	
	free(array);
	Py_INCREF(Py_None);
    return Py_None;	
}

