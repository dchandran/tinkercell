/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

Function that loads dll into main window

****************************************************************************/
#include "BasicToolsMain.h"

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	main->addTool(new Tinkercell::CollisionDetection);
	main->addTool(new Tinkercell::ConnectionInsertion);
	main->addTool(new Tinkercell::NodeInsertion);
	main->addTool(new Tinkercell::NodeSelection);
	main->addTool(new Tinkercell::ConnectionSelection);
	main->addTool(new Tinkercell::TinkercellAboutBox);
	main->addTool(new Tinkercell::GraphicsReplaceTool);
	main->addTool(new Tinkercell::GraphicsTransformTool);
	main->addTool(new Tinkercell::GroupHandlerTool);
	main->addTool(new Tinkercell::NameFamilyDialog);
	main->addTool(new Tinkercell::ConnectionMaker);
}

