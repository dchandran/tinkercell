#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"
#include "../../cvodesim.h"

static PyObject * pytc_plot(PyObject *self, PyObject *args)
{
	PyObject * colNames, * values, *item;
	int xaxis = 0, opt = 1;
	char * title = "";
	int isList1,n1=0,isList2,n2=0;
	int n3, isList3, rows;
	char ** cols;
	int i,j;
	double * nums;
	Matrix M;

	if(!PyArg_ParseTuple(args, "OO|isi", &colNames, &values, &xaxis, &title, &opt))
        return NULL;
	
	if (PyList_Check(colNames) || PyTuple_Check(colNames))
	{
		isList1 = PyList_Check(colNames);
		n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	}
	
	if (PyList_Check(values) || PyTuple_Check(values))
	{
		isList2 = PyList_Check(values);
		n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	}
	
	rows = 0;
	
	if (n1 > 0 && n2 == n1)
	{
		cols = malloc( (1+n1) * sizeof(char*) );
		cols[n1] = 0;
	
		for(i=0; i<n1; ++i) 
		{ 
			cols[i] = isList1 ? PyString_AsString( PyList_GetItem( colNames, i ) ) : PyString_AsString( PyTuple_GetItem( colNames, i ) );
		}
		
		//find the smallest row size (in case input is incorrect)
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			
			if (PyList_Check(item) || PyTuple_Check(item))
			{
				isList3 = PyList_Check(item);
				n3 = isList3 ? PyList_Size(item) : PyTuple_Size (item);
			
				if (n3 < rows || rows == 0) rows = n3;
			}
		}
		
		nums = malloc( n1 * rows * sizeof(double) );
		
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
		
		M.cols = n1;
		M.rows = rows;
		
		M.colnames = cols;
		M.rownames = 0;
		M.values = nums;
		
		tc_plot(M,xaxis,title,opt);
	
		TCFreeMatrix(M);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}


static PyObject * pytc_hist(PyObject *self, PyObject *args)
{
	PyObject * colNames, * values, *item;
	int bins = 100;
	char * title = "";
	int isList1,n1=0,isList2,n2=0;
	int n3, isList3, rows;
	char ** cols;
	int i,j;
	double * nums;
	Matrix M;

	if(!PyArg_ParseTuple(args, "OO|is", &colNames, &values, &bins, &title))
        return NULL;
	
	if (PyList_Check(colNames) || PyTuple_Check(colNames))
	{
		isList1 = PyList_Check(colNames);
		n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	}
	
	if (PyList_Check(values) || PyTuple_Check(values))
	{
		isList2 = PyList_Check(values);
		n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	}
	
	rows = 0;
	
	if (n1 > 0 && n2 == n1)
	{
		cols = malloc( (1+n1) * sizeof(char*) );
		cols[n1] = 0;
	
		for(i=0; i<n1; ++i) 
		{ 
			cols[i] = isList1 ? PyString_AsString( PyList_GetItem( colNames, i ) ) : PyString_AsString( PyTuple_GetItem( colNames, i ) );
		}
		
		//find the smallest row size (in case input is incorrect)
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			
			if (PyList_Check(item) || PyTuple_Check(item))
			{
				isList3 = PyList_Check(item);
				n3 = isList3 ? PyList_Size(item) : PyTuple_Size (item);
			
				if (n3 < rows || rows == 0) rows = n3;
			}
		}
		
		nums = malloc( n1 * rows * sizeof(double) );
		
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
		
		M.cols = n1;
		M.rows = rows;
		
		M.colnames = cols;
		M.rownames = 0;
		M.values = nums;
		
		tc_hist(M,bins,title);
	
		TCFreeMatrix(M);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_surface(PyObject *self, PyObject *args)
{
	PyObject * colNames, * values, *item;
	char * title = "", **cols;
	int isList1, n1=0, isList2, n2=0, isList3, n3=0, rows;
	int i,j;
	double * nums;
	Matrix M;
	
	if(!PyArg_ParseTuple(args, "OO|s", &colNames, &values, &title))
        return NULL;
	
	if (PyList_Check(colNames) || PyTuple_Check(colNames))
	{
		isList1 = PyList_Check(colNames);
		n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	}
	
	if (PyList_Check(values) || PyTuple_Check(values))
	{
		isList2 = PyList_Check(values);
		n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	}
	
	rows = 0;
	
	if (n1 == 3 && n2 == 3)
	{
		cols = malloc( (1+n1) * sizeof(char*) );
		cols[n1] = 0;
	
		for(i=0; i<n2; ++i) 
		{ 
			if (i < 3)
				cols[i] = isList1 ? PyString_AsString( PyList_GetItem( colNames, i ) ) : PyString_AsString( PyTuple_GetItem( colNames, i ) );
			else
				cols[i] = "";
		}
		
		//find the smallest row size (in case input is incorrect)
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList3 = PyList_Check(item);
			n3 = isList3 ? PyList_Size(item) : PyTuple_Size (item);
			
			if (n3 < rows || rows == 0) rows = n3;
		}
		
		nums = malloc( n1 * rows * sizeof(double) );
		
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
		
		M.cols = n1;
		M.rows = rows;
		
		M.colnames = cols;
		M.rownames = 0;
		M.values = nums;
		
		tc_surface(M,title);
	
		TCFreeMatrix(M);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_multiplot(PyObject *self, PyObject *args)
{
	int x,y;

	if(!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;

	tc_multiplot(x,y);

	Py_INCREF(Py_None);
    return Py_None;
}


static PyObject * pytc_getPlotData(PyObject *self, PyObject *args)
{
	int i=-1,j;
	Matrix M;
	PyObject *ilist;
	PyObject * item, *rowItem;
	int rows, cols;
	PyObject * nlist, *clist;

	if(!PyArg_ParseTuple(args, "|i", &i))
        return NULL;
	
	M = tc_getPlotData(i);
	
	if (M.rows > 0 && M.cols > 0 && M.colnames && M.values)
	{
		rows = M.rows;
		cols = M.cols;
		
		ilist = PyTuple_New(2);
		
		nlist = PyTuple_New(cols);
		clist = PyTuple_New(M.cols);
		
		for (i=0; i < M.cols && M.colnames && M.colnames[i]; i++)
		{
			item = Py_BuildValue("s",M.colnames[i]);
			PyTuple_SetItem(clist, i, item);
		}
		
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
		
		PyTuple_SetItem(ilist, 0, clist);
		PyTuple_SetItem(ilist, 1, nlist);
		
		TCFreeMatrix(M);
	}
	else
	{
		ilist = PyTuple_New(0);
	}
	
	return Py_BuildValue("O",ilist);
}
