/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "MainWindow.h"
#include "LoadCLibraries.h"
#include "PythonTool.h"
#include "OctaveTool.h"
#include "CodingWindow.h"
#include "DynamicLibraryMenu.h"

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);
