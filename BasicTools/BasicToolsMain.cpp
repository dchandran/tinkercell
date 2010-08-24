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

	Tinkercell::CollisionDetection * collisionDetection = new Tinkercell::CollisionDetection;
	main->addTool(collisionDetection);

	Tinkercell::ConnectionInsertion * connectionInsertion = new Tinkercell::ConnectionInsertion;
	main->addTool(connectionInsertion);

	Tinkercell::NodeInsertion * nodeInsertion = new Tinkercell::NodeInsertion;
	main->addTool(nodeInsertion);
	
	Tinkercell::NodeSelection * partSelection = new Tinkercell::NodeSelection;
	main->addTool(partSelection);

	Tinkercell::ConnectionSelection * connectionSelection = new Tinkercell::ConnectionSelection;
	main->addTool(connectionSelection);
	
	Tinkercell::TinkercellAboutBox * about = new Tinkercell::TinkercellAboutBox;
	main->addTool(about);
	
	Tinkercell::BasicGraphicsToolbox * basicGraphicsToolbox = new Tinkercell::BasicGraphicsToolbox;
	main->addTool(basicGraphicsToolbox);

	Tinkercell::GraphicsReplaceTool * graphicsReplaceTool = new Tinkercell::GraphicsReplaceTool;
	main->addTool(graphicsReplaceTool);

	Tinkercell::GraphicsTransformTool * graphicsTransformTool = new Tinkercell::GraphicsTransformTool;
	main->addTool(graphicsTransformTool);

	Tinkercell::GroupHandlerTool * groupHandlerTool = new Tinkercell::GroupHandlerTool;
	main->addTool(groupHandlerTool);

	Tinkercell::TextGraphicsTool * textGraphicsTool = new Tinkercell::TextGraphicsTool;
	main->addTool(textGraphicsTool);
	
	Tinkercell::NameFamilyDialog * nameFamilyDialog = new Tinkercell::NameFamilyDialog;
	main->addTool(nameFamilyDialog);

	Tinkercell::ConnectionMaker * connectionMaker = new Tinkercell::ConnectionMaker;
	main->addTool(connectionMaker);
	
	Tinkercell::LoadSaveTool * loadSaveTool = new Tinkercell::LoadSaveTool;
	main->addTool(loadSaveTool);

}

