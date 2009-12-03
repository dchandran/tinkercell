#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_loadSBMLString(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_loadSBMLString(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_loadAntimonyString(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_loadAntimonyString(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_loadSBMLFile(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_loadSBMLFile(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_loadAntimonyFile(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_loadAntimonyFile(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_getSBMLString(PyObject *self, PyObject *args)
{
	int i;
	const char * s;
	PyObject * pylist = 0;
	char ** rates = 0;
	int isList, N=0;
	void ** array=0;

	if(!PyArg_ParseTuple(args, "|O", &pylist))
	{
		return NULL;
	}
	
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
	
	if (array == 0)
	{
		array = tc_allItems();
	}
	
	s = tc_getSBMLString(array);
	free(array);
	
	return Py_BuildValue("s",s);
}

static PyObject * pytc_getAntimonyString(PyObject *self, PyObject *args)
{
	int i;
	const char * s;
	PyObject * pylist = 0;
	char ** rates = 0;
	int isList, N=0;
	void ** array=0;

	if(!PyArg_ParseTuple(args, "|O", &pylist))
	{
		return NULL;
	}
	
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
	
	if (array == 0)
	{
		array = tc_allItems();
	}
	
	s = tc_getAntimonyString(array);
	free(array);
	
	return Py_BuildValue("s",s);
}

static PyObject * pytc_writeSBMLFile(PyObject *self, PyObject *args)
{
	char * s;
	Array array = tc_allItems();


	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_writeSBMLFile(array,s);
	
	free(array);
	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_writeAntimonyFile(PyObject *self, PyObject *args)
{
	char * s;
	Array array = tc_allItems();

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_writeAntimonyFile(array,s);

	free(array);
	Py_INCREF(Py_None);
	return Py_None;	
}

