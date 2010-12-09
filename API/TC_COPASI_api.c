#include "TC_COPASI_api.h"

copasi_model (*_tc_CopasiModel)() = 0;
/*!
 \brief get the COPASI model from current TinkerCell model
 \ingroup Export
*/
copasi_model tc_CopasiModel()
{
	copasi_model m = {0,0};
	if (_tc_CopasiModel)
		return _tc_CopasiModel();
	return m;
}
/*!
 \brief initializing function
 \ingroup init
*/
void tc_COPASI_api( copasi_model (*copasiModel)() )
{
	_tc_CopasiModel = copasiModel;
}

