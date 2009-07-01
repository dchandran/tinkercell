#ifndef TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H
#define TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H

#include "../TCstructs.h"

/*! 
 \brief get the event triggers for a set of items
 \ingroup Events and forcing functions
*/
char** (*tc_getEventTriggers)(Array);
/*! 
 \brief get the event responses for a set of items
 \ingroup Events and forcing functions
*/
char** (*tc_getEventResponses)(Array);
/*! 
 \brief set the event trigger and response for an item
 \ingroup Events and forcing functions
*/
void (*tc_addEvent)(OBJ item,const char* trigger, const char* event);

/*! 
 \brief initialize
 \ingroup init
*/
void tc_SimulationEventsTool_api(
		char** (*getEventTriggers)(Array),
		 char** (*getEventResponses)(Array),
		 void (*addEvent)(OBJ,const char*, const char*)
	)
{
	tc_getEventTriggers = getEventTriggers;
	tc_getEventResponses = getEventResponses;
	tc_addEvent = addEvent;
}

/*! 
 \brief get the forcing function names for a set of items
 \ingroup Events and forcing functions
*/
char** (*tc_getForcingFunctionNames)(Array);
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Events and forcing functions
*/
char** (*tc_getForcingFunctionAssignments)(Array);
/*! 
 \brief set the forcing function for an item
 \ingroup Events and forcing functions
*/
void (*tc_addForcingFunction)(OBJ item,const char* functionName, const char* assignmentRule);

/*! 
 \brief initialize
 \ingroup init
*/
void tc_AssignmentFunctionsTool_api(
		char** (*getForcingFunctionNames)(Array),
		 char** (*getForcingFunctionAssignments)(Array),
		 void (*addForcingFunction)(OBJ,const char*, const char*)
	)
{
	tc_getForcingFunctionNames = getForcingFunctionNames;
	tc_getForcingFunctionAssignments = getForcingFunctionAssignments;
	tc_addForcingFunction = addForcingFunction;
}

/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and output
*/
void (*tc_displayText)(OBJ item,const char* text);
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and output
*/
void (*tc_displayNumber)(OBJ item,double number);
/*! 
 \brief set the color for the number or text when using tc_displayNumber and tc_displayText
 \ingroup Input and output
*/
void (*tc_setDisplayLabelColor)(int r1,int g1,int b1,int r2,int g2,int b2);
/*! 
 \brief highlights an item (the highlight is temporary)
 \ingroup Input and output
*/
void (*tc_highlight)(OBJ item,int,int,int);
/*! 
 \brief initialize
 \ingroup init
*/
void tc_CLabelsTool_api(
		void (*displayText)(OBJ item,const char*),
		void (*displayNumber)(OBJ item,double),
		void (*setDisplayLabelColor)(int r1,int g1,int b1,int r2,int g2,int b2),
		void (*highlight)(OBJ,int,int,int)
	)
{
	tc_displayText = displayText;
	tc_displayNumber = displayNumber;
	tc_setDisplayLabelColor = setDisplayLabelColor;
	tc_highlight = highlight;
}


#endif
