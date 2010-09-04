#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "../TCstructs.h"

BEGIN_C_DECLS

/*! 
 \brief get all the parameters
 \ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getParameters(ArrayOfItems a);
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getInitialValues(ArrayOfItems a);
/*! 
 \brief set initial values of the given items. 
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setInitialValues(ArrayOfItems items,TableOfReals values);
/*! 
 \brief get all fixed variables
 \ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getFixedVariables(ArrayOfItems a);
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getParametersAndFixedVariables(ArrayOfItems a);
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Network data
*/
TCAPIEXPORT const char* tc_getTextAttribute(int item,const char* attribute);
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Modeling
*/
TCAPIEXPORT double tc_getParameter(int item,const char* attribute);
/*! 
 \brief get all numerical Modeling with the given names for the given items
 \ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getParametersNamed(ArrayOfItems a,ArrayOfStrings attibutes);
/*! 
 \brief get all numerical Modeling EXCEPT the given names
 \ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getParametersExcept(ArrayOfItems a,ArrayOfStrings Modeling);
/*! 
 \brief get all text Modeling with the given name for the given items
 \ingroup Network data
*/
TCAPIEXPORT ArrayOfStrings tc_getAllTextNamed(ArrayOfItems a,ArrayOfStrings Modeling);
/*! 
 \brief set text attribute for the given item
 \ingroup Network data
*/
TCAPIEXPORT void tc_setTextAttribute(int item,const char* attribute,const char* value);
/*! 
 \brief set numerical attribute for the given item
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setParameter(int item,const char* attribute,double value);
/*! 
 \brief initialize attribute functions
 \ingroup init
*/
TCAPIEXPORT void tc_BasicInformationTool_Text_api(
		const char* (*getTextData)(int ,const char* ),
		ArrayOfStrings (*getAllTextDataNamed)(ArrayOfItems,ArrayOfStrings),
		void (*setTextData)(int ,const char* ,const char* ));

TCAPIEXPORT void tc_BasicInformationTool_Numeric_api(
		TableOfReals (*getInitialValues)(ArrayOfItems ),
		void (*setInitialValues)(ArrayOfItems,TableOfReals),
		TableOfReals (*getParameters)(ArrayOfItems ),
		TableOfReals (*getFixedVariabes)(ArrayOfItems),
		TableOfReals (*getParametersAndFixedVariabes)(ArrayOfItems ),
		double (*getNumericalData)(int ,const char* ),
		TableOfReals (*getParametersNamed)(ArrayOfItems,ArrayOfStrings),
		TableOfReals (*getParametersExcept)(ArrayOfItems,ArrayOfStrings),
		void (*setNumericalData)(int ,const char* ,double )
	);

END_C_DECLS
#endif

