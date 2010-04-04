#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief get all the parameters
 \ingroup Modeling
*/
TCAPIEXPORT Matrix tc_getParameters(ArrayOfItems a);
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Modeling
*/
TCAPIEXPORT Matrix tc_getInitialValues(ArrayOfItems a);
/*! 
 \brief set initial values of the given items. 
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setInitialValues(ArrayOfItems items,Matrix values);
/*! 
 \brief get all fixed variables
 \ingroup Modeling
*/
TCAPIEXPORT Matrix tc_getFixedVariables(ArrayOfItems a);
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Modeling
*/
TCAPIEXPORT Matrix tc_getParametersAndFixedVariables(ArrayOfItems a);
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Network data
*/
TCAPIEXPORT String tc_getTextAttribute(Item item,String attribute);
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Modeling
*/
TCAPIEXPORT double tc_getParameter(Item item,String attribute);
/*! 
 \brief get all numerical Modeling with the given names for the given items
 \ingroup Modeling
*/
TCAPIEXPORT Matrix tc_getParametersNamed(ArrayOfItems a,ArrayOfStrings attibutes);
/*! 
 \brief get all numerical Modeling EXCEPT the given names
 \ingroup Modeling
*/
TCAPIEXPORT Matrix tc_getParametersExcept(ArrayOfItems a,ArrayOfStrings Modeling);
/*! 
 \brief get all text Modeling with the given name for the given items
 \ingroup Network data
*/
TCAPIEXPORT ArrayOfStrings tc_getAllTextNamed(ArrayOfItems a,ArrayOfStrings Modeling);
/*! 
 \brief set text attribute for the given item
 \ingroup Network data
*/
TCAPIEXPORT void tc_setTextAttribute(Item item,String attribute,String value);
/*! 
 \brief set numerical attribute for the given item
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setParameter(Item item,String attribute,double value);
/*! 
 \brief initialize attribute functions
 \ingroup init
*/
TCAPIEXPORT void tc_BasicInformationTool_Text_api(
		String (*getTextData)(Item ,String ),
		ArrayOfStrings (*getAllTextDataNamed)(ArrayOfItems,ArrayOfStrings),
		void (*setTextData)(Item ,String ,String ));

TCAPIEXPORT void tc_BasicInformationTool_Numeric_api(
		Matrix (*getInitialValues)(ArrayOfItems ),
		void (*setInitialValues)(ArrayOfItems,Matrix),
		Matrix (*getParameters)(ArrayOfItems ),
		Matrix (*getFixedVariabes)(ArrayOfItems),
		Matrix (*getParametersAndFixedVariabes)(ArrayOfItems ),
		double (*getNumericalData)(Item ,String ),
		Matrix (*getParametersNamed)(ArrayOfItems,ArrayOfStrings),
		Matrix (*getParametersExcept)(ArrayOfItems,ArrayOfStrings),
		void (*setNumericalData)(Item ,String ,double )
	);
#endif
