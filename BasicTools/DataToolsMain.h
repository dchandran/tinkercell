/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/
#include "MainWindow.h"
#include "NameFamilyDialog.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "FunctionDeclarationsTool.h"
#include "ModelSummaryTool.h"
#include "ModelFileGenerator.h"

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
