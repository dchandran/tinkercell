/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Function that loads dll into main window

****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include "DynamicCodeMain.h"

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	QString appDir = QCoreApplication::applicationDirPath();
	QString homeDir = Tinkercell::MainWindow::homeDir();
	QString tempDir = Tinkercell::MainWindow::tempDir();

#ifdef Q_WS_WIN

	tempDir.replace(QObject::tr("/"),QObject::tr("\\"));
	appDir.replace(QObject::tr("/"),QObject::tr("\\"));
	homeDir = QObject::tr("\"") + homeDir.replace(QObject::tr("/"),QObject::tr("\\")) + QObject::tr("\"");

	QString s1(QObject::tr("copy \"") + appDir + QObject::tr("\"\\win32\\*.* \"") + tempDir + QObject::tr("\" /Y"));
	system(s1.toAscii().data());

#endif

	QString s;
	if (!QFile::exists(homeDir + QObject::tr("/.svn")))
	{
		s = QObject::tr("svn co https://tinkercellextra.svn.sourceforge.net/svnroot/tinkercellextra ") + homeDir;
		system(s.toAscii().data());
	}
	
	s = QObject::tr("cd ") + homeDir + QObject::tr("; svn update");
	system(s.toAscii().data());

	Tinkercell::DynamicLibraryMenu * libMenu = new Tinkercell::DynamicLibraryMenu;
	main->addTool(libMenu);

	Tinkercell::LoadCLibrariesTool * cLibraries = new Tinkercell::LoadCLibrariesTool;
	main->addTool(cLibraries);

	Tinkercell::PythonTool * pythonTool = new Tinkercell::PythonTool;
	main->addTool(pythonTool);
	
	Tinkercell::OctaveTool * octaveTool = new Tinkercell::OctaveTool;
	main->addTool(octaveTool);

	Tinkercell::CodingWindow * cScriptWindow = new Tinkercell::CodingWindow;
	main->addTool(cScriptWindow);

}

