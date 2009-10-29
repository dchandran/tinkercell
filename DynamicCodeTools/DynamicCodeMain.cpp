/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/
#include <QProcess>
#include "ConsoleWindow.h"
#include "DynamicCodeMain.h"

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
#ifdef Q_WS_WIN
	QProcess proc;
    QString appDir = QCoreApplication::applicationDirPath();
	QString homeDir = Tinkercell::MainWindow::userHome();
	
	proc.setWorkingDirectory(homeDir);
	homeDir.replace(QObject::tr("/"),QObject::tr("\\"));
	
	proc.start(QObject::tr("copy c\\*.dll ") + homeDir + QObject::tr(" /Y"));
	
#endif
	Tinkercell::DynamicLibraryMenu * libMenu = new Tinkercell::DynamicLibraryMenu;
	main->addTool(libMenu);
	
	Tinkercell::LoadCLibrariesTool * cLibraries = new Tinkercell::LoadCLibrariesTool;
	main->addTool(cLibraries);
		
	Tinkercell::PythonTool * pythonTool = new Tinkercell::PythonTool;
	main->addTool(pythonTool);
		
	Tinkercell::CodingWindow * cScriptWindow = new Tinkercell::CodingWindow;
	main->addTool(cScriptWindow);

}

