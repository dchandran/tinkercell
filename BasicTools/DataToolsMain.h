/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/
#include "Core/MainWindow.h"
#include "BasicTools/NameFamilyDialog.h"
#include "BasicTools/BasicInformationTool.h"
#include "BasicTools/StoichiometryTool.h"
#include "BasicTools/FunctionDeclarationsTool.h"
#include "BasicTools/ModelSummaryTool.h"
#include "BasicTools/ModelFileGenerator.h"

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
