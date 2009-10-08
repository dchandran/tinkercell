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
		
		if (tc_plot)
			tc_plot(M,xaxis,title,opt);
	
		TCFreeMatrix(M);
	}
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_surface(PyObject *self, PyObject *args)
{
	PyObject * colNames, * values, *item;
	char * title = "", **cols;
	double x0,x1,y0,y1;
	int isList1, n1=0, isList2, n2=0, isList3, n3=0, rows;
	int i,j;
	double * nums;
	Matrix M;
	
	if(!PyArg_ParseTuple(args, "OOdddd|s", &colNames, &values, &x0, &x1, &y0, &y1, &title))
        return NULL;
	
	if (PyList_Check(colNames) || PyTuple_Check(colNames))
	{
		isList1 = PyList_Check(colNames);
		n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	}
	
	if (PyList_Check(colNames) || PyTuple_Check(colNames))
	{
		isList2 = PyList_Check(values);
		n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	}
	
	rows = 0;
	
	if (n1 == 3 && n2 > 0)
	{
		n1 = n2;
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
		
		if (tc_surface)
			tc_surface(M,x0,x1,y0,y1,title);
	
		TCFreeMatrix(M);
	}
	
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

	if(!PyArg_ParseTuple(args, "|i", &i) || (tc_plotData == 0))
        return NULL;
	
	M = tc_plotData(i);
	
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

static PyObject * pytc_getJacobian(PyObject *self, PyObject *args)
{
	int i=-1;
	PyObject * pylist = 0;
	void ** A = 0;
	int isList, N=0, k;
	FILE * out;
	char * appDir, * cmd;
	int sz;

	if(!PyArg_ParseTuple(args, "|O", &pylist) || (tc_allItems == 0) || (tc_allItems == 0))
        return NULL;
	
	if (pylist)
	{
		if (PyList_Check(pylist) || PyTuple_Check(pylist))
		{
			isList = PyList_Check(pylist);
			N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
		}
	
		if (N > 0)
		{
			A = malloc( (1+N) * sizeof(void*) );
			A[N] = 0;
			
			for(i=0; i<N; ++i ) 
			{ 
				A[i] = isList ? (void*)((int)PyInt_AsLong( PyList_GetItem( pylist, i ) )) : (void*)((int)PyInt_AsLong( PyTuple_GetItem( pylist, i ) ));
			}
		}
	}
	else
	{
		A = tc_allItems();
	}
	
	if (A==0 || A[0] == 0)
	{
		tc_errorReport("No model");
		if (A) free(A);
		return PyTuple_New(0);
	}
	
	k = tc_writeModel( "ode", A );
    TCFreeArray(A);
	if (!k)
	{
		tc_errorReport("No Model\0");
		return PyTuple_New(0);
	}
	
	out = fopen("ode.c","a");
	
	fprintf( out , "#include \"TC_api.h\"\n#include \"cvodesim.h\"\n\n\
   \n\
int run(Matrix input) \n\
{\n\
   TCinitialize();\n\
   double * y = steadyState(TCvars, TCinit, &(TCodeFunc), 0, %lf, %lf, %lf);\n\
   if (!y) \
   {\n\
      tc_errorReport(\"CVode failed! Possible cause of failure: some values are reaching infinity. Double check your model.\");\n\
      return 0;\n\
   }\n\
   double * J = jacobian(TCvars,y,&(TCodeFunc),1);\n\
   Matrix data;\n\
   data.rows = data.cols = TCvars;\n\
   data.values = J;\n\
   data.rownames = data.colnames = TCcolnames;\n\
   tc_plot(data,-1,\"Jacobian at the nearest stable point\",0);\n\
   free(y); free(J);\n\
   return 1;\n}\n", 1.0E-5, 1000000.0, 2.0);
   fclose(out);

   appDir = tc_appDir();

   sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   cmd = malloc((sz*3 + 50) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"odesim.o ode.c\0");
   }
   else
   {
       sprintf(cmd,"ode.c -lodesim\0",appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0","Jacobian\0");
   
   free(cmd);
   free(appDir);

   Py_INCREF(Py_None);
   return Py_None;
}

