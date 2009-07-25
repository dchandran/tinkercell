/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

Function that loads dll into main window

****************************************************************************/

#include "VisualToolsMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

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


}

