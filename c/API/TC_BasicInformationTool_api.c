#include "TC_BasicInformationTool_api.h"

Matrix (*_tc_getParameters)(ArrayOfItems) = 0;
/*! 
 \brief get all the parameters
 \ingroup Attributes
*/
Matrix tc_getParameters(ArrayOfItems a)
{
	if (_tc_getParameters)
		return _tc_getParameters(a);

	return newMatrix(0,0);
}

Matrix (*_tc_getInitialValues)(ArrayOfItems) = 0;
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Attributes
*/
Matrix tc_getInitialValues(ArrayOfItems a)
{
	if (_tc_getInitialValues)
		return _tc_getInitialValues(a);
		
	return newMatrix(0,0);
}

void (*_tc_setInitialValues)(ArrayOfItems items,Matrix values) = 0;
/*! 
 \brief set initial values of the given items. 
 \ingroup Attributes
*/
void tc_setInitialValues(ArrayOfItems items,Matrix values)
{
	if (_tc_setInitialValues)
		_tc_setInitialValues(items,values);
}

Matrix (*_tc_getFixedVariables)(ArrayOfItems) = 0;
/*! 
 \brief get all fixed variables
 \ingroup Attributes
*/
Matrix tc_getFixedVariables(ArrayOfItems a)
{
	if (_tc_getFixedVariables)
		return _tc_getFixedVariables(a);
	return newMatrix(0,0);
}

Matrix (*_tc_getParametersAndFixedVariables)(ArrayOfItems) = 0;
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Attributes
*/
Matrix tc_getParametersAndFixedVariables(ArrayOfItems a)
{
	if (_tc_getParametersAndFixedVariables)
		return _tc_getParametersAndFixedVariables(a);
	return newMatrix(0,0);
}

const char* (*_tc_getTextAttribute)(void* item,const char* attribute) = 0;
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Attributes
*/
const char* tc_getTextAttribute(void* item,const char* attribute)
{
	if (_tc_getTextAttribute)
		return _tc_getTextAttribute(item,attribute);
	return 0;
}

double (*_tc_getParameter)(void* item,const char* attribute) = 0;
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Attributes
*/
double tc_getParameter(void* item,const char* attribute)
{
	if (_tc_getParameter)
		return _tc_getParameter(item,attribute);
	return 0.0;
}

Matrix (*_tc_getParametersNamed)(ArrayOfItems,ArrayOfStrings attibutes) = 0;
/*! 
 \brief get all numerical attributes with the given names for the given items
 \ingroup Attributes
*/
Matrix tc_getParametersNamed(ArrayOfItems a,ArrayOfStrings attibutes)
{
	if (_tc_getParametersNamed)
		return _tc_getParametersNamed(a,attibutes);
	return newMatrix(0,0);
}

Matrix (*_tc_getParametersExcept)(ArrayOfItems,ArrayOfStrings attributes) = 0;
/*! 
 \brief get all numerical attributes EXCEPT the given names
 \ingroup Attributes
*/
Matrix tc_getParametersExcept(ArrayOfItems a,ArrayOfStrings attributes)
{
	if (_tc_getParametersExcept)
		return _tc_getParametersExcept(a,attributes);
	return newMatrix(0,0);
}

ArrayOfStrings (*_tc_getAllTextNamed)(ArrayOfItems,ArrayOfStrings attributes) = 0;
/*! 
 \brief get all text attributes with the given name for the given items
 \ingroup Attributes
*/
ArrayOfStrings tc_getAllTextNamed(ArrayOfItems a,ArrayOfStrings attributes)
{
	if (_tc_getAllTextNamed)
		return _tc_getAllTextNamed(a,attributes);
	return newArrayOfStrings(0);
}

void (*_tc_setTextAttribute)(void* item,const char* attribute,const char* value) = 0;
/*! 
 \brief set text attribute for the given item
 \ingroup Attributes
*/
void tc_setTextAttribute(void* item,const char* attribute,const char* value)
{
	if (_tc_setTextAttribute)
		_tc_setTextAttribute(item,attribute,value);
}

void (*_tc_setParameter)(void* item,const char* attribute,double value) = 0;
/*! 
 \brief set numerical attribute for the given item
 \ingroup Attributes
*/
void tc_setParameter(void* item,const char* attribute,double value)
{
	if (_tc_setParameter)
		_tc_setParameter(item,attribute,value);
}

/*! 
 \brief initialize attribute functions
 \ingroup init
*/
void tc_BasicInformationTool_Text_api(
		const char* (*getTextData)(void* ,const char* ),
		ArrayOfStrings (*getAllTextDataNamed)(ArrayOfItems,ArrayOfStrings),
		void (*setTextData)(void* ,const char* ,const char* ))
{
	_tc_getTextAttribute = getTextData;
	_tc_getAllTextNamed = getAllTextDataNamed;
	_tc_setTextAttribute = setTextData;
}

void tc_BasicInformationTool_Numeric_api(
		Matrix (*getInitialValues)(ArrayOfItems ),
		void (*setInitialValues)(ArrayOfItems,Matrix),
		Matrix (*getParameters)(ArrayOfItems ),
		Matrix (*getFixedVariabes)(ArrayOfItems),
		Matrix (*getParametersAndFixedVariabes)(ArrayOfItems ),
		double (*getNumericalData)(void* ,const char* ),
		Matrix (*getParametersNamed)(ArrayOfItems,ArrayOfStrings),
		Matrix (*getParametersExcept)(ArrayOfItems,ArrayOfStrings),
		void (*setNumericalData)(void* ,const char* ,double )
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

