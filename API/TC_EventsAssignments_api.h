#ifndef TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H
#define TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*! 
 \brief get the event triggers for a set of items
 \return tc_strings all event trigger equations, e.g. A > 10
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getEventTriggers();
/*! 
 \brief get the event responses for a set of items
 \return tc_strings all event trigger responses, e.g. A = 10; B = 2
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getEventResponses();
/*! 
 \brief set the event trigger and response
 \param string trigger, e.g. a > 2
 \param string response to trigger, e.g. x = 5
 \ingroup Modeling
*/
TCAPIEXPORT void tc_addEvent(const char* trigger, const char* event);
/*! 
 \brief initialize events plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_SimulationEventsTool_api(
		tc_strings (*getEventTriggers)(),
		 tc_strings (*getEventResponses)(),
		 void (*addEvent)(const char*, const char*)
	);

/*! 
 \brief get the forcing function names for a set of items
 \param tc_items list of items. use tc_allItems() to get all forcing functions
 \return tc_strings list of variable names
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getForcingFunctionNames(tc_items a);
/*! 
 \brief get the forcing function definitions for a set of items
 \param tc_items list of items. use tc_allItems() to get all forcing functions
 \return tc_strings list of assignment equations
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getForcingFunctionAssignments(tc_items a);

/*! 
 \brief set the forcing function for an item
 \param int address of an item, e.g. obtained from tc_find
 \param string name of existing variable or new variable
 \param string formula for the variable
 \ingroup Modeling
*/
TCAPIEXPORT void tc_addForcingFunction(long item,const char* variable, const char* formula);

/*! 
 \brief initialize forcing functions plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_AssignmentFunctionsTool_api(
		tc_strings (*getForcingFunctionNames)(tc_items),
		 tc_strings (*getForcingFunctionAssignments)(tc_items),
		 void (*addForcingFunction)(long,const char*, const char*)
	);

END_C_DECLS
#endif

