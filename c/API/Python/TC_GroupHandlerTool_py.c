#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_merge(PyObject *self, PyObject *args)
{
	PyObject * parts;
	if(!PyArg_ParseTuple(args, "O", &parts))
        return NULL;
	
	int isList = PyList_Check(parts);
	int n = isList ? PyList_Size(parts) : PyTuple_Size (parts);
	
	if (n)
	{
	
		void ** array = malloc( (1+n) * sizeof(void*) );
		array[n] = 0;
		
		int i;
		for(i=0; i<n; ++i) 
		{ 
			array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( parts, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( parts, i ) ));
		}
	
		tc_merge(array);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_separate(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
	
	tc_separate((void*)i);
	
	Py_INCREF(Py_None);
    return Py_None;
}
