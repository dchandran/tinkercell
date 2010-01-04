#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_showProgress(PyObject *self, PyObject *args)
{
	int i = 0;

	if (!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	tc_showProgress("python thread",i);

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

