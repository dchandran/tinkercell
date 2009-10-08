#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_insertConnection(PyObject *self, PyObject *args)
{
	PyObject * partsFrom, * partsTo;
	char * s;
	char * f = "Connection\0";
	int isList1, n1=0, isList2, n2=0, i;
	void * o = 0, ** array1, **array2;

	if(!PyArg_ParseTuple(args, "OOs|s", &partsFrom, &partsTo, &s, &f) ||
		(tc_insertConnection == 0))
		return NULL;

	if (PyList_Check(partsFrom) || PyTuple_Check(partsFrom))
	{
		isList1 = PyList_Check(partsFrom);
		n1 = isList1 ? PyList_Size(partsFrom) : PyTuple_Size (partsFrom);
	}

	if (PyList_Check(partsTo) || PyTuple_Check(partsTo))
	{
		isList2 = PyList_Check(partsTo);
		n2 = isList2 ? PyList_Size(partsTo) : PyTuple_Size (partsTo);
	}

	o = 0;

	if (n1 > 0 && n2 > 0)
	{

		array1 = malloc( (1+n1) * sizeof(void*) );
		array1[n1] = 0;

		array2 = malloc( (1+n2) * sizeof(void*) );
		array2[n2] = 0;

		for(i=0; i<n1; ++i) 
		{ 
			array1[i] = isList1 ? (void*)((int)PyInt_AsLong( PyList_GetItem( partsFrom, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( partsFrom, i ) ));
		}
		for(i=0; i<n2; ++i) 
		{ 
			array2[i] = isList2 ? (void*)((int)PyInt_AsLong( PyList_GetItem( partsTo, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( partsTo, i ) ));
		}

		o = tc_insertConnection(array1,array2,s,f);
	}

	return Py_BuildValue("i",(int)o);
}

static PyObject * pytc_getConnectedNodes(PyObject *self, PyObject *args)
{
	int i,len;
	void** array;
	PyObject *pylist, *item;

	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedNodes == 0))
		return NULL;

	array = tc_getConnectedNodes((void*)i);


	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnectedNodesIn(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;
	
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedNodesIn == 0))
		return NULL;

	array = tc_getConnectedNodesIn((void*)i);

	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnectedNodesOut(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;
	
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedNodesOut == 0))
		return NULL;

	array = tc_getConnectedNodesOut((void*)i);
	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnectedNodesOther(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;

	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedNodesOther == 0))
		return NULL;

	array = tc_getConnectedNodesOther((void*)i);

	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnections(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;
	
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnections == 0))
		return NULL;

	array = tc_getConnections((void*)i);

	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnectionsIn(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;


	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectionsIn == 0))
		return NULL;

	array = tc_getConnectionsIn((void*)i);
	
	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnectionsOut(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;

	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectionsOut == 0))
		return NULL;

	array = tc_getConnectionsOut((void*)i);

	if (array)
	{
		len = 0;

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

static PyObject * pytc_getConnectionsOther(PyObject *self, PyObject *args)
{
	int i,len;
	void ** array;
	PyObject *pylist, *item;

	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectionsOther == 0))
		return NULL;

	array = tc_getConnectionsOther((void*)i);

	if (array)
	{
		len = 0;

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
