#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

#define _CALLBACK_STR_LEN 100

static char _CALLBACK_STRING[_CALLBACK_STR_LEN];
int _CALLBACK_ADDED = 0;
static void PYTHON_CALLBACK()
{
	tc_runPythonCode((char*)_CALLBACK_STRING);
}

static char _CALLBACK_STRING_ARGS[_CALLBACK_STR_LEN];
static void PYTHON_CALLBACK_ARGS(Matrix m)
{
	int i,j;
	char * str = (char*)malloc((_CALLBACK_STR_LEN + (m.rows * 20)) * sizeof(char));
	
	for (i=0; _CALLBACK_STRING_ARGS[i]; ++i)
		str[i] = _CALLBACK_STRING_ARGS[i];
	
	sprintf(str,"(%s",str);
	
	for (j=0; j < m.rows; ++j)
		sprintf(str,"%s%0.4lf",str,m.values[j]);
	
	sprintf(str,"%s)\0",str);
	
	tc_runPythonCode(str);
	
	free(str);
}

static PyObject * pytc_setCallback(PyObject *self, PyObject *args)
{
	char * s = 0;
	int i;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	for (i=0; s && s[i]; ++i)
		_CALLBACK_STRING[i] = s[i];
	
	if (i >= _CALLBACK_STR_LEN)
	{
		tc_errorReport("callback function name is too long");
		i = 0;
		s = 0;
	}
	
	for (; i < _CALLBACK_STR_LEN; ++i)
		_CALLBACK_STRING[i] = 0;

	if (s && !_CALLBACK_ADDED)
	{
		tc_callback(&(PYTHON_CALLBACK));
		_CALLBACK_ADDED = 1;
	}

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_setSliderCallback(PyObject *self, PyObject *args)
{
	PyObject * names = 0 , * minv = 0, *maxv = 0;
	char * s = 0;
	int i,j;
	int isList1,n1=0,isList2,n2=0,n3=0, isList3;
	char ** rownames;
	double * nums;
	Matrix M;
	
	if (!PyArg_ParseTuple(args, "OOOs", &names, &minv, &maxv, &s))
		return NULL;
	
	if (PyList_Check(names) || PyTuple_Check(names))
	{
		isList1 = PyList_Check(names);
		n1 = isList1 ? PyList_Size(names) : PyTuple_Size (names);
	}
	
	if (PyList_Check(minv) || PyTuple_Check(minv))
	{
		isList2 = PyList_Check(minv);
		n2 = isList2 ? PyList_Size(minv) : PyTuple_Size (minv);
	}
	
	if (PyList_Check(maxv) || PyTuple_Check(maxv))
	{
		isList3 = PyList_Check(maxv);
		n3 = isList3 ? PyList_Size(maxv) : PyTuple_Size (maxv);
	}
	
	if (n1 > 0 && n1 == n2 && n1 == n3)
	{
		rownames = malloc( (1+n1) * sizeof(char*) );
		nums = malloc( 2 * n1 * sizeof(double) );
		rownames[n1] = 0;
	
		for(i=0; i<n1; ++i) 
		{ 
			rownames[i] = isList1 ? PyString_AsString( PyList_GetItem( names, i ) ) : PyString_AsString( PyTuple_GetItem( names, i ) );
			nums[ i*2 + 0 ] = isList2 ? PyFloat_AsDouble( PyList_GetItem( minv, i ) ) : PyFloat_AsDouble( PyTuple_GetItem( minv, i ) );
			nums[ i*2 + 1 ] = isList3 ? PyFloat_AsDouble( PyList_GetItem( maxv, i ) ) : PyFloat_AsDouble( PyTuple_GetItem( maxv, i ) );
		}
		
		M.cols = 2;
		M.rows = n1;		
		M.colnames = 0;
		M.rownames = rownames;
		M.values = nums;
		
		for (i=0; s && s[i]; ++i)
			_CALLBACK_STRING_ARGS[i] = s[i];
	
		if (i >= _CALLBACK_STR_LEN)
		{
			tc_errorReport("callback function name is too long");
			i = 0;
			s = 0;
		}
		
		for (; i < _CALLBACK_STR_LEN; ++i)
			_CALLBACK_STRING_ARGS[i] = 0;

		if (names && minv && maxv && s)
			tc_createSliders(M,&(PYTHON_CALLBACK_ARGS));
	
		free(nums);
	}

	Py_INCREF(Py_None);
	return Py_None;	
}


static PyObject * pytc_showProgress(PyObject *self, PyObject *args)
{
	int i = 0;

	if (!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	tc_showProgress(i);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_compileAndRun(PyObject *self, PyObject *args)
{
	char * a, *b;
	int i;

	if(!PyArg_ParseTuple(args, "ss", &a, &b))
		return NULL;

	i = tc_compileAndRun(a,b);

	return Py_BuildValue("i",i);
}

static PyObject * pytc_compileBuildLoad(PyObject *self, PyObject *args)
{
	char * a, *b, *c;
	int i;
	if(!PyArg_ParseTuple(args, "sss", &a, &b, &c))
		return NULL;

	i = tc_compileBuildLoad(a,b,c);

	return Py_BuildValue("i",i);
}

static PyObject * pytc_callFunction(PyObject *self, PyObject *args)
{
	char * a;
	if(!PyArg_ParseTuple(args, "s", &a))
		return NULL;

	tc_callFunction(a);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_loadLibrary(PyObject *self, PyObject *args)
{
	char * a;
	if(!PyArg_ParseTuple(args, "s", &a))
		return NULL;

	tc_loadLibrary(a);

	Py_INCREF(Py_None);
	return Py_None;	
}
static PyObject * pytc_addPythonPlugin(PyObject *self, PyObject *args)
{
	char * a, *b, *c, *d, *e;

	if(!PyArg_ParseTuple(args, "sssss", &a, &b, &c, &d, &e))
		return NULL;

	tc_addPythonPlugin(a,b,c,d,e);
	
	Py_INCREF(Py_None);
	return Py_None;
}

