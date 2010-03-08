#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_partsIn(PyObject *self, PyObject *args)
{
    int i,len;
	PyObject *pylist, *item;
	void ** array = 0;

	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

	array = tc_partsIn((void*)i);

	if (array)
	{
		len = 0;

		while (array[len] != 0) ++len;

		pylist = PyTuple_New(len);

		for (i=0; i<len; i++)
		{
			item = Py_BuildValue("i",(size_t)(array[i]));
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
	PyObject *pylist, *item;
	void ** array = 0;
	int len;

	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

	array = tc_partsUpstream((void*)i);

	if (array)
	{
		len = 0;

		while (array[len] != 0) ++len;

		pylist = PyTuple_New(len);

		for (i=0; i<len; i++)
		{
			item = Py_BuildValue("i",(size_t)(array[i]));
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
    int i,len;
	PyObject *pylist, *item;
	void ** array = 0;

	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;


	array = tc_partsDownstream((void*)i);

	if (array)
	{
		len = 0;

		while (array[len] != 0) ++len;

		pylist = PyTuple_New(len);

		for (i=0; i<len; i++)
		{
			item = Py_BuildValue("i",(size_t)(array[i]));
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
