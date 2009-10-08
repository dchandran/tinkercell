#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_showProgress(PyObject *self, PyObject *args)
{
	int i = 0;

	if (!PyArg_ParseTuple(args, "i", &i) || (tc_showProgress == 0))
		return NULL;

	tc_showProgress("python thread",i);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_compileAndRun(PyObject *self, PyObject *args)
{
	char * a, *b;
	int i;

	if(!PyArg_ParseTuple(args, "ss", &a, &b) || (tc_compileAndRun == 0))
		return NULL;

	i = tc_compileAndRun(a,b);

	return Py_BuildValue("i",i);
}

static PyObject * pytc_compileBuildLoad(PyObject *self, PyObject *args)
{
	char * a, *b, *c;
	int i;
	if(!PyArg_ParseTuple(args, "sss", &a, &b, &c) || (tc_compileBuildLoad == 0))
		return NULL;

	i = tc_compileBuildLoad(a,b,c);

	return Py_BuildValue("i",i);
}

static PyObject * pytc_callFunction(PyObject *self, PyObject *args)
{
	char * a;
	if(!PyArg_ParseTuple(args, "s", &a) || (tc_callFunction == 0))
		return NULL;

	tc_callFunction(a);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_loadLibrary(PyObject *self, PyObject *args)
{
	char * a;
	if(!PyArg_ParseTuple(args, "s", &a) || (tc_loadLibrary == 0))
		return NULL;

	tc_loadLibrary(a);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_getString(PyObject *self, PyObject *args)
{
	char * a, * s;
	if(!PyArg_ParseTuple(args, "s", &a) || (tc_getString == 0))
		return NULL;

	s = tc_getString(a);

	return Py_BuildValue("s",s);
}

static PyObject * pytc_getFilename(PyObject *self, PyObject *args)
{
	char * s;
	if (tc_getFilename == 0) return NULL;	

	s = tc_getFilename();

	return Py_BuildValue("s",s);
}

static PyObject * pytc_getFromList(PyObject *self, PyObject *args)
{
	char * s;
	char * s0 = "";
	PyObject * pylist;
	int isList, n=0 , j = -1, i = 0;
	char ** array;

	int k = 1;

	if(!PyArg_ParseTuple(args, "sO|si", &s, &pylist,&s0,&k) || (tc_getFromList == 0))
		return NULL;

	if (PyList_Check(pylist) || PyTuple_Check(pylist))
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	j = -1;
	i = 0;

	if (n)
	{
		array = malloc( (1+n) * sizeof(char*) );
		array[n] = 0;

		for(i=0; i<n; ++i) 
		{ 
			array[i] = isList ? PyString_AsString( PyList_GetItem( pylist, i ) ) : PyString_AsString( PyTuple_GetItem( pylist, i ) );
		}

		j = tc_getFromList(s,array,s0, k);
		TCFreeChars(array);
	}

	return Py_BuildValue("i",j);
}

static PyObject * pytc_getNumber(PyObject *self, PyObject *args)
{
	char * a;
	double d;
	if(!PyArg_ParseTuple(args, "s", &a) || (tc_getNumber == 0))
		return NULL;

	d = tc_getNumber(a);

	return Py_BuildValue("d",d);
}

static PyObject * pytc_getNumbers(PyObject *self, PyObject *args)
{
	PyObject *pylist, *item,* pylist2;
	double * values;
	int isList, n=0, i;
	char ** array;

	if(!PyArg_ParseTuple(args, "O", &pylist) || (tc_getNumbers == 0))
		return NULL;

	if (PyList_Check(pylist) || PyTuple_Check(pylist))
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	if (n > 0)
	{
		array = malloc( (1+n) * sizeof(char*) );
		array[n] = 0;

		for(i=0; i<n; ++i) 
		{ 
			array[i] = isList ? PyString_AsString( PyList_GetItem( pylist, i ) ) : PyString_AsString( PyTuple_GetItem( pylist, i ) );
		}

		values = malloc(n*sizeof(double));
		tc_getNumbers(array,values);
		TCFreeChars(array);

		pylist2 = PyTuple_New(n);

		for (i=0; i<n; i++) 
		{
			item = Py_BuildValue("d",(values[i]));
			PyTuple_SetItem(pylist2, i, item);
		}
		free(values);

		return pylist2;

	}
	else
	{
		pylist2 = PyTuple_New(0);
	}

	return pylist2;
}
