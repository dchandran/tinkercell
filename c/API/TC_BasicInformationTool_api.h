#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief get all the parameters
 \ingroup Attributes
*/
Matrix (*tc_getAllParameters)(Array);
/*! 
 \brief get initial values of the given items. Fixed varianbles will not be included
 \ingroup Attributes
*/
Matrix (*tc_getInitialValues)(Array);
/*! 
 \brief get all fixed variables
 \ingroup Attributes
*/
Matrix (*tc_getFixedVariables)(Array);
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Attributes
*/
Matrix (*tc_getParametersAndFixedVariables)(Array);
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Attributes
*/
char* (*tc_getTextAttribute)(OBJ item,const char* attribute);
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Attributes
*/
double (*tc_getParameter)(OBJ item,const char* attribute);
/*! 
 \brief get all numerical attributes with the given names for the given items
 \ingroup Attributes
*/
Matrix (*tc_getParametersNamed)(Array,char** attibutes);
/*! 
 \brief get all numerical attributes EXCEPT the given names
 \ingroup Attributes
*/
Matrix (*tc_getParametersExcept)(Array,char** attributes);
/*! 
 \brief get all text attributes with the given name for the given items
 \ingroup Attributes
*/
char** (*tc_getAllTextNamed)(Array,char** attributes);
/*! 
 \brief set text attribute for the given item
 \ingroup Attributes
*/
void (*tc_setTextAttribute)(OBJ item,const char* attribute,const char* value);
/*! 
 \brief set numerical attribute for the given item
 \ingroup Attributes
*/
void (*tc_setParameter)(OBJ item,const char* attribute,double value);
/*! 
 \brief initialize attribute functions
 \ingroup init
*/
void tc_BasicInformationTool_Text_api(
		char* (*getTextData)(OBJ ,const char* ),
		char** (*getAllTextDataNamed)(Array,char**),
		void (*setTextData)(OBJ ,const char* ,const char* ))
{
	tc_getTextAttribute = getTextData;
	tc_getAllTextNamed = getAllTextDataNamed;
	tc_setTextAttribute = setTextData;
}

void tc_BasicInformationTool_Numeric_api(
		Matrix (*getInitialValues)(Array ),
		Matrix (*getParameters)(Array ),
		Matrix (*getFixedVariabes)(Array),
		Matrix (*getParametersAndFixedVariabes)(Array ),
		double (*getNumericalData)(OBJ ,const char* ),
		Matrix (*getParametersNamed)(Array,char**),
		Matrix (*getParametersExcept)(Array,char**),
		void (*setNumericalData)(OBJ ,const char* ,double )
	)
{
	tc_getInitialValues = getInitialValues;
	tc_getAllParameters = getParameters;
	
	tc_getFixedVariables = getFixedVariabes;
	tc_getParametersAndFixedVariables = getParametersAndFixedVariabes;
	
	tc_getParameter = getNumericalData;
	tc_getParametersNamed = getParametersNamed;
	tc_getParametersExcept = getParametersExcept;
	tc_setParameter = setNumericalData;
}

#endif
