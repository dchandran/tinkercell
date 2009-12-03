/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

Function that loads dll into main window

****************************************************************************/


#include "DataToolsMain.h"

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::BasicInformationTool * basicInformationTool1 = new Tinkercell::BasicInformationTool(QString("text"));
	main->addTool(basicInformationTool1);

	Tinkercell::BasicInformationTool * basicInformationTool2 = new Tinkercell::BasicInformationTool(QString("numerical"));
	main->addTool(basicInformationTool2);

	Tinkercell::AssignmentFunctionsTool * functionsTool = new Tinkercell::AssignmentFunctionsTool;
	main->addTool(functionsTool);

	Tinkercell::StoichiometryTool * stoichiometryTool = new Tinkercell::StoichiometryTool;
	main->addTool(stoichiometryTool);

	Tinkercell::ModelSummaryTool * modelSummaryTool = new Tinkercell::ModelSummaryTool;
	main->addTool(modelSummaryTool);
	
	Tinkercell::ModelFileGenerator * modelFileGenerator = new Tinkercell::ModelFileGenerator;
	main->addTool(modelFileGenerator);

}

