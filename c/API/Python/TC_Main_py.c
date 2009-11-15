#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_clearScreen(PyObject *self, PyObject *args)
{
	tc_clear();

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_print(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_print(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_errorReport(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_errorReport(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_writeFile(PyObject *self, PyObject *args)
{
	char * s;

	if (!PyArg_ParseTuple(args, "s", &s))
		return NULL;

	tc_printFile(s);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_allItems(PyObject *self, PyObject *args)
{
	int i,len;
	PyObject *pylist, *item;
	void ** array = 0;
	array = tc_allItems();
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

static PyObject * pytc_selectedItems(PyObject *self, PyObject *args)
{
	int i;
	PyObject *pylist, *item;
	void ** array = 0;
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
	PyObject *pylist, *item;
	int i = 0, isList, N=0, len = 0;
	void ** array = 0, ** array1; 

	if(!PyArg_ParseTuple(args, "s|O", &s,&pylist1))
		return NULL;

	if (pylist1 && tc_itemsOfFamilyFrom)
	{
		if (PyList_Check(pylist1) || PyTuple_Check(pylist1))
		{
			isList = PyList_Check(pylist1);
			N = isList ? PyList_Size(pylist1) : PyTuple_Size (pylist1);
		}
		
		if (N > 0)
		{
			array1 = malloc( (1+N) * sizeof(void*) );
			array1[N] = 0;

			for(i=0; i<N; ++i ) 
			{ 
				array1[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist1, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist1, i ) ));
			}

			array = tc_itemsOfFamilyFrom(s,array1);
		}
	}
	else
	{
		array= tc_itemsOfFamily(s);
	}

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

static PyObject * pytc_find(PyObject *self, PyObject *args)
{
	char * s = 0;
	void * o = 0;
	PyObject *pylist, *outlist, *item;
	int i = 0, isList, N=0, len = 0;
	void ** array = 0;
	char ** names=0; 

	if (!PyArg_ParseTuple(args, "O", &pylist) || !tc_find || !tc_findItems)
		return NULL;

	if (PyString_Check(pylist))
		s = PyString_AsString(pylist);
	
	if (s)
	{
		o = tc_find(s);
		return Py_BuildValue("i", (int)o);	 
	}
	else
	{
		if (PyList_Check(pylist) || PyTuple_Check(pylist))
		{
			isList = PyList_Check(pylist);
			N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
		}
		
		if (N > 0)
		{
			names = malloc( (1+N) * sizeof(char*) );
			names[N] = 0;

			for(i=0; i<N; ++i ) 
			{ 
				names[i] = isList ? PyString_AsString( PyList_GetItem( pylist, i ) ) : PyString_AsString( PyTuple_GetItem( pylist, i ) );
			}

			array = tc_findItems(names);
			free(names);
		}
		
		if (array)
		{
			len = 0;
			while (array[len] != 0) ++len;

			outlist = PyTuple_New(len);

			for (i=0; i<len; ++i) 
			{
				item = Py_BuildValue("i",(int)(array[i]));
				PyTuple_SetItem(outlist, i, item);
			}
			free(array);
			return outlist;
		}
	}

	outlist =  PyTuple_New(0);
	return outlist;	 
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

	tc_deselect();	 
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_getName(PyObject *self, PyObject *args)
{
	int i;
	const char * s;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	if ((i == 0)) return NULL;

	s = tc_getName((void*)i);

	return Py_BuildValue("s",s);
}

static PyObject * pytc_rename(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	if(!PyArg_ParseTuple(args, "is", &i, &s))
		return NULL;

	if ((i == 0)) return NULL;

	tc_rename((void*)i,s);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_getFamily(PyObject *self, PyObject *args)
{
	int i;

	const char * s;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	if ((i == 0)) return NULL;

	s = tc_getName((void*)i);

	return Py_BuildValue("s",s);
}

static PyObject * pytc_getNames(PyObject *self, PyObject *args)
{
	PyObject * pylist;
	int isList;
	int N=0;
	void ** array=0;
	int i;
	char ** names=0;
	PyObject *strlist;
	PyObject * item;
	
	if(!PyArg_ParseTuple(args, "O", &pylist))
		return NULL;

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

		names = tc_getNames(array);
		free(array);
	}
	
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
	int i,k;
	char * s;

	if(!PyArg_ParseTuple(args, "is", &i, &s))
		return NULL;

	k = tc_isA((void*)i,s);

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
	double k;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	k = tc_getX((void*)i);

	return Py_BuildValue("d", k);
}

static PyObject * pytc_getY(PyObject *self, PyObject *args)
{
	int i;
	double k;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	k = tc_getY((void*)i);

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

	if(!PyArg_ParseTuple(args, "dd", &x, &y))
		return NULL;

	tc_moveSelected(x,y);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_isWindows(PyObject *self, PyObject *args)
{
	int i = 0;

	i = tc_isWindows();

	return Py_BuildValue("i", i);
}

static PyObject * pytc_isMac(PyObject *self, PyObject *args)
{
	int i = 0;

	i = tc_isMac();

	return Py_BuildValue("i", i);
}

static PyObject * pytc_isLinux(PyObject *self, PyObject *args)
{
	int i = 0;

	i = tc_isLinux();

	return Py_BuildValue("i", i);
}

static PyObject * pytc_appDir(PyObject *self, PyObject *args)
{
	char * s = "";
	s = tc_appDir();

	return Py_BuildValue("s", s);
}

static PyObject * pytc_createInputWindow(PyObject *self, PyObject *args)
{
	//inputs: array of strings (row names), array of doubles (def. values), array of strings (options), lib filename, lib function name, title
	int isList1, rows1=0, isList2, rows2=0;
	PyObject * rownames, * values;
	char *filename, *funcname, *title;
	int i;
	Matrix M;
	char * colnames[] = { "values\0", 0 };

	if(!PyArg_ParseTuple(args, "OOsss", &rownames, &values, &filename, &funcname, &title))
		return NULL;

	if (PyList_Check(rownames) || PyTuple_Check(rownames))
	{
		isList1 = PyList_Check(rownames);
		rows1 = isList1 ? PyList_Size(rownames) : PyTuple_Size (rownames);
	}
	
	if (PyList_Check(values) || PyTuple_Check(values))
	{
		isList2 = PyList_Check(values);
		rows2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	}
	
	if (rows1 < 1 || rows2 < 1) 
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (rows2 > rows1) rows2 = rows1;
	if (rows2 < rows1) rows1 = rows2;

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
	char *title, **coptions;
	int i,j, isList3, rows3=0;

	if(!PyArg_ParseTuple(args, "siiO",&title, &i, &j, &options))
		return NULL;

	if (PyList_Check(options) || PyTuple_Check(options))
	{
		isList3 = PyList_Check(options);
		rows3 = isList3 ? PyList_Size(options) : PyTuple_Size (options);
	}
	
	if (rows3 < 1)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	coptions = malloc( (1+rows3) * sizeof(char*) );
	coptions[rows3] = 0;

	for(i=0; i < rows3; ++i) 
	{ 
		coptions[i] = isList3 ? PyString_AsString( PyList_GetItem( options, i ) ) : PyString_AsString( PyTuple_GetItem( options, i ) );
	}

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

	tc_openNewWindow(s);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_getNumericalData(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s2 = "", *s3 = "";
	double d;

	if(!PyArg_ParseTuple(args, "is|ss", &i, &s1, &s2, &s3))
		return NULL;

	d = tc_getNumericalData((void*)i,s1,s2,s3);
	return Py_BuildValue("d", d);	 
}

static PyObject * pytc_getTextData(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s, *s2 = "", *s3 = "";

	if(!PyArg_ParseTuple(args, "is|ss", &i, &s1, &s2, &s3))
		return NULL;

	s = tc_getTextData((void*)i,s1,s2,s3);
	return Py_BuildValue("s", s);	 
}

static PyObject * pytc_setNumericalData(PyObject *self, PyObject *args)
{
	int i;
	double d;
	char * s1, *s2, *s3;

	if(!PyArg_ParseTuple(args, "isssd", &i, &s1, &s2, &s3, &d))
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

	if(!PyArg_ParseTuple(args, "issss", &i, &s1, &s2, &s3, &s))
		return NULL;

	tc_setTextData((void*)i,s1,s2,s3,s);

	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject * pytc_getChildren(PyObject *self, PyObject *args)
{
	int i;
	void ** array;
	PyObject *pylist, *item;
	int len = 0;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	array = tc_getChildren((void*)i);
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

static PyObject * pytc_getParent(PyObject *self, PyObject *args)
{
	int i;
	void* j;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	j = tc_getParent((void*)i);
	return Py_BuildValue("i", (int)j);	 
}

static PyObject * pytc_getNumericalDataRowNames(PyObject *self, PyObject *args)
{
	int i,len;
	char * s, ** names;
	PyObject *strlist;
	PyObject * item;

	if(!PyArg_ParseTuple(args, "is", &i, &s))
	{
		return NULL;
	}

	names = tc_getNumericalDataRowNames((void*)i,s);

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

static PyObject * pytc_getNumericalDataColNames(PyObject *self, PyObject *args)
{
	int i;
	char * s;
	PyObject *strlist;
	PyObject * item;
	char ** names;
	int len = 0;

	if(!PyArg_ParseTuple(args, "is", &i, &s))
	{
		return NULL;
	}

	names = tc_getNumericalDataColNames((void*)i,s);

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

static PyObject * pytc_getTextDataRowNames(PyObject *self, PyObject *args)
{
	int i;
	char * s, **names;
	PyObject *strlist;
	PyObject * item;
	int len = 0;

	if(!PyArg_ParseTuple(args, "is", &i, &s))
	{
		return NULL;
	}

	names = tc_getTextDataRowNames((void*)i,s);

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

static PyObject * pytc_getTextDataColNames(PyObject *self, PyObject *args)
{
	int i,len;
	char * s;
	char ** names;
	PyObject *strlist;
	PyObject * item;

	if(!PyArg_ParseTuple(args, "is", &i, &s))
	{
		return NULL;
	}

	names = tc_getTextDataColNames((void*)i,s);

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

static PyObject * pytc_getTextDataRow(PyObject *self, PyObject *args)
{
	int i;
	char * s1, *s2;
	char** names;
	PyObject *strlist;
	PyObject * item;
	int len = 0;

	if(!PyArg_ParseTuple(args, "iss", &i, &s1, &s2))
	{
		return NULL;
	}

	names = tc_getTextDataRow((void*)i,s1,s2);

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

static PyObject * pytc_getTextDataCol(PyObject *self, PyObject *args)
{
	int i,len;
	char * s1, *s2;
	char ** names;
	PyObject *strlist;
	PyObject * item;

	if(!PyArg_ParseTuple(args, "iss", &i, &s1, &s2))
	{
		return NULL;
	}

	names = tc_getTextDataCol((void*)i,s1,s2);

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

static PyObject * pytc_getNumericalDataMatrix(PyObject *self, PyObject *args)
{
	char * s;
	int o,i,j,rows,cols;
	Matrix M;
	PyObject *ilist, *nlist, *clist, *rlist;
	PyObject * item, *rowItem;

	if(!PyArg_ParseTuple(args, "is", &o, &s))
		return NULL;

	M = tc_getNumericalDataMatrix((void*)(o),s);

	if (M.rows > 0 && M.cols > 0 && M.colnames && M.values)
	{
		rows = M.rows;
		cols = M.cols;

		ilist = PyTuple_New(3);

		nlist = PyTuple_New(rows);
		clist = PyTuple_New(M.cols);
		rlist = PyTuple_New(M.rows);

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
	int isList1, n1=0, isList2, n2=0;
	int n3, isList3, rows;
	int i,j;
	PyObject * items, * values, *item;
	Array array;
	double * nums;
	Matrix M;

	if(!PyArg_ParseTuple(args, "OO", &items, &values))
		return NULL;

	if (PyList_Check(items) || PyTuple_Check(items))
	{
		isList1 = PyList_Check(items);
		n1 = isList1 ? PyList_Size(items) : PyTuple_Size (items);
	}

	if (PyList_Check(values) || PyTuple_Check(values))
	{
		isList2 = PyList_Check(values);
		n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	}

	rows = 0;

	if (n1 > 0 && n2 == 2)
	{
		array = malloc( (1+n1) * sizeof(OBJ) );
		array[n1] = 0;

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

		nums = malloc( n2 * rows * sizeof(double) );

		//make the matrix
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList3 = PyList_Check(item);

			for (j=0; j < rows; ++j)
			{
				nums[ j*n2 + i ] = isList3 ? PyFloat_AsDouble( PyList_GetItem( item, j ) ) : PyFloat_AsDouble( PyTuple_GetItem( item, j ) );
			}
		}

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
	int i,j,isList,N=0,rows,cols;
	void ** array;
	Matrix M;
	PyObject * nlist;
	PyObject * item, *rowItem;

	if(!PyArg_ParseTuple(args, "O", &pylist))
		return NULL;

	if (PyList_Check(pylist) || PyTuple_Check(pylist))
	{
		isList = PyList_Check(pylist);
		N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	}

	array = malloc( (1+N) * sizeof(void*) );
	array[N] = 0;

	for(i=0; i<N; ++i ) 
	{ 
		array[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
	}

	M = tc_getPos(array);
	free(array);

	if (M.rows > 0 && M.cols > 0 && M.values)
	{
		rows = M.rows;
		cols = M.cols;

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
	char ** names;
	PyObject *strlist;
	PyObject * item;
	int len = 0;

	if(!PyArg_ParseTuple(args, "i", &i))
	{
		return NULL;
	}

	names = tc_getNumericalDataNames((void*)i);

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

static PyObject * pytc_getTextDataNames(PyObject *self, PyObject *args)
{
	int i,len;
	char ** names;
	PyObject *strlist;
	PyObject * item;

	if(!PyArg_ParseTuple(args, "i", &i))
	{
		return NULL;
	}

	names = tc_getTextDataNames((void*)i);

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

static PyObject * pytc_setNumericalDataMatrix(PyObject *self, PyObject *args)
{
	PyObject * colNames, * rowNames, * values, *item;
	int object = 0;
	char * title = "";
	int isList1, n1=0, isList2, n2=0, isList3, n3=0;
	int n4, isList4, numRows;
	int i,j;
	char ** cols = 0;
	char ** rows = 0;
	double * nums;
	Matrix M;

	if(!PyArg_ParseTuple(args, "sOOO|i", &title, &colNames, &rowNames, &values, &object))
		return NULL;

	if (PyList_Check(colNames) || PyTuple_Check(colNames))
	{
		isList1 = PyList_Check(colNames);
		n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	}

	if (PyList_Check(rowNames) || PyTuple_Check(rowNames))
	{
		isList2 = PyList_Check(rowNames);
		n2 = isList2 ? PyList_Size(rowNames) : PyTuple_Size (rowNames);
	}

	if (PyList_Check(values) || PyTuple_Check(values))
	{
		isList3 = PyList_Check(values);
		n3 = isList3 ? PyList_Size(values) : PyTuple_Size (values);
	}

	numRows = 0;

	if (n3 > 0)
	{
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

		if (n2 == numRows)
		{
			rows = malloc( (1+n2) * sizeof(char*) );
			rows[n2] = 0;

			for(i=0; i<n2; ++i) 
			{ 
				rows[i] = isList2 ? PyString_AsString( PyList_GetItem( rowNames, i ) ) : PyString_AsString( PyTuple_GetItem( rowNames, i ) );
			}
		}

		nums = malloc( n3 * numRows * sizeof(double) );

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

		M.cols = n3;

		M.rows = numRows;

		M.colnames = cols;
		M.rownames = rows;
		M.values = nums;

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

	tc_zoom(x);

	Py_INCREF(Py_None);
	return Py_None;	
}

static PyObject * pytc_getAnnotation(PyObject *self, PyObject *args)
{
	PyObject *strlist;
	PyObject * item;
	int i,len;
	char ** names;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	names = tc_getAnnotation((void*)i);

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

static PyObject * pytc_setAnnotation(PyObject *self, PyObject *args)
{
	//inputs: array of strings (row names), array of doubles (def. values), array of strings (options), lib filename, lib function name, title

	PyObject * options;
	int i,k,isList3,rows3=0;
	char ** annotations;

	if(!PyArg_ParseTuple(args, "iO",&k, &options))
		return NULL;

	if (PyList_Check(options) || PyTuple_Check(options))
	{
		isList3 = PyList_Check(options);
		rows3 = isList3 ? PyList_Size(options) : PyTuple_Size (options);
	}

	annotations = malloc( (1+rows3) * sizeof(char*) );
	annotations[rows3] = 0;

	for(i=0; i < rows3; ++i) 
	{
		annotations[i] = isList3 ? PyString_AsString( PyList_GetItem( options, i ) ) : PyString_AsString( PyTuple_GetItem( options, i ) );
	}

	tc_setAnnotation((void*)k,annotations);

	TCFreeChars(annotations);

	Py_INCREF(Py_None);
	return Py_None;
}
