#include "TC_EventsAssignments_api.h"

tc_strings (*_tc_getEventTriggers)() = 0;
/*! 
 \brief get the event triggers for a set of items
 \ingroup Events and forcing functions
*/ TCAPIEXPORT 
tc_strings tc_getEventTriggers()
{
	if (_tc_getEventTriggers)
		return _tc_getEventTriggers();
	return tc_createStringsArray(0);
}

tc_strings (*_tc_getEventResponses)() = 0;
/*! 
 \brief get the event responses for a set of items
 \ingroup Events and forcing functions
*/ TCAPIEXPORT 
tc_strings tc_getEventResponses()
{
	if (_tc_getEventResponses)
		return _tc_getEventResponses();
	return tc_createStringsArray(0);
}

void (*_tc_addEvent)(const char* trigger, const char* event) = 0;
/*! 
 \brief set the event trigger and response
 \ingroup Events and forcing functions
*/ TCAPIEXPORT 
void tc_addEvent(const char* trigger, const char* event)
{
	if (_tc_addEvent)
		_tc_addEvent(trigger,event);
}

/*! 
 \brief initialize
 \ingroup init
*/ TCAPIEXPORT 
void tc_SimulationEventsTool_api(
		tc_strings (*getEventTriggers)(),
		 tc_strings (*getEventResponses)(),
		 void (*addEvent)(const char*, const char*)
	)
{
	_tc_getEventTriggers = getEventTriggers;
	_tc_getEventResponses = getEventResponses;
	_tc_addEvent = addEvent;
}

tc_strings (*_tc_getForcingFunctionNames)(tc_items) = 0;
/*! 
 \brief get the forcing function names for a set of items
 \ingroup Events and forcing functions
*/ TCAPIEXPORT 
tc_strings tc_getForcingFunctionNames(tc_items a)
{
	if (_tc_getForcingFunctionNames)
		return _tc_getForcingFunctionNames(a);
	return tc_createStringsArray(0);
}

tc_strings (*_tc_getForcingFunctionAssignments)(tc_items) = 0;
/*! 
 \brief get the forcing function definitions for a set of items
 \ingroup Events and forcing functions
*/ TCAPIEXPORT 
tc_strings tc_getForcingFunctionAssignments(tc_items a)
{
	if (_tc_getForcingFunctionAssignments)
		return _tc_getForcingFunctionAssignments(a);
	return tc_createStringsArray(0);
}

void (*_tc_addForcingFunction)(long item,const char* functionName, const char* assignmentRule) = 0;
/*! 
 \brief set the forcing function for an item
 \ingroup Events and forcing functions
*/ TCAPIEXPORT 
void tc_addForcingFunction(long item,const char* functionName, const char* assignmentRule)
{
	if (_tc_addForcingFunction)
		_tc_addForcingFunction(item,functionName,assignmentRule);
}

/*! 
 \brief initialize
 \ingroup init
*/ TCAPIEXPORT 
void tc_AssignmentFunctionsTool_api(
		tc_strings (*getForcingFunctionNames)(tc_items),
		 tc_strings (*getForcingFunctionAssignments)(tc_items),
		 void (*addForcingFunction)(long,const char*, const char*)
	)
{
	_tc_getForcingFunctionNames = getForcingFunctionNames;
	_tc_getForcingFunctionAssignments = getForcingFunctionAssignments;
	_tc_addForcingFunction = addForcingFunction;
}

void (*_tc_displayText)(long item,const char* text) = 0;
/*! 
 \brief displays the given text on the given item (the text is temporary)
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_displayText(long item,const char* text)
{
	if (_tc_displayText)
		_tc_displayText(item,text);
}

void (*_tc_displayNumber)(long item,double number) = 0;
/*! 
 \brief displays the given number on the given item (the text is temporary)
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_displayNumber(long item,double number)
{
	if (_tc_displayNumber)
		_tc_displayNumber(item,number);
}

void (*_tc_setDisplayLabelColor)(const char *, const char *) = 0;
/*! 
 \brief set the color for the number or text when using tc_displayNumber and tc_displayText
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_setDisplayLabelColor(const char * a, const char * b)
{
	if (_tc_setDisplayLabelColor)
		_tc_setDisplayLabelColor(a,b);
}

void (*_tc_highlight)(long item,const char*) = 0;
/*! 
 \brief highlights an item (the highlight is temporary) with the given color (hex)
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_highlight(long item,const char* color)
{
	if (_tc_highlight)
		_tc_highlight(item,color);
}
/*! 
 \brief initialize
 \ingroup init
*/ TCAPIEXPORT 
void tc_CLabelsTool_api(
		void (*displayText)(long item,const char*),
		void (*displayNumber)(long item,double),
		void (*setDisplayLabelColor)(const char *, const char *),
		void (*highlight)(long,const char*)
	)
{
	_tc_displayText = displayText;
	_tc_displayNumber = displayNumber;
	_tc_setDisplayLabelColor = setDisplayLabelColor;
	_tc_highlight = highlight;
}


