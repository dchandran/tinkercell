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

