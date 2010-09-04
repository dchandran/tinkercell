#ifndef TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H
#define TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief get the event triggers for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getEventTriggers();
/*! 
 \brief get the event responses for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getEventResponses();
/*! 
 \brief set the event trigger and response
 \ingroup Modeling
*/
TCAPIEXPORT void tc_addEvent(const char* trigger, const char* event);
/*! 
 \brief initialize
 \ingroup init
*/
TCAPIEXPORT void tc_SimulationEventsTool_api(
		tc_strings (*getEventTriggers)(),
		 tc_strings (*getEventResponses)(),
		 void (*addEvent)(const char*, const char*)
	);

/*! 
 \brief get the forcing function names for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getForcingFunctionNames(tc_items a);
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getForcingFunctionAssignments(tc_items a);

/*! 
 \brief set the forcing function for an item
 \ingroup Modeling
*/
TCAPIEXPORT void tc_addForcingFunction(long item,const char* functionName, const char* assignmentRule);

/*! 
 \brief initialize
 \ingroup init
*/
TCAPIEXPORT void tc_AssignmentFunctionsTool_api(
		tc_strings (*getForcingFunctionNames)(tc_items),
		 tc_strings (*getForcingFunctionAssignments)(tc_items),
		 void (*addForcingFunction)(long,const char*, const char*)
	);
/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_displayText(long item,const char* text);
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_displayNumber(long item,double number);
/*! 
 \brief set the color for the number or text when using tc_displayNumber and tc_displayText
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_setDisplayLabelColor(const char* color1, const char* color2);
/*! 
 \brief highlights an item (the highlight is temporary) with the given color
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_highlight(long item,const char* color);

/*! 
 \brief initialize
 \ingroup init
*/
TCAPIEXPORT void tc_CLabelsTool_api(
		void (*displayText)(long item,const char*),
		void (*displayNumber)(long item,double),
		void (*setDisplayLabelColor)(const char* color1,const char* color2),
		void (*highlight)(long,const char* color)
	);

END_C_DECLS
#endif

