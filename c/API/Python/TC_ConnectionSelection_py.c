#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_getControlPointX(PyObject *self, PyObject *args)
{
	int i,j,k;
	double d;

	if(!PyArg_ParseTuple(args, "iii", &i, &j, &k))
		return NULL;

	d = tc_getControlPointX((void*)i,(void*)j,k);

	return Py_BuildValue("d",d);
}

static PyObject * pytc_getControlPointY(PyObject *self, PyObject *args)
{
	int i,j,k;
	double d;

	if(!PyArg_ParseTuple(args, "iii", &i, &j, &k))
		return NULL;

	d = tc_getControlPointY((void*)i,(void*)j,k);

	return Py_BuildValue("d",d);
}

static PyObject * pytc_setControlPoint(PyObject *self, PyObject *args)
{
	int i,j,k;
	double x,y;

	if(!PyArg_ParseTuple(args, "iiidd", &i, &j, &k, &x, &y))
		return NULL;

	tc_setControlPoint((void*)i,(void*)j,k,x,y);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_setCenterPoint(PyObject *self, PyObject *args)
{
	int i;
	double x,y;

	if(!PyArg_ParseTuple(args, "idd", &i, &x, &y))
		return NULL;

	tc_setCenterPoint((void*)i,x,y);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_getCenterPointX(PyObject *self, PyObject *args)
{
	int i;
	double d;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	d = tc_getCenterPointX((void*)i);

	return Py_BuildValue("d",d);
}

static PyObject * pytc_getCenterPointY(PyObject *self, PyObject *args)
{
	int i;
	double d;

	if(!PyArg_ParseTuple(args, "i", &i))
		return NULL;

	d = tc_getCenterPointY((void*)i);

	return Py_BuildValue("d",d);
}

static PyObject * pytc_setStraight(PyObject *self, PyObject *args)
{
	int item, v = 1;
	
	if(!PyArg_ParseTuple(args, "i|i", &item, &v))
		return NULL;

	tc_setStraight((void*)item,v);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_setAllStraight(PyObject *self, PyObject *args)
{
	int v = 1;
	
	if(!PyArg_ParseTuple(args, "|i", &v))
		return NULL;

	tc_setAllStraight(v);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject * pytc_setLineWidth(PyObject *self, PyObject *args)
{
	int item,i = 0;
	double w;
	
	if(!PyArg_ParseTuple(args, "id|i", &item, &w, &i))
		return NULL;

	tc_setLineWidth((void*)item,w,i);

	Py_INCREF(Py_None);
	return Py_None;
}

