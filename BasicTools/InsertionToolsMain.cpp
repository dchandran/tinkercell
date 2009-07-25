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
	main->addTool(collisionDetection);

	Tinkercell::ConnectionInsertion * connectionInsertion = new Tinkercell::ConnectionInsertion;
	main->addTool(connectionInsertion);

	Tinkercell::NodeInsertion * nodeInsertion = new Tinkercell::NodeInsertion;
	main->addTool(nodeInsertion);

}

