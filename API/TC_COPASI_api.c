#include "TC_COPASI_api.h"


void (*_tc_updateCopasiModel)(copasi_model) = 0;
/*!
 \brief get the COPASI model from current TinkerCell model
 \param copasi_model* model pointer. This model will be updated
 \ingroup Export
*/
void tc_updateCopasiModel(copasi_model m)
{
	//model1(m);
	//if (_tc_updateCopasiModel)
		//_tc_updateCopasiModel(m);
}
/*!
 \brief initializing function
 \ingroup init
*/
void tc_COPASI_api( void (*updateCopasiModel)(copasi_model) )
{
	_tc_updateCopasiModel = updateCopasiModel;
}

