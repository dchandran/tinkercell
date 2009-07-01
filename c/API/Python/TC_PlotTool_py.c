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
	if(!PyArg_ParseTuple(args, "OO|isi", &colNames, &values, &xaxis, &title, &opt))
        	return NULL;
	
	int isList1 = PyList_Check(colNames);
	int n1 = isList1 ? PyList_Size(colNames) : PyTuple_Size (colNames);
	
	int isList2 = PyList_Check(values);
	int n2 = isList2 ? PyList_Size(values) : PyTuple_Size (values);
	
	int n3, isList3, rows;
	
	rows = 0;
	
	if (n1 > 0 && n2 == n1)
	{
		char ** cols = malloc( (1+n1) * sizeof(char*) );
		cols[n1] = 0;
	
		int i,j;
		for(i=0; i<n1; ++i) 
		{ 
			cols[i] = isList1 ? PyString_AsString( PyList_GetItem( colNames, i ) ) : PyString_AsString( PyTuple_GetItem( colNames, i ) );
		}
		
		//find the smallest row size (in case input is incorrect)
		for(i=0; i<n2; ++i) 
		{ 
			item = isList2 ? (PyList_GetItem( values, i ) ) : ( PyTuple_GetItem( values, i ) );
			isList3 = PyList_Check(item);
			n3 = isList3 ? PyList_Size(item) : PyTuple_Size (item);
			
			if (n3 < rows || rows == 0) rows = n3;
		}
		
		double * nums = malloc( n1 * rows * sizeof(double) );
		
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

static PyObject * pytc_getPlotData(PyObject *self, PyObject *args)
{
	int i=-1,j;
	
	if(!PyArg_ParseTuple(args, "|i", &i))
        	return NULL;
	
	Matrix M;
	
	M = tc_plotData(i);
	
	PyObject *ilist;
	PyObject * item, *rowItem;
	
	if (M.rows > 0 && M.cols > 0 && M.colnames && M.values)
	{
		int rows = M.rows, cols = M.cols;
		
		ilist = PyTuple_New(2);
		
		PyObject * nlist = PyTuple_New(cols);
		PyObject * clist = PyTuple_New(M.cols);
		//PyObject * rlist = PyTuple_New(M.rows);
		
		for (i=0; i < M.cols && M.colnames && M.colnames[i]; i++)
		{
			item = Py_BuildValue("s",M.colnames[i]);
			PyTuple_SetItem(clist, i, item);
		}
		
		/*for (i=0; i < M.rows && M.rownames && M.rownames[i]; i++)
		{
			item = Py_BuildValue("s",M.rownames[i]);
			PyTuple_SetItem(rlist, i, item);
		}*/
		
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
		
		//PyTuple_SetItem(ilist, 0, rlist);
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
	int i=-1,j;
	PyObject * pylist = 0;
	
	if(!PyArg_ParseTuple(args, "|O", &pylist))
        return NULL;
	
	void ** A = 0;
	
	if (pylist)
	{
		int isList = PyList_Check(pylist);
		int N = isList ? PyList_Size(pylist) : PyTuple_Size (pylist);
	
		if (N > 0)
		{
			A = malloc( (1+N) * sizeof(void*) );
			A[N] = 0;
			
			int i;
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
	
	int k = tc_writeModel( "ode", A );
    TCFreeArray(A);
	if (!k)
	{
		tc_errorReport("No Model\0");
		return PyTuple_New(0);
	}
	
	FILE * out = fopen("ode.c","a");
	
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
   
   
   char* appDir = tc_appDir();

   int sz = 0;
   while (appDir[sz] != 0) ++sz;
   
   char* cmd = malloc((sz*3 + 50) * sizeof(char));

   if (tc_isWindows())
   {
       sprintf(cmd,"\"%s\"/c/odesim.o ode.c -I\"%s\"/include -I\"%s\"/c\0",appDir,appDir,appDir);
   }
   else
   {
       sprintf(cmd,"ode.c -I%s/c -L%s/lib -lodesim\0",appDir,appDir);
   }
   tc_compileBuildLoad(cmd,"run\0");
   
   free(cmd);

   Py_INCREF(Py_None);
   return Py_None;
}

