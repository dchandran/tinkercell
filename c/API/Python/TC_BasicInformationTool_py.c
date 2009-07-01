#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_modelParameters(PyObject *self, PyObject *args)
{
	int i;
	PyObject * pylist = 0, *item;
	if(!PyArg_ParseTuple(args, "|O", &pylist))
        	return NULL;

	int isList = 1;
	int n = 1;
	if (pylist)
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	PyObject * params, *values, *twoTuples;

	if (n > 0)
	{
		void ** array;
		if (pylist)
		{
			array = malloc( (1+n) * sizeof(void*) );
			array[n] = 0;
		
			for(i=0; i<n; ++i) 
			{ 
				array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			}
		}
		else
		{
			array = tc_allItems();
		}
		Matrix M = tc_getModelParameters(array);
		free(array);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}


static PyObject * pytc_allParameters(PyObject *self, PyObject *args)
{
	int i;
	PyObject * pylist = 0, *item;
	if(!PyArg_ParseTuple(args, "|O", &pylist))
        	return NULL;

	int isList = 1;
	int n = 1;
	if (pylist)
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	PyObject * params, *values, *twoTuples;

	if (n > 0)
	{
		void ** array;
		if (pylist)
		{
			array = malloc( (1+n) * sizeof(void*) );
			array[n] = 0;
		
			for(i=0; i<n; ++i) 
			{ 
				array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			}
		}
		else
		{
			array = tc_allItems();
		}

		Matrix M = tc_getAllParameters(array);
		free(array);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}

static PyObject * pytc_allInitialValues(PyObject *self, PyObject *args)
{
	int i;
	PyObject * pylist = 0, *item;
	if(!PyArg_ParseTuple(args, "|O", &pylist))
        	return NULL;

	int isList = 1;
	int n = 1;
	if (pylist)
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	PyObject * params, *values, *twoTuples;

	if (n > 0)
	{
		void ** array;
		if (pylist)
		{
			array = malloc( (1+n) * sizeof(void*) );
			array[n] = 0;
		
			for(i=0; i<n; ++i) 
			{ 
				array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			}
		}
		else
		{
			array = tc_allItems();
		}

		Matrix M = tc_getInitialValues(array);
		free(array);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}

static PyObject * pytc_allFixedVars(PyObject *self, PyObject *args)
{
	int i;
	PyObject * pylist = 0, *item;
	if(!PyArg_ParseTuple(args, "|O", &pylist))
        	return NULL;

	int isList = 1;
	int n = 1;
	if (pylist)
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	PyObject * params, *values, *twoTuples;

	if (n > 0)
	{
		void ** array;
		if (pylist)
		{
			array = malloc( (1+n) * sizeof(void*) );
			array[n] = 0;
		
			for(i=0; i<n; ++i) 
			{ 
				array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			}
		}
		else
		{
			array = tc_allItems();
		}

		Matrix M = tc_getFixedVariables(array);
		free(array);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}

static PyObject * pytc_allParamsAndFixedVars(PyObject *self, PyObject *args)
{
	int i;
	PyObject * pylist = 0, *item;
	if(!PyArg_ParseTuple(args, "|O", &pylist))
        	return NULL;

	int isList = 1;
	int n = 1;
	if (pylist)
	{
		isList = PyList_Check(pylist);
		n = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}
	PyObject * params, *values, *twoTuples;

	if (n > 0)
	{
		void ** array;
		if (pylist)
		{
			array = malloc( (1+n) * sizeof(void*) );
			array[n] = 0;
		
			for(i=0; i<n; ++i) 
			{ 
				array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			}
		}
		else
		{
			array = tc_allItems();
		}

		Matrix M = tc_getParametersAndFixedVariables(array);
		free(array);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}

static PyObject * pytc_getTextAttribute(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	if(!PyArg_ParseTuple(args, "is", &i, &s))
        	return NULL;

	char * c = tc_getTextAttribute((void*)i,s);
	
    	return Py_BuildValue("s",c);
}

static PyObject * pytc_getParameter(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	if(!PyArg_ParseTuple(args, "is", &i, &s))
        	return NULL;

	double d = tc_getParameter((void*)i,s);
	
    	return Py_BuildValue("d",d);
}


static PyObject * pytc_getParametersNamed(PyObject *self, PyObject *args)
{
	PyObject * pylist1, * pylist2, *item;
	if(!PyArg_ParseTuple(args, "OO", &pylist1, &pylist2))
        	return NULL;

	int isList1 = PyList_Check(pylist1);
	int n1 = isList1 ? PyList_Size(pylist1) : PyTuple_Size (pylist1);

	int isList2 = PyList_Check(pylist2);
	int n2 = isList2 ? PyList_Size(pylist2) : PyTuple_Size (pylist2);

	PyObject * params, *values, *twoTuples;

	if (n1 > 0 && n2 > 0)
	{
		void ** array1 = malloc( (1+n1) * sizeof(void*) );
		array1[n1] = 0;

		char ** array2 = malloc( (1+n2) * sizeof(char*) );
		array2[n2] = 0;
	
		int i;
		for(i=0; i<n1; ++i) 
		{ 
			array1[i] = isList1 ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist1, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist1, i ) ));
		}

		for(i=0; i<n2; ++i) 
		{ 
			array2[i] = isList2 ? PyString_AsString( PyList_GetItem( pylist2, i ) ) : PyString_AsString( PyTuple_GetItem( pylist2, i ));
		}

		Matrix M = tc_getParametersNamed(array1,array2);
		free(array1);
		free(array2);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}

static PyObject * pytc_getParametersExcept(PyObject *self, PyObject *args)
{
	PyObject * pylist1, * pylist2, *item;
	if(!PyArg_ParseTuple(args, "OO", &pylist1, &pylist2))
        	return NULL;

	int isList1 = PyList_Check(pylist1);
	int n1 = isList1 ? PyList_Size(pylist1) : PyTuple_Size (pylist1);

	int isList2 = PyList_Check(pylist2);
	int n2 = isList2 ? PyList_Size(pylist2) : PyTuple_Size (pylist2);

	PyObject * params, *values, *twoTuples;

	if (n1 > 0 && n2 > 0)
	{
		void ** array1 = malloc( (1+n1) * sizeof(void*) );
		array1[n1] = 0;

		char ** array2 = malloc( (1+n2) * sizeof(char*) );
		array2[n2] = 0;
	
		int i;
		for(i=0; i<n1; ++i) 
		{ 
			array1[i] = isList1 ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist1, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist1, i ) ));
		}

		for(i=0; i<n2; ++i) 
		{ 
			array2[i] = isList2 ? PyString_AsString( PyList_GetItem( pylist2, i ) ) : PyString_AsString( PyTuple_GetItem( pylist2, i ));
		}

		Matrix M = tc_getParametersExcept(array1,array2);
		free(array1);
		TCFreeChars(array2);

		if (M.rows > 0 && M.values && M.rownames)
		{
			params = PyTuple_New(M.rows);
			values = PyTuple_New(M.rows);

			for (i=0; i < M.rows; i++) 
			{
				item = Py_BuildValue("s",M.rownames[i]);
				PyTuple_SetItem(params, i, item);
				
				item = Py_BuildValue("d",valueAt(M,i,0));
				PyTuple_SetItem(values, i, item);
			}
			TCFreeMatrix(M);
		}
		else
		{
			params = PyTuple_New(0);
			values = PyTuple_New(0);
		}
		
	}
	
	twoTuples = PyTuple_New(2);
	PyTuple_SetItem(twoTuples,0,params);
	PyTuple_SetItem(twoTuples,1,values);
	 
   	return Py_BuildValue("O",twoTuples);
}

static PyObject * pytc_getAllTextNamed(PyObject *self, PyObject *args)
{
	PyObject * pylist1, * pylist2, *item;
	if(!PyArg_ParseTuple(args, "OO", &pylist1, &pylist2))
        	return NULL;

	int isList1 = PyList_Check(pylist1);
	int n1 = isList1 ? PyList_Size(pylist1) : PyTuple_Size (pylist1);

	int isList2 = PyList_Check(pylist2);
	int n2 = isList2 ? PyList_Size(pylist2) : PyTuple_Size (pylist2);

	PyObject * params;

	if (n1 > 0 && n2 > 0)
	{
		void ** array1 = malloc( (1+n1) * sizeof(void*) );
		array1[n1] = 0;

		char ** array2 = malloc( (1+n2) * sizeof(char*) );
		array2[n2] = 0;
	
		int i;
		for(i=0; i<n1; ++i) 
		{ 
			array1[i] = isList1 ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist1, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist1, i ) ));
		}

		for(i=0; i<n2; ++i) 
		{ 
			array2[i] = isList2 ? PyString_AsString( PyList_GetItem( pylist2, i ) ) : PyString_AsString( PyTuple_GetItem( pylist2, i ));
		}

		char ** M = tc_getAllTextNamed(array1,array2);
		free(array1);
		TCFreeChars(array2);

		if (M)
		{
		
			int len = 0;
			
			while (M[len] != 0) ++len;
		
			params = PyTuple_New(len);

			for (i=0; i < len; i++)
			{
				item = Py_BuildValue("s",M[i]);
				PyTuple_SetItem(params, i, item);
			}
			TCFreeChars(M);
		}
		else
		{
			params = PyTuple_New(0);
		}
		
	}
	else
	{
		params = PyTuple_New(0);
	}
	 
    return Py_BuildValue("O",params);
}


static PyObject * pytc_setTextAttribute(PyObject *self, PyObject *args)
{
	int i;
	char * s, * c;
	if(!PyArg_ParseTuple(args, "iss", &i, &s, &c))
        	return NULL;
	
	tc_setTextAttribute((void*)i,s,c);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_setParameter(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	double d;
	if(!PyArg_ParseTuple(args, "isd", &i, &s, &d))
        	return NULL;
	
	tc_setParameter((void*)i,s,d);
	
	Py_INCREF(Py_None);
    return Py_None;
}
