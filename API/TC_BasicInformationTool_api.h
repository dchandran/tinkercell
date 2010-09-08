#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "TCstructs.h"

BEGIN_C_DECLS

/*! 
 \brief get all the parameters
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParameters(tc_items a);
/*! 
 \brief get initial values of the given items. Fixed varianbles are included.
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getInitialValues(tc_items a);
/*! 
 \brief set initial values of the given items. 
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setInitialValues(tc_items items,tc_matrix values);
/*! 
 \brief get all fixed variables
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getFixedVariables(tc_items a);
/*! 
 \brief get all the parameters and fixed variables
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParametersAndFixedVariables(tc_items a);
/*! 
 \brief get the text attribute with the given name for the given item
 \ingroup Network data
*/
TCAPIEXPORT const char* tc_getTextAttribute(long item,const char* attribute);
/*! 
 \brief get the numerical attribute with the given name for the given item
 \ingroup Modeling
*/
TCAPIEXPORT double tc_getParameter(long item,const char* attribute);
/*! 
 \brief get all numerical Modeling with the given names for the given items
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParametersNamed(tc_items a,tc_strings attibutes);
/*! 
 \brief get all numerical Modeling EXCEPT the given names
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParametersExcept(tc_items a,tc_strings Modeling);
/*! 
 \brief get all text Modeling with the given name for the given items
 \ingroup Network data
*/
TCAPIEXPORT tc_strings tc_getAllTextNamed(tc_items a,tc_strings Modeling);
/*! 
 \brief set text attribute for the given item
 \ingroup Network data
*/
TCAPIEXPORT void tc_setTextAttribute(long item,const char* attribute,const char* value);
/*! 
 \brief set numerical attribute for the given item
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setParameter(long item,const char* attribute,double value);
/*! 
 \brief initialize attribute functions
 \ingroup init
*/
TCAPIEXPORT void tc_BasicInformationTool_Text_api(
		const char* (*getTextData)(long ,const char* ),
		tc_strings (*getAllTextDataNamed)(tc_items,tc_strings),
		void (*setTextData)(long ,const char* ,const char* ));

TCAPIEXPORT void tc_BasicInformationTool_Numeric_api(
		tc_matrix (*getInitialValues)(tc_items ),
		void (*setInitialValues)(tc_items,tc_matrix),
		tc_matrix (*getParameters)(tc_items ),
		tc_matrix (*getFixedVariabes)(tc_items),
		tc_matrix (*getParametersAndFixedVariabes)(tc_items ),
		double (*getNumericalData)(long ,const char* ),
		tc_matrix (*getParametersNamed)(tc_items,tc_strings),
		tc_matrix (*getParametersExcept)(tc_items,tc_strings),
		void (*setNumericalData)(long ,const char* ,double )
	);

END_C_DECLS
#endif

