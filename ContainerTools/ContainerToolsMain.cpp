/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Function that loads dll into main window

****************************************************************************/

#include "ContainerToolsMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::ContainerTreeTool * containerTool = new Tinkercell::ContainerTreeTool;
// 	if (main->tools.contains(containerTool->name))
// 		delete containerTool;
// 	else
// 		containerTool->setMainWindow(main);

}

