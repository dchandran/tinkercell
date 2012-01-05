#ifndef TINKERCELL_TC_BASICINFORMATIONTOOL_API_H
#define TINKERCELL_TC_BASICINFORMATIONTOOL_API_H

#include "TC_structs.h"

BEGIN_C_DECLS

/*! 
 \brief get all the parameters for the given items. use tc_allItems() as argument to get all parameters 
 \param tc_items list of items for which the parameters are returned
 \return tc_matrix parameter values in the same order as the input list
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParameters(tc_items a);
/*! 
 \brief get initial values of the given items. Fixed varianbles are included. use tc_allItems() for all items in the model.
 \param tc_items list of items for which the initial values are returned
 \return tc_matrix initial values in the same order as the input list
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getInitialValues(tc_items a);
/*! 
 \brief set initial values of the given items. 
 \param tc_items list of items for which initial values are set
 \param tc_matrix the initial values in the same order as the list of items
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setInitialValues(tc_items items,tc_matrix values);
/*! 
 \brief get all fixed variables
 \param tc_items list of items for which fixed attribute are set
 \param tc_matrix matrix with 1 (fixed) or 0 (floating) in the same order as the list of items
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getFixedVariables(tc_items a);
/*! 
 \brief get all the parameters and fixed variables
 \param tc_items list of items. use tc_allItems() to get all items in the model
 \return tc_matrix list of parameters and fixed variables. order is not preserved from the input
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParametersAndFixedVariables(tc_items a);
/*! 
 \brief get the text attribute with the given name for the given item
 \param int item in the model, e.g. something returned from tc_find
 \param string name of the attribute
 \return string attribute
 \ingroup Annotation
*/
TCAPIEXPORT const char* tc_getTextAttribute(long item,const char* attribute);
/*! 
 \brief get the parameter with the given name for the given item
 \param int item in the model, e.g. something returned from tc_find
 \param string name of the parameter
 \return double value
 \ingroup Modeling
*/
TCAPIEXPORT double tc_getParameter(long item, const char* attribute);
/*! 
 \brief get all numerical Modeling with the given names for the given items
 \param tc_items a list of items
 \param tc_strings a list of parameter names that exist in one or more of the given items
 \return tc_matrix the set of parameters with rownames as parameter names
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParametersNamed(tc_items a,tc_strings attibutes);
/*! 
 \brief get all numerical Modeling EXCEPT the given names
 \param tc_items a list of items
 \param tc_strings a list of parameter names that exist in one or more of the given items
 \return tc_matrix the set of parameters with rownames as parameter names
 \ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getParametersExcept(tc_items a,tc_strings attributes);
/*! 
 \brief get all text Modeling with the given name for the given items
 \param tc_items a list of items
 \param tc_strings a list of text attribute name that exists in each of the given items
 \return tc_strings the set of all text attribute values, one for each item in the input
 \ingroup Annotation
*/
TCAPIEXPORT tc_strings tc_getAllTextNamed(tc_items a,tc_strings attributes);
/*! 
 \brief set text attribute for the given item
 \param int item in model
 \param string name of text attribute
 \ingroup Annotation
*/
TCAPIEXPORT void tc_setTextAttribute(long item,const char* attribute,const char* value);
/*! 
 \brief set a parameter value for the given item
 \param int item in model
 \param string name of parameter
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setParameter(long item, const char* attribute,double value);
/*! 
 \brief set text attribute 
 \param string full name of text attribute, e.g. A.sequence or A_sequence
 \param string value
 \ingroup Annotation
*/
TCAPIEXPORT void tc_setTextAttributeByName(const char* attribute,const char* value);
/*! 
 \brief set a parameter value 
 \param string full name of parameter, e.g. A.k0 or A_k0
 \param double value
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setParameterByName(const char* attribute,double value);
/*! 
 \brief set text attributes for multiple items
 \param tc_table table with rownames as the attribute full names
 \ingroup Annotation
*/
TCAPIEXPORT void tc_setTextAttributes(tc_table);
/*! 
 \brief set parameter for multiple items
 \param tc_table table with rownames as the parameter full names
 \param int 0=temporarily (just for simulation, fast), 1 = permanent (slower)
 \ingroup Modeling
*/
TCAPIEXPORT void tc_setParameters(tc_matrix parameters, int permanentOrTemporary);
/*! 
 \brief initialize the parameters and attributes plug-in
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

