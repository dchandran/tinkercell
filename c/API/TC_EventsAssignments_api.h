#ifndef TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H
#define TINKERCELL_TC_EVENTS_AND_ASSIGNMENTS_API_H

#include "../TCstructs.h"

char** (*_tc_getEventTriggers)(Array) = 0;
/*! 
 \brief get the event triggers for a set of items
 \ingroup Events and forcing functions
*/
char** tc_getEventTriggers(Array a)
{
	if (_tc_getEventTriggers)
		return _tc_getEventTriggers(a);
	return 0;
}

char** (*_tc_getEventResponses)(Array) = 0;
/*! 
 \brief get the event responses for a set of items
 \ingroup Events and forcing functions
*/
char** tc_getEventResponses(Array a)
{
	if (_tc_getEventResponses)
		return _tc_getEventResponses(a);
	return 0;
}

void (*_tc_addEvent)(OBJ item,const char* trigger, const char* event) = 0;
/*! 
 \brief set the event trigger and response for an item
 \ingroup Events and forcing functions
*/
void tc_addEvent(OBJ item,const char* trigger, const char* event)
{
	if (_tc_addEvent)
		_tc_addEvent(item,trigger,event);
}

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
	_tc_getEventTriggers = getEventTriggers;
	_tc_getEventResponses = getEventResponses;
	_tc_addEvent = addEvent;
}

char** (*_tc_getForcingFunctionNames)(Array) = 0;
/*! 
 \brief get the forcing function names for a set of items
 \ingroup Events and forcing functions
*/
char** tc_getForcingFunctionNames(Array a)
{
	if (_tc_getForcingFunctionNames)
		return _tc_getForcingFunctionNames(a);
	return 0;
}

char** (*_tc_getForcingFunctionAssignments)(Array) = 0;
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Events and forcing functions
*/
char** tc_getForcingFunctionAssignments(Array a)
{
	if (_tc_getForcingFunctionAssignments)
		return _tc_getForcingFunctionAssignments(a);
	return 0;
}

void (*_tc_addForcingFunction)(OBJ item,const char* functionName, const char* assignmentRule) = 0;
/*! 
 \brief set the forcing function for an item
 \ingroup Events and forcing functions
*/
void tc_addForcingFunction(OBJ item,const char* functionName, const char* assignmentRule)
{
	if (_tc_addForcingFunction)
		_tc_addForcingFunction(item,functionName,assignmentRule);
}

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
	_tc_getForcingFunctionNames = getForcingFunctionNames;
	_tc_getForcingFunctionAssignments = getForcingFunctionAssignments;
	_tc_addForcingFunction = addForcingFunction;
}

void (*_tc_displayText)(OBJ item,const char* text) = 0;
/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and Output
*/
void tc_displayText(OBJ item,const char* text)
{
	if (_tc_displayText)
		_tc_displayText(item,text);
}

void (*_tc_displayNumber)(OBJ item,double number) = 0;
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and Output
*/
void tc_displayNumber(OBJ item,double number)
{
	if (_tc_displayNumber)
		_tc_displayNumber(item,number);
}

void (*_tc_setDisplayLabelColor)(int r1,int g1,int b1,int r2,int g2,int b2) = 0;
/*! 
 \brief set the color for the number or text when using tc_displayNumber and tc_displayText
 \ingroup Input and Output
*/
void tc_setDisplayLabelColor(int r1,int g1,int b1,int r2,int g2,int b2)
{
	if (_tc_setDisplayLabelColor)
		_tc_setDisplayLabelColor(r1,g1,b1,r2,g2,b2);
}

void (*_tc_highlight)(OBJ item,int,int,int) = 0;
/*! 
 \brief highlights an item (the highlight is temporary) with the given color (rgb)
 \ingroup Input and Output
*/
void tc_highlight(OBJ item,int r,int g,int b)
{
	if (_tc_highlight)
		_tc_highlight(item,r,g,b);
}
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
	_tc_displayText = displayText;
	_tc_displayNumber = displayNumber;
	_tc_setDisplayLabelColor = setDisplayLabelColor;
	_tc_highlight = highlight;
}


#endif
