#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_clearScreen(PyObject *self, PyObject *args)
{
	if (tc_clear)
		tc_clear();
	 
	Py_INCREF(Py_None);
    return Py_None;	
}

static PyObject * pytc_print(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;
	
	if (tc_print)
		tc_print(s);
	 
	Py_INCREF(Py_None);
    return Py_None;	
}

static PyObject * pytc_errorReport(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;
	
	if (tc_errorReport)
		tc_errorReport(s);
	 
	Py_INCREF(Py_None);
    return Py_None;	
}

static PyObject * pytc_writeFile(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;
	
	if (tc_printFile)
		tc_printFile(s);
	 
	Py_INCREF(Py_None);
    return Py_None;	
}

static PyObject * pytc_allItems(PyObject *self, PyObject *args)
{
    int i;
	PyObject *pylist, *item;
	void ** array = 0;
	if (tc_allItems)
		array = tc_allItems();
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

static PyObject * pytc_selectedItems(PyObject *self, PyObject *args)
{
    int i;
	PyObject *pylist, *item;
	void ** array = 0;
	if (tc_selectedItems)
		array = tc_selectedItems();
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

static PyObject * pytc_itemsOfFamily(PyObject *self, PyObject *args)
{
	char * s;
	PyObject * pylist1 = 0;
	
    if(!PyArg_ParseTuple(args, "s|O", &s,&pylist1))
    	return NULL;
		
	int i;
	void ** array = 0; 
	
	if (pylist1 && tc_itemsOfFamilyFrom)
	{
		int isList = PyList_Check(pylist1);
		int N = isList ? PyList_Size(pylist1) : PyTuple_Size (pylist1);
		
		void ** array1 = malloc( (1+N) * sizeof(void*) );
		array1[N] = 0;
		
		for(i=0; i<N; ++i ) 
		{ 
			array1[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist1, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist1, i ) ));
		}
		
		array = tc_itemsOfFamilyFrom(s,array1);
	}
	else
	{
		if (tc_itemsOfFamily)
			array= tc_itemsOfFamily(s);
	}
		
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

static PyObject * pytc_find(PyObject *self, PyObject *args)
{
    char * s;
	
    if(!PyArg_ParseTuple(args, "s", &s))
        return NULL;
		
	 void * o = 0;
	 if (tc_find)
		o = tc_find(s);
	 
	 return Py_BuildValue("i", (int)o);	 
	 
}

static PyObject * pytc_select(PyObject *self, PyObject *args)
{
    int i;
	
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
     
	 if (i && tc_select)
		tc_select((void*)i);
	 
	 Py_INCREF(Py_None);
     return Py_None;
}

static PyObject * pytc_deselect(PyObject *self, PyObject *args)
{

	 if (tc_deselect)
		tc_deselect();	 
	 Py_INCREF(Py_None);
     return Py_None;
}

static PyObject * pytc_getName(PyObject *self, PyObject *args)
{
	int i;
	
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

	if ((i == 0) || (tc_getName == 0)) return NULL;
	 
	const char * s = tc_getName((void*)i);
	 
	return Py_BuildValue("s",s);
}

static PyObject * pytc_rename(PyObject *self, PyObject *args)
{
	int i;
	char * s;
    if(!PyArg_ParseTuple(args, "is", &i, &s))
        return NULL;

	if ((tc_rename == 0) || (i == 0)) return NULL;
	 
	tc_rename((void*)i,s);
	 
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_getFamily(PyObject *self, PyObject *args)
{
	int i;
	
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;

	if ((tc_getName == 0) || (i == 0)) return NULL;
	 
	const char * s = tc_getName((void*)i);
	 
	return Py_BuildValue("s",s);
}

static PyObject * pytc_getNames(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	
	if(!PyArg_ParseTuple(args, "O", &pylist) || (tc_getNames == 0))
		return NULL;
    
	int isList = PyList_Check(pylist);
	int N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	
	void ** array = malloc( (1+N) * sizeof(void*) );
	array[N] = 0;
	
	int i;
    for(i=0; i<N; ++i ) 
    { 
		array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
    }
	
 	char ** names = tc_getNames(array);
	free(array);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_isA(PyObject *self, PyObject *args)
{
	int i;
    char * s;
	
    if(!PyArg_ParseTuple(args, "is", &i, &s) || (tc_isA == 0))
        return NULL;
		
	int k = tc_isA((void*)i,s);
	 
	return Py_BuildValue("i", k);
}

static PyObject * pytc_remove(PyObject *self, PyObject *args)
{
    int i;
	
    if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
     
	 if (i && tc_remove)
		tc_remove((void*)i);
	 
	 Py_INCREF(Py_None);
     return Py_None;
}

static PyObject * pytc_getX(PyObject *self, PyObject *args)
{
	int i;
	
    if(!PyArg_ParseTuple(args, "i", &i) || (tc_getX == 0))
        return NULL;
		
	double k = tc_getX((void*)i);
	 
	return Py_BuildValue("d", k);
}

static PyObject * pytc_getY(PyObject *self, PyObject *args)
{
	int i;
	
    if(!PyArg_ParseTuple(args, "i", &i) || (tc_getY == 0))
        return NULL;
		
	double k = tc_getY((void*)i);
	 
	return Py_BuildValue("d", k);
}

static PyObject * pytc_setPos(PyObject *self, PyObject *args)
{
    int i;
	double x, y;
	
    if(!PyArg_ParseTuple(args, "idd", &i, &x, &y))
        return NULL;
     
	 if (i && tc_setPos)
		tc_setPos((void*)i,x,y);
	 
	 Py_INCREF(Py_None);
     return Py_None;
}

static PyObject * pytc_moveSelected(PyObject *self, PyObject *args)
{
    double x, y;
	
    if(!PyArg_ParseTuple(args, "dd", &x, &y) || (tc_moveSelected == 0))
        return NULL;
     
	tc_moveSelected(x,y);
	 
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_isWindows(PyObject *self, PyObject *args)
{
	int i = 0;
	
	if (tc_isWindows())
		i = tc_isWindows();
	 
    return Py_BuildValue("i", i);
}

static PyObject * pytc_isMac(PyObject *self, PyObject *args)
{
	int i = 0;
	
	if (tc_isMac)
		i = tc_isMac();
	 
    return Py_BuildValue("i", i);
}

static PyObject * pytc_isLinux(PyObject *self, PyObject *args)
{
	int i = 0;
	
	if (tc_isLinux)
		i = tc_isLinux();
	 
    return Py_BuildValue("i", i);
}

static PyObject * pytc_appDir(PyObject *self, PyObject *args)
{
	char * s = "";
	if (tc_appDir)
		s = tc_appDir();
	 
    return Py_BuildValue("s", s);
}

static PyObject * pytc_createInputWindow(PyObject *self, PyObject *args)
{
	//inputs: array of strings (row names), array of doubles (def. values), array of strings (options), lib filename, lib function name, title
	
	PyObject * rownames, * values;
	char *filename, *funcname, *title;
	
	if(!PyArg_ParseTuple(args, "OOsss", &rownames, &values, &filename, &funcname, &title))
        	return NULL;
	
	int isList1 = PyList_Check(rownames);
	int rows1 = isList1 ? PyList_Size(rownames) : PyTuple_Size (rownames);
	
	int isList2 = PyList_Check(values);
	int rows2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	
	if (rows2 > rows1) rows2 = rows1;
	if (rows2 < rows1) rows1 = rows2;
	
	int i;
	Matrix M;
	char * colnames[] = { "values\0", 0 };
	M.cols = 1;
	M.rows = rows1;
	M.colnames = colnames;
	
	M.rownames = malloc( (1+rows1) * sizeof(char*) );
	M.rownames[rows1] = 0;
	for(i=0; i < rows1; ++i) 
    { 
		M.rownames[i] = isList1 ? PyString_AsString( PyList_GetItem( rownames, i ) ) : PyString_AsString( PyTuple_GetItem( rownames, i ) );
    }
	
	M.values = malloc( (1+rows2) * sizeof(double) );
	M.values[rows2] = 0;
	
	for(i=0; i < rows2; ++i) 
    { 
		M.values[i] = isList2 ? PyFloat_AsDouble( PyList_GetItem( values, i ) ) : PyFloat_AsDouble( PyTuple_GetItem( values, i ) );
    }
	
	if (tc_createInputWindowFromFile)
		tc_createInputWindowFromFile(M,filename,funcname, title);
	
	TCFreeChars(M.rownames);
	free(M.values);
	
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_addInputWindowOptions(PyObject *self, PyObject *args)
{
	//inputs: array of strings (row names), array of doubles (def. values), array of strings (options), lib filename, lib function name, title
	
	PyObject * options;
	char *title;
	int i,j;
	
	if(!PyArg_ParseTuple(args, "siiO",&title, &i, &j, &options))
        return NULL;
	
	int isList3 = PyList_Check(options);
	int rows3 = isList3 ? PyList_Size(options) : PyTuple_Size (options);
	
	char ** coptions = malloc( (1+rows3) * sizeof(char*) );
	coptions[rows3] = 0;
	
	for(i=0; i < rows3; ++i) 
    { 
		coptions[i] = isList3 ? PyString_AsString( PyList_GetItem( options, i ) ) : PyString_AsString( PyTuple_GetItem( options, i ) );
    }
	
	if (tc_addInputWindowOptions)
		tc_addInputWindowOptions(title,i,j,coptions);
	
	TCFreeChars(coptions);
	
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_openNewWindow(PyObject *self, PyObject *args)
{
	char * s;
	if(!PyArg_ParseTuple(args, "s", &s))
        return NULL;
	
	if (tc_openNewWindow)
		tc_openNewWindow(s);
	 
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_getNumericalData(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s2 = "", *s3 = "";
	
    if(!PyArg_ParseTuple(args, "is|ss", &i, &s1, &s2, &s3) || (tc_getNumericalData == 0))
        return NULL;
		
    double d = tc_getNumericalData((void*)i,s1,s2,s3);
	return Py_BuildValue("d", d);	 
}

static PyObject * pytc_getTextData(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s2 = "", *s3 = "";
	
    if(!PyArg_ParseTuple(args, "is|ss", &i, &s1, &s2, &s3) || (tc_getTextData == 0))
        return NULL;
		
    char * s = tc_getTextData((void*)i,s1,s2,s3);
	return Py_BuildValue("s", s);	 
}

static PyObject * pytc_setNumericalData(PyObject *self, PyObject *args)
{
	int i;
	double d;
	char * s1, *s2, *s3;
	
    if(!PyArg_ParseTuple(args, "isssd", &i, &s1, &s2, &s3, &d) || (tc_setNumericalData == 0))
        return NULL;
		
    tc_setNumericalData((void*)i,s1,s2,s3,d);
	
	Py_INCREF(Py_None);
    return Py_None; 
}

static PyObject * pytc_setTextData(PyObject *self, PyObject *args)
{
	int i;
	char* s;
	char * s1, *s2, *s3;
	
    if(!PyArg_ParseTuple(args, "issss", &i, &s1, &s2, &s3, &s) || (tc_setTextData == 0))
        return NULL;
		
    tc_setTextData((void*)i,s1,s2,s3,s);
	
	Py_INCREF(Py_None);
    return Py_None; 
}

static PyObject * pytc_getChildren(PyObject *self, PyObject *args)
{
	int i;
	
    if(!PyArg_ParseTuple(args, "i", &i) || (tc_getChildren == 0))
       	return NULL;
		
	PyObject *pylist, *item;
	void ** array = tc_getChildren((void*)i);
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

static PyObject * pytc_getParent(PyObject *self, PyObject *args)
{
	int i;
	
    if(!PyArg_ParseTuple(args, "i", &i) || (tc_getParent == 0))
        return NULL;
		
    void* j = tc_getParent((void*)i);
	return Py_BuildValue("i", (int)j);	 
}

static PyObject * pytc_getNumericalDataRowNames(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	
	if(!PyArg_ParseTuple(args, "is", &i, &s) || (tc_getNumericalDataRowNames == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getNumericalDataRowNames((void*)i,s);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getNumericalDataColNames(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	
	if(!PyArg_ParseTuple(args, "is", &i, &s) || (tc_getNumericalDataColNames == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getNumericalDataColNames((void*)i,s);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getTextDataRowNames(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	
	if(!PyArg_ParseTuple(args, "is", &i, &s) || (tc_getTextDataRowNames == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getTextDataRowNames((void*)i,s);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getTextDataColNames(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	
	if(!PyArg_ParseTuple(args, "is", &i, &s) || (tc_getTextDataColNames == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getTextDataColNames((void*)i,s);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getTextDataRow(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s2;
	
	if(!PyArg_ParseTuple(args, "iss", &i, &s1, &s2) || (tc_getTextDataRow == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getTextDataRow((void*)i,s1,s2);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getTextDataCol(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s2;
	
	if(!PyArg_ParseTuple(args, "iss", &i, &s1, &s2) || (tc_getTextDataCol == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getTextDataCol((void*)i,s1,s2);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getNumericalDataMatrix(PyObject *self, PyObject *args)
{
	char * s;
	int o,i,j;
	
	Matrix M;
	
	if(!PyArg_ParseTuple(args, "is", &o, &s) || (tc_getNumericalDataMatrix == 0))
		return NULL;
	
	M = tc_getNumericalDataMatrix((void*)(o),s);
	
	PyObject *ilist;
	PyObject * item, *rowItem;
	
	if (M.rows > 0 && M.cols > 0 && M.colnames && M.values)
	{
		int rows = M.rows, cols = M.cols;
		
		ilist = PyTuple_New(3);
		
		PyObject * nlist = PyTuple_New(rows);
		PyObject * clist = PyTuple_New(M.cols);
		PyObject * rlist = PyTuple_New(M.rows);
		
		for (i=0; i < M.cols; i++)
		{
			item = Py_BuildValue("s",M.colnames[i]);
			PyTuple_SetItem(clist, i, item);
		}
		
		for (i=0; i < M.rows; i++)
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

static PyObject * pytc_setPosMulti(PyObject *self, PyObject *args)
{
	PyObject * items, * values, *item;
	if(!PyArg_ParseTuple(args, "OO", &items, &values) || (tc_setPosMulti == 0))
        return NULL;
	
	int isList1 = PyList_Check(items);
	int n1 = isList1 ? PyList_Size(items) : PyTuple_Size (items);
	
	int isList2 = PyList_Check(values);
	int n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	
	int n3, isList3, rows;
	
	rows = 0;
	
	if (n1 > 0 && n2 == 2)
	{
		Array array = malloc( (1+n1) * sizeof(OBJ) );
		array[n1] = 0;
	
		int i,j;
		for(i=0; i<n1; ++i) 
		{ 
			array[i] = isList1 ? (void*)PyInt_AsLong( PyList_GetItem( items, i ) ) : (void*)PyInt_AsLong( PyTuple_GetItem( items, i ) );
		}
		
		//find the smallest row size (in case input is incorrect)
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList3 = PyList_Check(item);
			n3 = isList3 ? PyList_Size(item) : PyTuple_Size (item);
			
			if (n3 < rows || rows == 0) rows = n3;
		}
		
		double * nums = malloc( n2 * rows * sizeof(double) );
		
		//make the matrix
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList3 = PyList_Check(item);
			
			for (j=0; j < rows; ++j)
			{
				nums[ j*n2 + i ] = isList3 ? PyFloat_AsDouble( PyList_GetItem( item, j ) ) : PyFloat_AsDouble( PyTuple_GetItem( item, j ) );
			}
			
			//nums[i] = isList2 ? PyFloat_AsDouble( PyList_GetItem( values, i ) ) : PyFloat_AsDouble( PyTuple_GetItem( values, i ) );
		}
		
		Matrix M;
		M.cols = n2;
		M.rows = rows;
		
		M.colnames = 0;
		M.rownames = 0;
		M.values = nums;
		
		tc_setPosMulti(array,M);
	
		TCFreeMatrix(M);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_getPos(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int i,j;
	
	if(!PyArg_ParseTuple(args, "O", &pylist) || (tc_getPos == 0))
		return NULL;
    
	int isList = PyList_Check(pylist);
	int N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	
	void ** array = malloc( (1+N) * sizeof(void*) );
	array[N] = 0;
	
	for(i=0; i<N; ++i ) 
	{ 
		array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
    } 
	
 	
	Matrix M;
	M = tc_getPos(array);
	free(array);
	
	PyObject * nlist;
	PyObject * item, *rowItem;
	
	if (M.rows > 0 && M.cols > 0 && M.values)
	{
		int rows = M.rows, cols = M.cols;
		
		nlist = PyTuple_New(cols);
		
		for (i=0; i < cols; i++)
		{
			rowItem = PyTuple_New(rows);
			for (j=0; j < rows; j++)
			{
				item = Py_BuildValue("d",valueAt(M,j,i));
				PyTuple_SetItem(rowItem, j, item);
			}
			PyTuple_SetItem(nlist, i, rowItem);
		}
		
		TCFreeMatrix(M);
	}
	else
	{
		nlist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",nlist);
}

static PyObject * pytc_getNumericalDataNames(PyObject *self, PyObject *args)
{
	int i;
	
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getNumericalDataNames == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getNumericalDataNames((void*)i);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_getTextDataNames(PyObject *self, PyObject *args)
{
	int i;
	
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getTextDataNames == 0))
	{
		return NULL;
	}
	
 	char ** names = tc_getTextDataNames((void*)i);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_setNumericalDataMatrix(PyObject *self, PyObject *args)
{
	PyObject * colNames, * rowNames, * values, *item;
	int object = 0;
	char * title = "";
	if(!PyArg_ParseTuple(args, "sOOO|i", &title, &colNames, &rowNames, &values, &object))
        	return NULL;
	
	int isList1 = PyList_Check(colNames);
	int n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	
	int isList2 = PyList_Check(rowNames);
	int n2 = isList2 ? PyList_Size(rowNames) : PyTuple_Size (rowNames);
	
	int isList3 = PyList_Check(values);
	int n3 = isList3 ? PyList_Size(values) : PyTuple_Size (values);
	
	int n4, isList4, numRows;
	
	numRows = 0;
	
	if (n3 > 0)
	{
		int i,j;
		char ** cols = 0;
		if (n1 == n3)
		{
			cols = malloc( (1+n1) * sizeof(char*) );
			cols[n1] = 0;
		
			for(i=0; i<n1; ++i) 
			{ 
				cols[i] = isList1 ? PyString_AsString( PyList_GetItem( colNames, i ) ) : PyString_AsString( PyTuple_GetItem( colNames, i ) );
			}
		}
		
		numRows = 0;
		//find the smallest row size (in case input is incorrect)
		for(i=0; i<n3; ++i) 
		{ 
			item = isList3 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList4 = PyList_Check(item);
			n4 = isList4 ? PyList_Size(item) : PyTuple_Size (item);
			
			if (n4 < numRows || numRows == 0) numRows = n4;
		}
		
		char ** rows = 0;
		if (n2 == numRows)
		{
			rows = malloc( (1+n2) * sizeof(char*) );
			rows[n2] = 0;
		
			for(i=0; i<n2; ++i) 
			{ 
				rows[i] = isList2 ? PyString_AsString( PyList_GetItem( rowNames, i ) ) : PyString_AsString( PyTuple_GetItem( rowNames, i ) );
			}
		}
		
		double * nums = malloc( n3 * numRows * sizeof(double) );
		
		//make the matrix
		for(i=0; i<n3; ++i) 
		{ 
			item = isList3 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList3 = PyList_Check(item);
			
			for (j=0; j < numRows; ++j)
			{
				nums[ j*n3 + i ] = isList3 ? PyFloat_AsDouble( PyList_GetItem( item, j ) ) : PyFloat_AsDouble( PyTuple_GetItem( item, j ) );
			}
			
			//nums[i] = isList3 ? PyFloat_AsDouble( PyList_GetItem( values, i ) ) : PyFloat_AsDouble( PyTuple_GetItem( values, i ) );
		}
		
		Matrix M;
		M.cols = n3;
		
		M.rows = numRows;
		
		M.colnames = cols;
		M.rownames = rows;
		M.values = nums;
		
		if (tc_setNumericalDataMatrix)
			tc_setNumericalDataMatrix((void*)object,title,M);
		
		tc_print("done");
	
		TCFreeMatrix(M);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_zoom(PyObject *self, PyObject *args)
{
	double x = 1.2;
	if(!PyArg_ParseTuple(args, "d", &x))
        return NULL;
			
	if (tc_zoom)
		tc_zoom(x);
	 
	Py_INCREF(Py_None);
    	return Py_None;	
}

static PyObject * pytc_getAnnotation(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int i;
	if(!PyArg_ParseTuple(args, "i", &i) || (tc_getAnnotation == 0))
		return NULL;
    
 	char ** names = tc_getAnnotation((void*)i);
	
	PyObject *strlist;
	PyObject * item;
	
	if (names)
	{
		int len = 0;
		
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

static PyObject * pytc_setAnnotation(PyObject *self, PyObject *args)
{
	//inputs: array of strings (row names), array of doubles (def. values), array of strings (options), lib filename, lib function name, title
	
	PyObject * options;
	int i,j,k;
	
	if(!PyArg_ParseTuple(args, "iO",&k, &options))
        return NULL;
	
	int isList3 = PyList_Check(options);
	int rows3 = isList3 ? PyList_Size(options) : PyTuple_Size (options);
	
	char ** annotations = malloc( (1+rows3) * sizeof(char*) );
	annotations[rows3] = 0;
	
	for(i=0; i < rows3; ++i) 
    { 
		annotations[i] = isList3 ? PyString_AsString( PyList_GetItem( options, i ) ) : PyString_AsString( PyTuple_GetItem( options, i ) );
    }
	
	if (tc_setAnnotation)
		tc_setAnnotation((void*)k,annotations);
	
	TCFreeChars(annotations);
	
    Py_INCREF(Py_None);
    return Py_None;
}
