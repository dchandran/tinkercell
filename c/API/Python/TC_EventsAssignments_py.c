#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_getEventTriggers(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int isList, N=0, len, i;
	void ** array=0;
	PyObject *strlist;
	PyObject * item;
	char ** names=0;
	
	if(!PyArg_ParseTuple(args, "O", &pylist))
	{
		return NULL;
	}
	
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
		
		names = tc_getEventTriggers(array);
		free(array);
	}
	
	if (names)
	{
		len = 0;
		
		while (names[len] != 0) ++len;
		
		strlist = PyTuple_New(len);
		
		for (i=0; i<len && names[i]!=0; i++)
		{
			item = Py_BuildValue("s",names[i]);
			PyTuple_SetItem(strlist, i, item);
		}
		TCFreeChars(names);
	}
	else
	{
		strlist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",strlist);
}

static PyObject * pytc_getEventResponses(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int isList, N=0, i, len;
	void ** array=0;
	PyObject *strlist;
	PyObject * item;
	char ** names=0;
	
	if(!PyArg_ParseTuple(args, "O", &pylist))
	{
		return NULL;
	}
	
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
		
		names = tc_getEventResponses(array);
		free(array);
	}
	if (names)
	{
		len = 0;
		
		while (names[len] != 0) ++len;
		
		strlist = PyTuple_New(len);
		
		for (i=0; i<len && names[i]!=0; i++)
		{
			item = Py_BuildValue("s",names[i]);
			PyTuple_SetItem(strlist, i, item);
		}
		TCFreeChars(names);
	}
	else
	{
		strlist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",strlist);
}

static PyObject * pytc_addEvent(PyObject *self, PyObject *args)
{
	int item;
	char * s1, *s2;
	if (!PyArg_ParseTuple(args, "iss", &item,&s1, &s2))
        return NULL;
	
	tc_addEvent((void*)item,s1,s2);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_getForcingFunctionNames(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int isList, N=0, i, len;
	void ** array=0;
	char ** names;
	PyObject *strlist;
	PyObject * item=0;
	
	if(!PyArg_ParseTuple(args, "O", &pylist))
	{
		return NULL;
	}
	
	if (PyList_Check(pylist) || PyTuple_Check(pylist))
	{
		isList = PyList_Check(pylist);
		N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	
	if (N>0)
	{
		array = malloc( (1+N) * sizeof(void*) );
		array[N] = 0;
		
		for(i=0; i<N; ++i ) 
		{ 
			array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
		} 
		
		names = tc_getForcingFunctionNames(array);
		free(array);
	}
	if (names)
	{
		len = 0;
		
		while (names[len] != 0) ++len;
		
		strlist = PyTuple_New(len);
		
		for (i=0; i<len && names[i]!=0; i++)
		{
			item = Py_BuildValue("s",names[i]);
			PyTuple_SetItem(strlist, i, item);
		}
		TCFreeChars(names);
	}
	else
	{
		strlist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",strlist);
}

static PyObject * pytc_getForcingFunctionAssignments(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int isList, N=0, i, len;
	void ** array=0;
	char ** names=0;
	PyObject *strlist;
	PyObject * item;
	
	if(!PyArg_ParseTuple(args, "O", &pylist))
	{
		return NULL;
	}
	
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
		
		names = tc_getForcingFunctionAssignments(array);
		free(array);
	}
	
	if (names)
	{
		len = 0;
		
		while (names[len] != 0) ++len;
		
		strlist = PyTuple_New(len);
		
		for (i=0; i<len && names[i]!=0; i++)
		{
			item = Py_BuildValue("s",names[i]);
			PyTuple_SetItem(strlist, i, item);
		}
		TCFreeChars(names);
	}
	else
	{
		strlist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",strlist);
}

static PyObject * pytc_addForcingFunction(PyObject *self, PyObject *args)
{
	int item;
	char * s1, *s2;
	if(!PyArg_ParseTuple(args, "iss", &item,&s1, &s2))
        return NULL;
	
	tc_addForcingFunction((void*)item,s1,s2);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_displayText(PyObject *self, PyObject *args)
{
	int item;
	char * s;
	if(!PyArg_ParseTuple(args, "is", &item,&s))
        return NULL;
	
	tc_displayText((void*)item,s);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_displayNumber(PyObject *self, PyObject *args)
{
	int item;
	double d;
	if(!PyArg_ParseTuple(args, "id", &item,&d))
        return NULL;
	
	tc_displayNumber((void*)item,d);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_highlight(PyObject *self, PyObject *args)
{
	int item,r=0,g=0,b=0;
	if(!PyArg_ParseTuple(args, "i|iii", &item,&r,&g,&b))
        return NULL;
	
	tc_highlight((void*)item,r,g,b);
	
	Py_INCREF(Py_None);
    return Py_None;
}
