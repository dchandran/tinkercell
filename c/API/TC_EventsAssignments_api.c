#include "TC_EventsAssignments_api.h"

ArrayOfStrings (*_tc_getEventTriggers)() = 0;
/*! 
 \brief get the event triggers for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getEventTriggers()
{
	if (_tc_getEventTriggers)
		return _tc_getEventTriggers();
	return newArrayOfStrings(0);
}

ArrayOfStrings (*_tc_getEventResponses)() = 0;
/*! 
 \brief get the event responses for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getEventResponses()
{
	if (_tc_getEventResponses)
		return _tc_getEventResponses();
	return newArrayOfStrings(0);
}

void (*_tc_addEvent)(String trigger, String event) = 0;
/*! 
 \brief set the event trigger and response
 \ingroup Events and forcing functions
*/
void tc_addEvent(String trigger, String event)
{
	if (_tc_addEvent)
		_tc_addEvent(trigger,event);
}

/*! 
 \brief initialize
 \ingroup init
*/
void tc_SimulationEventsTool_api(
		ArrayOfStrings (*getEventTriggers)(),
		 ArrayOfStrings (*getEventResponses)(),
		 void (*addEvent)(String, String)
	)
{
	_tc_getEventTriggers = getEventTriggers;
	_tc_getEventResponses = getEventResponses;
	_tc_addEvent = addEvent;
}

ArrayOfStrings (*_tc_getForcingFunctionNames)(ArrayOfItems) = 0;
/*! 
 \brief get the forcing function names for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getForcingFunctionNames(ArrayOfItems a)
{
	if (_tc_getForcingFunctionNames)
		return _tc_getForcingFunctionNames(a);
	return newArrayOfStrings(0);
}

ArrayOfStrings (*_tc_getForcingFunctionAssignments)(ArrayOfItems) = 0;
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Events and forcing functions
*/
ArrayOfStrings tc_getForcingFunctionAssignments(ArrayOfItems a)
{
	if (_tc_getForcingFunctionAssignments)
		return _tc_getForcingFunctionAssignments(a);
	return newArrayOfStrings(0);
}

void (*_tc_addForcingFunction)(void* item,String functionName, String assignmentRule) = 0;
/*! 
 \brief set the forcing function for an item
 \ingroup Events and forcing functions
*/
void tc_addForcingFunction(void* item,String functionName, String assignmentRule)
{
	if (_tc_addForcingFunction)
		_tc_addForcingFunction(item,functionName,assignmentRule);
}

/*! 
 \brief initialize
 \ingroup init
*/
void tc_AssignmentFunctionsTool_api(
		ArrayOfStrings (*getForcingFunctionNames)(ArrayOfItems),
		 ArrayOfStrings (*getForcingFunctionAssignments)(ArrayOfItems),
		 void (*addForcingFunction)(void*,String, String)
	)
{
	_tc_getForcingFunctionNames = getForcingFunctionNames;
	_tc_getForcingFunctionAssignments = getForcingFunctionAssignments;
	_tc_addForcingFunction = addForcingFunction;
}

void (*_tc_displayText)(void* item,String text) = 0;
/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and Output
*/
void tc_displayText(void* item,String text)
{
	if (_tc_displayText)
		_tc_displayText(item,text);
}

void (*_tc_displayNumber)(void* item,double number) = 0;
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and Output
*/
void tc_displayNumber(void* item,double number)
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

void (*_tc_highlight)(void* item,int,int,int) = 0;
/*! 
 \brief highlights an item (the highlight is temporary) with the given color (rgb)
 \ingroup Input and Output
*/
void tc_highlight(void* item,int r,int g,int b)
{
	if (_tc_highlight)
		_tc_highlight(item,r,g,b);
}
/*! 
 \brief initialize
 \ingroup init
*/
void tc_CLabelsTool_api(
		void (*displayText)(void* item,String),
		void (*displayNumber)(void* item,double),
		void (*setDisplayLabelColor)(int r1,int g1,int b1,int r2,int g2,int b2),
		void (*highlight)(void*,int,int,int)
	)
{
	_tc_displayText = displayText;
	_tc_displayNumber = displayNumber;
	_tc_setDisplayLabelColor = setDisplayLabelColor;
	_tc_highlight = highlight;
}


