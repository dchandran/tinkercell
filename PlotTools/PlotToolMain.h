/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "MainWindow.h"
#include "PlotTool.h"
#include "GnuplotTool.h"

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);
