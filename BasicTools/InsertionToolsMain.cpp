/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Function that loads dll into main window

****************************************************************************/

#include "InsertionToolsMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::CollisionDetection * collisionDetection = new Tinkercell::CollisionDetection;
// 	if (main->tools.contains(collisionDetection->name))
// 		delete collisionDetection;
// 	else
// 		collisionDetection->setMainWindow(main);

	Tinkercell::ConnectionInsertion * connectionInsertion = new Tinkercell::ConnectionInsertion;
// 	if (main->tools.contains(connectionInsertion->name))
// 		delete connectionInsertion;
// 	else
// 		connectionInsertion->setMainWindow(main);

	Tinkercell::NodeInsertion * nodeInsertion = new Tinkercell::NodeInsertion;
// 	if (main->tools.contains(nodeInsertion->name))
// 		delete nodeInsertion;
// 	else
// 		nodeInsertion->setMainWindow(main);

}

