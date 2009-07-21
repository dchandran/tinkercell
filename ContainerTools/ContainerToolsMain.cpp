/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "ContainerTools/ContainerToolsMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	
	Tinkercell::ContainerTreeTool * containerTool = new Tinkercell::ContainerTreeTool;
	main->addTool(containerTool);

}

