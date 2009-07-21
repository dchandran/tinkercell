/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "DynamicCodeTools/DynamicCodeMain.h"

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	Tinkercell::DynamicLibraryMenu * libMenu = new Tinkercell::DynamicLibraryMenu;
	main->addTool(libMenu);
	
	Tinkercell::LoadCLibrariesTool * cLibraries = new Tinkercell::LoadCLibrariesTool;
	main->addTool(cLibraries);
		
	Tinkercell::PythonTool * pythonTool = new Tinkercell::PythonTool;
	main->addTool(pythonTool);
		
	Tinkercell::CodingWindow * cScriptWindow = new Tinkercell::CodingWindow;
	main->addTool(cScriptWindow);

}

