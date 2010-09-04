#ifndef TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H
#define TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief get the event triggers for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT ArrayOfStrings tc_getEventTriggers();
/*! 
 \brief get the event responses for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT ArrayOfStrings tc_getEventResponses();
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
		ArrayOfStrings (*getEventTriggers)(),
		 ArrayOfStrings (*getEventResponses)(),
		 void (*addEvent)(const char*, const char*)
	);

/*! 
 \brief get the forcing function names for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT ArrayOfStrings tc_getForcingFunctionNames(ArrayOfItems a);
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Modeling
*/
TCAPIEXPORT ArrayOfStrings tc_getForcingFunctionAssignments(ArrayOfItems a);

/*! 
 \brief set the forcing function for an item
 \ingroup Modeling
*/
TCAPIEXPORT void tc_addForcingFunction(int item,const char* functionName, const char* assignmentRule);

/*! 
 \brief initialize
 \ingroup init
*/
TCAPIEXPORT void tc_AssignmentFunctionsTool_api(
		ArrayOfStrings (*getForcingFunctionNames)(ArrayOfItems),
		 ArrayOfStrings (*getForcingFunctionAssignments)(ArrayOfItems),
		 void (*addForcingFunction)(int,const char*, const char*)
	);
/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_displayText(int item,const char* text);
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_displayNumber(int item,double number);
/*! 
 \brief set the color for the number or text when using tc_displayNumber and tc_displayText
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_setDisplayLabelColor(const char* color1, const char* color2);
/*! 
 \brief highlights an item (the highlight is temporary) with the given color
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_highlight(int item,const char* color);

/*! 
 \brief initialize
 \ingroup init
*/
TCAPIEXPORT void tc_CLabelsTool_api(
		void (*displayText)(int item,const char*),
		void (*displayNumber)(int item,double),
		void (*setDisplayLabelColor)(const char* color1,const char* color2),
		void (*highlight)(int,const char* color)
	);

END_C_DECLS
#endif

