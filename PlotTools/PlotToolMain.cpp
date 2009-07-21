/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "Core/OutputWindow.h"
#include "PlotTools/PlotToolMain.h"

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	
	Tinkercell::PlotTool * plotTool = new Tinkercell::PlotTool;
	main->addTool(plotTool);
	/*Tinkercell::GraphStringTool * graphTool = new Tinkercell::GraphStringTool;
	main->addTool(graphTool);*/
}

