/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Function that loads dll into main window

****************************************************************************/

#include "DynamicCodeMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	Tinkercell::DynamicLibraryMenu * libMenu = new Tinkercell::DynamicLibraryMenu;
// 	if (main->tools.contains(libMenu->name))
// 		delete libMenu;
// 	else
// 		libMenu->setMainWindow(main);

	Tinkercell::LoadCLibrariesTool * cLibraries = new Tinkercell::LoadCLibrariesTool;
// 	if (main->tools.contains(cLibraries->name))
// 		delete cLibraries;
// 	else
// 		cLibraries->setMainWindow(main);

	Tinkercell::PythonTool * pythonTool = new Tinkercell::PythonTool;
// 	if (main->tools.contains(pythonTool->name))
// 		delete pythonTool;
// 	else
// 		pythonTool->setMainWindow(main);

	Tinkercell::CodingWindow * cScriptWindow = new Tinkercell::CodingWindow;
// 	if (main->tools.contains(cScriptWindow->name))
// 		delete cScriptWindow;
// 	else
// 		cScriptWindow->setMainWindow(main);

}

