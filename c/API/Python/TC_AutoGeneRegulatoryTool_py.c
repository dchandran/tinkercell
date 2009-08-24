#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_partsIn(PyObject *self, PyObject *args)
{
    int i;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
		
	PyObject *pylist, *item;
	void ** array = 0;
	if (tc_partsIn)
		array = tc_partsIn((void*)i);
	
	if (array)
	{
		int len = 0;
		
		while (array[len] != 0) ++len;
		
		pylist = PyTuple_New(len);
		
		for (i=0; i<len; i++) 
		{
			item = Py_BuildValue("i",(int)(array[i]));
			PyTuple_SetItem(pylist, i, item);
		}
		free(array);
	}
	else
	{
		pylist = PyTuple_New(0);
	}
	
	return pylist;
}

static PyObject * pytc_partsUpstream(PyObject *self, PyObject *args)
{
    int i;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
		
	PyObject *pylist, *item;
	void ** array = 0;
	if (tc_partsUpstream)
		array = tc_partsUpstream((void*)i);
	
	if (array)
	{
		int len = 0;
		
		while (array[len] != 0) ++len;
		
		pylist = PyTuple_New(len);
		
		for (i=0; i<len; i++) 
		{
			item = Py_BuildValue("i",(int)(array[i]));
			PyTuple_SetItem(pylist, i, item);
		}
		free(array);
	}
	else
	{
		pylist = PyTuple_New(0);
	}
	
	return pylist;
}

static PyObject * pytc_partsDownstream(PyObject *self, PyObject *args)
{
    int i;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
		
	PyObject *pylist, *item;
	void ** array = 0;
	if (tc_partsDownstream)
		array = tc_partsDownstream((void*)i);
	
	if (array)
	{
		int len = 0;
		
		while (array[len] != 0) ++len;
		
		pylist = PyTuple_New(len);
		
		for (i=0; i<len; i++) 
		{
			item = Py_BuildValue("i",(int)(array[i]));
			PyTuple_SetItem(pylist, i, item);
		}
		free(array);
	}
	else
	{
		pylist = PyTuple_New(0);
	}
	
	return pylist;
}

static PyObject * pytc_alignParts(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	
	if(!PyArg_ParseTuple(args, "O", &pylist) || (tc_alignParts == 0))
		return NULL;
    
	int isList = PyList_Check(pylist);
	int N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	
	void ** array = malloc( (1+N) * sizeof(void*) );
	array[N] = 0;
	
	int i;
    for(i=0; i<N; ++i ) 
    { 
		array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
    }
	
 	tc_alignParts(array);
	free(array);
	
	Py_INCREF(Py_None);
    return Py_None;
}