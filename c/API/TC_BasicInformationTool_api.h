#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief get all the parameters
 \ingroup Attributes
*/
Matrix tc_getParameters(ArrayOfItems a);
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Attributes
*/
Matrix tc_getInitialValues(ArrayOfItems a);
/*! 
 \brief set initial values of the given items. 
 \ingroup Attributes
*/
void tc_setInitialValues(ArrayOfItems items,Matrix values);
/*! 
 \brief get all fixed variables
 \ingroup Attributes
*/
Matrix tc_getFixedVariables(ArrayOfItems a);
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Attributes
*/
Matrix tc_getParametersAndFixedVariables(ArrayOfItems a);
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Attributes
*/
const char* tc_getTextAttribute(void* item,const char* attribute);
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Attributes
*/
double tc_getParameter(void* item,const char* attribute);
/*! 
 \brief get all numerical attributes with the given names for the given items
 \ingroup Attributes
*/
Matrix tc_getParametersNamed(ArrayOfItems a,ArrayOfStrings attibutes);
/*! 
 \brief get all numerical attributes EXCEPT the given names
 \ingroup Attributes
*/
Matrix tc_getParametersExcept(ArrayOfItems a,ArrayOfStrings attributes);
/*! 
 \brief get all text attributes with the given name for the given items
 \ingroup Attributes
*/
ArrayOfStrings tc_getAllTextNamed(ArrayOfItems a,ArrayOfStrings attributes);
/*! 
 \brief set text attribute for the given item
 \ingroup Attributes
*/
void tc_setTextAttribute(void* item,const char* attribute,const char* value);
/*! 
 \brief set numerical attribute for the given item
 \ingroup Attributes
*/
void tc_setParameter(void* item,const char* attribute,double value);
/*! 
 \brief initialize attribute functions
 \ingroup init
*/
void tc_BasicInformationTool_Text_api(
		const char* (*getTextData)(void* ,const char* ),
		ArrayOfStrings (*getAllTextDataNamed)(ArrayOfItems,ArrayOfStrings),
		void (*setTextData)(void* ,const char* ,const char* ));

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
	);
#endif
