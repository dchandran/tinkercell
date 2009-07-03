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
// 	if (main->tools.contains(basicGraphicsToolbox->name))
// 		delete basicGraphicsToolbox;
// 	else
// 		basicGraphicsToolbox->setMainWindow(main);


	Tinkercell::CopyPasteTool * copyPasteTool = new Tinkercell::CopyPasteTool;
// 	if (main->tools.contains(copyPasteTool->name))
// 		delete copyPasteTool;
// 	else
// 		copyPasteTool->setMainWindow(main);

	Tinkercell::GraphicsTransformTool * graphicsTransformTool = new Tinkercell::GraphicsTransformTool;
// 	if (main->tools.contains(graphicsTransformTool->name))
// 		delete graphicsTransformTool;
// 	else
// 		graphicsTransformTool->setMainWindow(main);

	Tinkercell::GroupHandlerTool * groupHandlerTool = new Tinkercell::GroupHandlerTool;
// 	if (main->tools.contains(groupHandlerTool->name))
// 		delete groupHandlerTool;
// 	else
// 		groupHandlerTool->setMainWindow(main);


	Tinkercell::TextGraphicsTool * textGraphicsTool = new Tinkercell::TextGraphicsTool;
// 	if (main->tools.contains(textGraphicsTool->name))
// 		delete textGraphicsTool;
// 	else
// 		textGraphicsTool->setMainWindow(main);



}

