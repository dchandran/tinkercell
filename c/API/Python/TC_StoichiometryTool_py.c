#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_getStoichiometry(PyObject *self, PyObject *args)
{
	PyObject * pylist = 0;
	int o,i,j;
	Matrix M;
	int isList, N=0;
	void ** array=0;
	PyObject *ilist;
	PyObject * item, *rowItem;
	PyObject * nlist, * clist , * rlist;

	if(!PyArg_ParseTuple(args, "|O", &pylist))
	{
		
		if(!PyArg_ParseTuple(args, "i", &o))  //single arg version
			return NULL;
		else
		{
			M = tc_getStoichiometryFor((void*)(o));
		}
	}
    else
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
			
			M = tc_getStoichiometry(array);
			free(array);
		}
	}
	
	if (array == 0)
	{
		array = tc_allItems();
		M = tc_getStoichiometry(array);
		free(array);
	}
	
	if (M.rows > 0 && M.cols > 0 && M.colnames && M.values)
	{
		int rows = M.rows, cols = M.cols;
		
		ilist = PyTuple_New(3);
		
		nlist = PyTuple_New(rows);
		clist = PyTuple_New(M.cols);
		rlist = PyTuple_New(M.rows);
		
		for (i=0; i < M.cols && M.colnames && M.colnames[i]; i++)
		{
			item = Py_BuildValue("s",M.colnames[i]);
			PyTuple_SetItem(clist, i, item);
		}
		
		for (i=0; i < M.rows && M.rownames && M.rownames[i]; i++)
		{
			item = Py_BuildValue("s",M.rownames[i]);
			PyTuple_SetItem(rlist, i, item);
		}
		
		for (i=0; i < rows; i++)
		{
			rowItem = PyTuple_New(cols);
			for (j=0; j < cols; j++)
			{
				item = Py_BuildValue("d",valueAt(M,i,j));
				PyTuple_SetItem(rowItem, j, item);
			}
			PyTuple_SetItem(nlist, i, rowItem);
		}
		
		PyTuple_SetItem(ilist, 0, rlist);
		PyTuple_SetItem(ilist, 1, clist);
		PyTuple_SetItem(ilist, 2, nlist);
		
		TCFreeMatrix(M);
	}
	else
	{
		ilist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",ilist);
}

static PyObject * pytc_getRates(PyObject *self, PyObject *args)
{
	PyObject * pylist = 0;
	char ** rates = 0;
	int isList, N=0;
	void ** array=0;
	int i,len;
	PyObject *strlist;
	PyObject * item;

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
			
			rates = tc_getRates(array);
			free(array);
		}
	}
	
	if (array == 0)
	{
		array = tc_allItems();
		rates = tc_getRates(array);
		free(array);
	}
	
	if (rates)
	{
		len = 0;
		
		while (rates[len] != 0) ++len;
		
		strlist = PyTuple_New(len);
		
		for (i=0; i<len && rates[i]!=0; i++)
		{
			item = Py_BuildValue("s",rates[i]);
			PyTuple_SetItem(strlist, i, item);
		}
		TCFreeChars(rates);
	}
	else
	{
		strlist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",strlist);
}

static PyObject * pytc_getRate(PyObject *self, PyObject *args)
{
	int o;
	char * s;
	
	if(!PyArg_ParseTuple(args, "i", &o))  //single arg version
		return NULL;

	s = tc_getRate((void*)(o));
	return Py_BuildValue("s", s);
}

static PyObject * pytc_setRates(PyObject *self, PyObject *args)
{
	PyObject * pylist=0, * rates=0;
	int isList1=0, rows1=0, isList2=0, rows2=0, i;
	void ** array;
	char ** crates;
	
	if(!PyArg_ParseTuple(args, "OO", &pylist, &rates))
	{
		return NULL;
	}
	
	if (PyList_Check(pylist) || PyTuple_Check(pylist))
	{
		isList1 = PyList_Check(pylist);
		rows1 = isList1 ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	
	if (PyList_Check(rates) || PyTuple_Check(rates))
	{
		isList2 = PyList_Check(rates);
		rows2 = isList2 ? PyList_Size(rates) : PyTuple_Size (rates);
	}
	
	if (rows2 > rows1) rows2 = rows1;
	if (rows2 < rows1) rows1 = rows2;
	
	if (rows1 == 1 || rows2 < 1) 
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
	
	array = malloc( (1+rows1) * sizeof(void*) );
	array[rows1] = 0;
	
    for(i=0; i < rows1; ++i ) 
    { 
		array[i] = isList1 ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
    } 
	
	crates = malloc( (1+rows1) * sizeof(char*) );
	crates[rows1] = 0;
	
	for(i=0; i < rows1; ++i) 
    { 
		crates[i] = isList2 ? PyString_AsString( PyList_GetItem( rates, i ) ) : PyString_AsString( PyTuple_GetItem( rates, i ) );
    }
	
	tc_setRates(array,crates);
	
	free(array);
	TCFreeChars(crates);
	
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject * pytc_setRate(PyObject *self, PyObject *args)
{
	int o = 0;
	char *  s;
	
	if(!PyArg_ParseTuple(args, "is", &o, &s))  //single arg version
		return NULL;

	tc_setRate((void*)o,s);
	Py_INCREF(Py_None);
	return Py_None;
}
