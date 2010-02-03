#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include "../../TC_api.h"

static PyObject * pytc_getColorR(PyObject *self, PyObject *args)
{
	int i,j;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
	
	j = tc_getColorR((void*)i);
	
	return Py_BuildValue("j",j);
}

static PyObject * pytc_getColorG(PyObject *self, PyObject *args)
{
	int i,j;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
		
	j = tc_getColorG((void*)i);
	
	return Py_BuildValue("j",j);
}

static PyObject * pytc_getColorB(PyObject *self, PyObject *args)
{
	int i,j;
	if(!PyArg_ParseTuple(args, "i", &i))
        return NULL;
	
	j = tc_getColorB((void*)i);
	
	return Py_BuildValue("j",j);
}

static PyObject * pytc_setColor(PyObject *self, PyObject *args)
{
	int item,r,g,b,perm=0;
	if(!PyArg_ParseTuple(args, "iiii|i", &item,&r,&g,&b,&perm))
        return NULL;
	
	tc_setColor((void*)item,r,g,b,perm);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_changeNodeImage(PyObject *self, PyObject *args)
{
	int i=0;
	char * s;
	if(!PyArg_ParseTuple(args, "is", &i, &s))
        return NULL;
	
	tc_changeNodeImage((void*)i,s);
	
	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * pytc_changeArrowHead(PyObject *self, PyObject *args)
{
	int i=0;
	char * s;
	if(!PyArg_ParseTuple(args, "is", &i, &s))
        return NULL;
	
	tc_changeArrowHead((void*)i,s);
	
	Py_INCREF(Py_None);
    return Py_None;
}
