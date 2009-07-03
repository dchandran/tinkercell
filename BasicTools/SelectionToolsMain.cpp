/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Function that loads dll into main window

****************************************************************************/
#include "SelectionToolsMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::NodeSelection * partSelection = new Tinkercell::NodeSelection;
// 	if (main->tools.contains(partSelection->name))
// 		delete partSelection;
// 	else
// 		partSelection->setMainWindow(main);

	Tinkercell::ConnectionSelection * connectionSelection = new Tinkercell::ConnectionSelection;
// 	if (main->tools.contains(connectionSelection->name))
// 		delete connectionSelection;
// 	else
// 		connectionSelection->setMainWindow(main);

}

