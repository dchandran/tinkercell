#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "../TCstructs.h"

Matrix (*_tc_getParameters)(Array) = 0;
/*! 
 \brief get all the parameters
 \ingroup Attributes
*/
Matrix tc_getParameters(Array a)
{
	Matrix M;
	if (_tc_getParameters)
		return _tc_getParameters(a);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	return M;
}

Matrix (*_tc_getInitialValues)(Array) = 0;
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Attributes
*/
Matrix tc_getInitialValues(Array a)
{
	Matrix M;
	if (_tc_getInitialValues)
		return _tc_getInitialValues(a);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	return M;
}

void (*_tc_setInitialValues)(Array items,Matrix values) = 0;
/*! 
 \brief set initial values of the given items. 
 \ingroup Attributes
*/
void tc_setInitialValues(Array items,Matrix values)
{
	if (_tc_setInitialValues)
		_tc_setInitialValues(items,values);
}

Matrix (*_tc_getFixedVariables)(Array) = 0;
/*! 
 \brief get all fixed variables
 \ingroup Attributes
*/
Matrix tc_getFixedVariables(Array a)
{
	Matrix M;
	if (_tc_getFixedVariables)
		return _tc_getFixedVariables(a);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	return M;
}

Matrix (*_tc_getParametersAndFixedVariables)(Array) = 0;
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Attributes
*/
Matrix tc_getParametersAndFixedVariables(Array a)
{
	Matrix M;
	if (_tc_getParametersAndFixedVariables)
		return _tc_getParametersAndFixedVariables(a);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	return M;
}

char* (*_tc_getTextAttribute)(OBJ item,const char* attribute) = 0;
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Attributes
*/
char* tc_getTextAttribute(OBJ item,const char* attribute)
{
	if (_tc_getTextAttribute)
		return _tc_getTextAttribute(item,attribute);
	return 0;
}

double (*_tc_getParameter)(OBJ item,const char* attribute) = 0;
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Attributes
*/
double tc_getParameter(OBJ item,const char* attribute)
{
	if (_tc_getParameter)
		return _tc_getParameter(item,attribute);
	return 0.0;
}

Matrix (*_tc_getParametersNamed)(Array,ArrayOfStrings attibutes) = 0;
/*! 
 \brief get all numerical attributes with the given names for the given items
 \ingroup Attributes
*/
Matrix tc_getParametersNamed(Array a,ArrayOfStrings attibutes)
{
	Matrix M;
	if (_tc_getParametersNamed)
		return _tc_getParametersNamed(a,attibutes);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	return M;
}

Matrix (*_tc_getParametersExcept)(Array,ArrayOfStrings attributes) = 0;
/*! 
 \brief get all numerical attributes EXCEPT the given names
 \ingroup Attributes
*/
Matrix tc_getParametersExcept(Array a,ArrayOfStrings attributes)
{
	Matrix M;
	if (_tc_getParametersExcept)
		return _tc_getParametersExcept(a,attributes);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	return M;
}

ArrayOfStrings (*_tc_getAllTextNamed)(Array,ArrayOfStrings attributes) = 0;
/*! 
 \brief get all text attributes with the given name for the given items
 \ingroup Attributes
*/
ArrayOfStrings tc_getAllTextNamed(Array a,ArrayOfStrings attributes)
{
	if (_tc_getAllTextNamed)
		return _tc_getAllTextNamed(a,attributes);
	return 0;
}

void (*_tc_setTextAttribute)(OBJ item,const char* attribute,const char* value) = 0;
/*! 
 \brief set text attribute for the given item
 \ingroup Attributes
*/
void tc_setTextAttribute(OBJ item,const char* attribute,const char* value)
{
	if (_tc_setTextAttribute)
		_tc_setTextAttribute(item,attribute,value);
}

void (*_tc_setParameter)(OBJ item,const char* attribute,double value) = 0;
/*! 
 \brief set numerical attribute for the given item
 \ingroup Attributes
*/
void tc_setParameter(OBJ item,const char* attribute,double value)
{
	if (_tc_setParameter)
		_tc_setParameter(item,attribute,value);
}

/*! 
 \brief initialize attribute functions
 \ingroup init
*/
void tc_BasicInformationTool_Text_api(
		char* (*getTextData)(OBJ ,const char* ),
		ArrayOfStrings (*getAllTextDataNamed)(Array,ArrayOfStrings),
		void (*setTextData)(OBJ ,const char* ,const char* ))
{
	_tc_getTextAttribute = getTextData;
	_tc_getAllTextNamed = getAllTextDataNamed;
	_tc_setTextAttribute = setTextData;
}

void tc_BasicInformationTool_Numeric_api(
		Matrix (*getInitialValues)(Array ),
		void (*setInitialValues)(Array,Matrix),
		Matrix (*getParameters)(Array ),
		Matrix (*getFixedVariabes)(Array),
		Matrix (*getParametersAndFixedVariabes)(Array ),
		double (*getNumericalData)(OBJ ,const char* ),
		Matrix (*getParametersNamed)(Array,ArrayOfStrings),
		Matrix (*getParametersExcept)(Array,ArrayOfStrings),
		void (*setNumericalData)(OBJ ,const char* ,double )
	)
{
	_tc_getInitialValues = getInitialValues;
	_tc_setInitialValues = setInitialValues;
	
	_tc_getParameters = getParameters;
	
	_tc_getFixedVariables = getFixedVariabes;
	_tc_getParametersAndFixedVariables = getParametersAndFixedVariabes;
	
	_tc_getParameter = getNumericalData;
	_tc_getParametersNamed = getParametersNamed;
	_tc_getParametersExcept = getParametersExcept;
	_tc_setParameter = setNumericalData;
}

#endif
