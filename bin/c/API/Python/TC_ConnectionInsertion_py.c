#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_insertConnection(PyObject *self, PyObject *args)
{
	PyObject * partsFrom, * partsTo;
	char * s;
	char * f = "Connection\0";
	if(!PyArg_ParseTuple(args, "OOs|s", &partsFrom, &partsTo, &s, &f) ||
		(tc_insertConnection == 0))
        return NULL;
	
	int isList1 = PyList_Check(partsFrom);
	int n1 = isList1 ? PyList_Size(partsFrom) : PyTuple_Size (partsFrom);
	
	int isList2 = PyList_Check(partsTo);
	int n2 = isList2 ? PyList_Size(partsTo) : PyTuple_Size (partsTo);
	
	void * o = 0;
	
	if (n1 > 0 && n2 > 0)
	{
	
		void ** array1 = malloc( (1+n1) * sizeof(void*) );
		array1[n1] = 0;
		
		void ** array2 = malloc( (1+n2) * sizeof(void*) );
		array2[n2] = 0;
	
		int i;
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

static PyObject * pytc_getConnectedParts(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedParts == 0))
        return NULL;
		
	void** array = tc_getConnectedParts((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnectedPartsIn(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedPartsIn == 0))
        return NULL;
		
	void** array = tc_getConnectedPartsIn((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnectedPartsOut(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedPartsOut == 0))
        return NULL;
		
	void** array = tc_getConnectedPartsOut((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnectedPartsOther(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectedPartsOther == 0))
        return NULL;
		
	void** array = tc_getConnectedPartsOther((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnections(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnections == 0))
        return NULL;
		
	void** array = tc_getConnections((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnectionsIn(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectionsIn == 0))
        return NULL;
		
	void** array = tc_getConnectionsIn((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnectionsOut(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectionsOut == 0))
        return NULL;
		
	void** array = tc_getConnectionsOut((void*)i);
	
	PyObject *pylist, *item;
	
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

static PyObject * pytc_getConnectionsOther(PyObject *self, PyObject *args)
{
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getConnectionsOther == 0))
        return NULL;
		
	void** array = tc_getConnectionsOther((void*)i);
	
	PyObject *pylist, *item;
	
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
