#include "TC_BasicInformationTool_api.h"

TableOfReals (*_tc_getParameters)(ArrayOfItems) = 0;
/*! 
 \brief get all the parameters
 \ingroup Attributes
*/
TableOfReals tc_getParameters(ArrayOfItems a)
{
	if (_tc_getParameters)
		return _tc_getParameters(a);

	return newMatrix(0,0);
}

TableOfReals (*_tc_getInitialValues)(ArrayOfItems) = 0;
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Attributes
*/
TableOfReals tc_getInitialValues(ArrayOfItems a)
{
	if (_tc_getInitialValues)
		return _tc_getInitialValues(a);
		
	return newMatrix(0,0);
}

void (*_tc_setInitialValues)(ArrayOfItems items,TableOfReals values) = 0;
/*! 
 \brief set initial values of the given items. 
 \ingroup Attributes
*/
void tc_setInitialValues(ArrayOfItems items,TableOfReals values)
{
	if (_tc_setInitialValues)
		_tc_setInitialValues(items,values);
}

TableOfReals (*_tc_getFixedVariables)(ArrayOfItems) = 0;
/*! 
 \brief get all fixed variables
 \ingroup Attributes
*/
TableOfReals tc_getFixedVariables(ArrayOfItems a)
{
	if (_tc_getFixedVariables)
		return _tc_getFixedVariables(a);
	return newMatrix(0,0);
}

TableOfReals (*_tc_getParametersAndFixedVariables)(ArrayOfItems) = 0;
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Attributes
*/
TableOfReals tc_getParametersAndFixedVariables(ArrayOfItems a)
{
	if (_tc_getParametersAndFixedVariables)
		return _tc_getParametersAndFixedVariables(a);
	return newMatrix(0,0);
}

String (*_tc_getTextAttribute)(Item item,String attribute) = 0;
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Attributes
*/
String tc_getTextAttribute(Item item,String attribute)
{
	if (_tc_getTextAttribute)
		return _tc_getTextAttribute(item,attribute);
	return 0;
}

double (*_tc_getParameter)(Item item,String attribute) = 0;
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Attributes
*/
double tc_getParameter(Item item,String attribute)
{
	if (_tc_getParameter)
		return _tc_getParameter(item,attribute);
	return 0.0;
}

TableOfReals (*_tc_getParametersNamed)(ArrayOfItems,ArrayOfStrings attibutes) = 0;
/*! 
 \brief get all numerical attributes with the given names for the given items
 \ingroup Attributes
*/
TableOfReals tc_getParametersNamed(ArrayOfItems a,ArrayOfStrings attibutes)
{
	if (_tc_getParametersNamed)
		return _tc_getParametersNamed(a,attibutes);
	return newMatrix(0,0);
}

TableOfReals (*_tc_getParametersExcept)(ArrayOfItems,ArrayOfStrings attributes) = 0;
/*! 
 \brief get all numerical attributes EXCEPT the given names
 \ingroup Attributes
*/
TableOfReals tc_getParametersExcept(ArrayOfItems a,ArrayOfStrings attributes)
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

void (*_tc_setTextAttribute)(Item item,String attribute,String value) = 0;
/*! 
 \brief set text attribute for the given item
 \ingroup Attributes
*/
void tc_setTextAttribute(Item item,String attribute,String value)
{
	if (_tc_setTextAttribute)
		_tc_setTextAttribute(item,attribute,value);
}

void (*_tc_setParameter)(Item item,String attribute,double value) = 0;
/*! 
 \brief set numerical attribute for the given item
 \ingroup Attributes
*/
void tc_setParameter(Item item,String attribute,double value)
{
	if (_tc_setParameter)
		_tc_setParameter(item,attribute,value);
}

/*! 
 \brief initialize attribute functions
 \ingroup init
*/
void tc_BasicInformationTool_Text_api(
		String (*getTextData)(Item ,String ),
		ArrayOfStrings (*getAllTextDataNamed)(ArrayOfItems,ArrayOfStrings),
		void (*setTextData)(Item ,String ,String ))
{
	_tc_getTextAttribute = getTextData;
	_tc_getAllTextNamed = getAllTextDataNamed;
	_tc_setTextAttribute = setTextData;
}

void tc_BasicInformationTool_Numeric_api(
		TableOfReals (*getInitialValues)(ArrayOfItems ),
		void (*setInitialValues)(ArrayOfItems,TableOfReals),
		TableOfReals (*getParameters)(ArrayOfItems ),
		TableOfReals (*getFixedVariabes)(ArrayOfItems),
		TableOfReals (*getParametersAndFixedVariabes)(ArrayOfItems ),
		double (*getNumericalData)(Item ,String ),
		TableOfReals (*getParametersNamed)(ArrayOfItems,ArrayOfStrings),
		TableOfReals (*getParametersExcept)(ArrayOfItems,ArrayOfStrings),
		void (*setNumericalData)(Item ,String ,double )
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

