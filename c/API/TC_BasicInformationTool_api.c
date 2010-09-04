#include "TC_BasicInformationTool_api.h"

tc_matrix (*_tc_getParameters)(tc_items) = 0;
/*! 
 \brief get all the parameters
 \ingroup Attributes
*/
tc_matrix tc_getParameters(tc_items a)
{
	if (_tc_getParameters)
		return _tc_getParameters(a);

	return tc_createMatrix(0,0);
}

tc_matrix (*_tc_getInitialValues)(tc_items) = 0;
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Attributes
*/
tc_matrix tc_getInitialValues(tc_items a)
{
	if (_tc_getInitialValues)
		return _tc_getInitialValues(a);
		
	return tc_createMatrix(0,0);
}

void (*_tc_setInitialValues)(tc_items items,tc_matrix values) = 0;
/*! 
 \brief set initial values of the given items. 
 \ingroup Attributes
*/
void tc_setInitialValues(tc_items items,tc_matrix values)
{
	if (_tc_setInitialValues)
		_tc_setInitialValues(items,values);
}

tc_matrix (*_tc_getFixedVariables)(tc_items) = 0;
/*! 
 \brief get all fixed variables
 \ingroup Attributes
*/
tc_matrix tc_getFixedVariables(tc_items a)
{
	if (_tc_getFixedVariables)
		return _tc_getFixedVariables(a);
	return tc_createMatrix(0,0);
}

tc_matrix (*_tc_getParametersAndFixedVariables)(tc_items) = 0;
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Attributes
*/
tc_matrix tc_getParametersAndFixedVariables(tc_items a)
{
	if (_tc_getParametersAndFixedVariables)
		return _tc_getParametersAndFixedVariables(a);
	return tc_createMatrix(0,0);
}

const char* (*_tc_getTextAttribute)(long item,const char* attribute) = 0;
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Attributes
*/
const char* tc_getTextAttribute(long item,const char* attribute)
{
	if (_tc_getTextAttribute)
		return _tc_getTextAttribute(item,attribute);
	return 0;
}

double (*_tc_getParameter)(long item,const char* attribute) = 0;
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Attributes
*/
double tc_getParameter(long item,const char* attribute)
{
	if (_tc_getParameter)
		return _tc_getParameter(item,attribute);
	return 0.0;
}

tc_matrix (*_tc_getParametersNamed)(tc_items,tc_strings attibutes) = 0;
/*! 
 \brief get all numerical attributes with the given names for the given items
 \ingroup Attributes
*/
tc_matrix tc_getParametersNamed(tc_items a,tc_strings attibutes)
{
	if (_tc_getParametersNamed)
		return _tc_getParametersNamed(a,attibutes);
	return tc_createMatrix(0,0);
}

tc_matrix (*_tc_getParametersExcept)(tc_items,tc_strings attributes) = 0;
/*! 
 \brief get all numerical attributes EXCEPT the given names
 \ingroup Attributes
*/
tc_matrix tc_getParametersExcept(tc_items a,tc_strings attributes)
{
	if (_tc_getParametersExcept)
		return _tc_getParametersExcept(a,attributes);
	return tc_createMatrix(0,0);
}

tc_strings (*_tc_getAllTextNamed)(tc_items,tc_strings attributes) = 0;
/*! 
 \brief get all text attributes with the given name for the given items
 \ingroup Attributes
*/
tc_strings tc_getAllTextNamed(tc_items a,tc_strings attributes)
{
	if (_tc_getAllTextNamed)
		return _tc_getAllTextNamed(a,attributes);
	return tc_createStringsArray(0);
}

void (*_tc_setTextAttribute)(long item,const char* attribute,const char* value) = 0;
/*! 
 \brief set text attribute for the given item
 \ingroup Attributes
*/
void tc_setTextAttribute(long item,const char* attribute,const char* value)
{
	if (_tc_setTextAttribute)
		_tc_setTextAttribute(item,attribute,value);
}

void (*_tc_setParameter)(long item,const char* attribute,double value) = 0;
/*! 
 \brief set numerical attribute for the given item
 \ingroup Attributes
*/
void tc_setParameter(long item,const char* attribute,double value)
{
	if (_tc_setParameter)
		_tc_setParameter(item,attribute,value);
}

/*! 
 \brief initialize attribute functions
 \ingroup init
*/
void tc_BasicInformationTool_Text_api(
		const char* (*getTextData)(long ,const char* ),
		tc_strings (*getAllTextDataNamed)(tc_items,tc_strings),
		void (*setTextData)(long ,const char* ,const char* ))
{
	_tc_getTextAttribute = getTextData;
	_tc_getAllTextNamed = getAllTextDataNamed;
	_tc_setTextAttribute = setTextData;
}

void tc_BasicInformationTool_Numeric_api(
		tc_matrix (*getInitialValues)(tc_items ),
		void (*setInitialValues)(tc_items,tc_matrix),
		tc_matrix (*getParameters)(tc_items ),
		tc_matrix (*getFixedVariabes)(tc_items),
		tc_matrix (*getParametersAndFixedVariabes)(tc_items ),
		double (*getNumericalData)(long ,const char* ),
		tc_matrix (*getParametersNamed)(tc_items,tc_strings),
		tc_matrix (*getParametersExcept)(tc_items,tc_strings),
		void (*setNumericalData)(long ,const char* ,double )
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

