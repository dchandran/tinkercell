#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_writeModel(PyObject *self, PyObject *args)
{
	char * s;
	void ** array = 0;
	PyObject * pylist = 0;
	int isList, N=0, i=0;

	if(!PyArg_ParseTuple(args, "s|O", &s, &pylist))
        return NULL;

	if (pylist)
	{
		if (PyList_Check(pylist) || PyTuple_Check(pylist))
		{
			isList = PyList_Check(pylist);
			N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
		}
	
		if (N > 0)
		{
			array = malloc( (1+N) * sizeof(void*) );
			array[N] = 0;
	
			for(i=0; i<N; ++i ) 
			{ 
				array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			} 
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

