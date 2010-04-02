#ifndef TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H
#define TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H

#include "../TCstructs.h"

/*! 
 \brief get the event triggers for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getEventTriggers();
/*! 
 \brief get the event responses for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getEventResponses();
/*! 
 \brief set the event trigger and response
 \ingroup Events and forcing functions
*/
void tc_addEvent(String trigger, String event);
/*! 
 \brief initialize
 \ingroup init
*/
void tc_SimulationEventsTool_api(
		ArrayOfStrings (*getEventTriggers)(),
		 ArrayOfStrings (*getEventResponses)(),
		 void (*addEvent)(String, String)
	);

/*! 
 \brief get the forcing function names for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getForcingFunctionNames(ArrayOfItems a);
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getForcingFunctionAssignments(ArrayOfItems a);

/*! 
 \brief set the forcing function for an item
 \ingroup Events and forcing functions
*/
void tc_addForcingFunction(Item item,String functionName, String assignmentRule);

/*! 
 \brief initialize
 \ingroup init
*/
void tc_AssignmentFunctionsTool_api(
		ArrayOfStrings (*getForcingFunctionNames)(ArrayOfItems),
		 ArrayOfStrings (*getForcingFunctionAssignments)(ArrayOfItems),
		 void (*addForcingFunction)(Item,String, String)
	);
/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and Output
*/
void tc_displayText(Item item,String text);
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and Output
*/
void tc_displayNumber(Item item,double number);
/*! 
 \brief set the color for the number or text when using tc_displayNumber and tc_displayText
 \ingroup Input and Output
*/
void tc_setDisplayLabelColor(int r1,int g1,int b1,int r2,int g2,int b2);
/*! 
 \brief highlights an item (the highlight is temporary) with the given color (rgb)
 \ingroup Input and Output
*/
void tc_highlight(Item item,int r,int g,int b);

/*! 
 \brief initialize
 \ingroup init
*/
void tc_CLabelsTool_api(
		void (*displayText)(Item item,String),
		void (*displayNumber)(Item item,double),
		void (*setDisplayLabelColor)(int r1,int g1,int b1,int r2,int g2,int b2),
		void (*highlight)(Item,int,int,int)
	);


#endif
