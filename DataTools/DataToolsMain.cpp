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
//     if (main->tools.contains(basicInformationTool1->name))
//         delete basicInformationTool1;
//     else
//         basicInformationTool1->setMainWindow(main);

    Tinkercell::BasicInformationTool * basicInformationTool2 = new Tinkercell::BasicInformationTool(QString("numerical"));
//     if (main->tools.contains(basicInformationTool2->name))
//         delete basicInformationTool2;
//     else
//         basicInformationTool2->setMainWindow(main);

    Tinkercell::AssignmentFunctionsTool * functionsTool = new Tinkercell::AssignmentFunctionsTool;
//     if (main->tools.contains(functionsTool->name))
//         delete functionsTool;
//     else
//         functionsTool->setMainWindow(main);

    Tinkercell::StoichiometryTool * stoichiometryTool = new Tinkercell::StoichiometryTool;
//     if (main->tools.contains(stoichiometryTool->name))
//         delete stoichiometryTool;
//     else
//         stoichiometryTool->setMainWindow(main);

    Tinkercell::ModelSummaryTool * modelSummary = new Tinkercell::ModelSummaryTool;
//     if (main->tools.contains(modelSummary->name))
//         delete modelSummary;
//     else
//         modelSummary->setMainWindow(main);

    Tinkercell::ModelFileGenerator * modelFileGenerator = new Tinkercell::ModelFileGenerator;
//     if (main->tools.contains(modelFileGenerator->name))
//         delete modelFileGenerator;
//     else
//         modelFileGenerator->setMainWindow(main);

    Tinkercell::NameFamilyDialog * nameFamilyDialog = new Tinkercell::NameFamilyDialog;
//     if (main->tools.contains(nameFamilyDialog->name))
//         delete nameFamilyDialog;
//     else
//         nameFamilyDialog->setMainWindow(main);

}

