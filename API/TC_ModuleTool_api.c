#include "TC_ModuleTool_api.h"

void (*_tc_substituteModel)(long, const char*) = 0;

TCAPIEXPORT 
void tc_substituteModel(long item, const char* filename)
{
	if (_tc_substituteModel)
		_tc_substituteModel(item,filename);
}

TCAPIEXPORT void tc_substituteEmptyModel(long item)
{
	if (_tc_substituteModel)
		_tc_substituteModel(item,"empty");
}

TCAPIEXPORT void tc_substituteOriginalModel(long item)
{
	if (_tc_substituteModel)
		_tc_substituteModel(item,"original");
}

tc_strings (*_tc_listOfPossibleModels)(long) = 0;

TCAPIEXPORT
tc_strings tc_listOfPossibleModels(long item)
{
	if (_tc_listOfPossibleModels)
		return _tc_listOfPossibleModels(item);
	return tc_createStringsArray(0);
}

/*!
 \brief initializing function
 \ingroup init
*/ TCAPIEXPORT 
void tc_ModuleTool_api(
	void (*substituteModel)(long, const char*),
	tc_strings (*listOfModels)(long))
{
	_tc_substituteModel = substituteModel;
	_tc_listOfPossibleModels = listOfModels;
}
