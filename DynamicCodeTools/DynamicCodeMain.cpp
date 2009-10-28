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
	/*
	QProcess proc;
    QString appDir = QCoreApplication::applicationDirPath();
	QString homeDir = Tinkercell::MainWindow::userHome();
	
	proc.setWorkingDirectory(homeDir);
	QString err;
	
	//build the odesim and ssa libraries for use by various C plug-ins (windows: generate .o files, not libs)
	
#ifdef Q_WS_WIN
	
	// TCC cannot handle *.c in the file name when the directory has spaces in it -- strange, but have to deal with it
	QStringList filter;
	filter << "*.c";
	
	QDir dir1(appDir + QString("/c/cvode_src/cvode/"));
	QDir dir2(appDir + QString("/c/cvode_src/sundials/"));
	QDir dir3(appDir + QString("/c/cvode_src/nvec_ser/"));
	
	QFileInfoList files = dir1.entryInfoList(filter);
	files << dir2.entryInfoList(filter) << dir3.entryInfoList(filter);

	QStringList fileNames;
	for (int i=0; i < files.size(); ++i)
		fileNames << (QObject::tr("\"") + files[i].absolutePath() + QObject::tr("\\") + files[i].fileName() + QObject::tr("\""));
	
	QString cvodeFiles = fileNames.join(QObject::tr(" "));
	cvodeFiles.replace(QObject::tr("/"),QObject::tr("\\"));
	appDir.replace(QObject::tr("/"),QObject::tr("\\"));
	proc.start(QObject::tr("\"") + appDir + QObject::tr("\"\\win32\\tcc -r -I\"") + appDir + 
				("\"\\win32\\include -I\"") + appDir + QObject::tr("\"\\c -L\"") + 
				appDir + QObject::tr("\"\\win32\\lib -o odesim.o ") + cvodeFiles +
				QObject::tr(" \"") + appDir + QObject::tr("\"\\c\\cvodesim.c"));
	proc.waitForFinished();
	err = proc.readAllStandardError();
	if (!err.isEmpty())
		Tinkercell::ConsoleWindow::message(err);
	else
		Tinkercell::ConsoleWindow::message(QObject::tr("odesim.o created in ") + homeDir);
	
	proc.start(QObject::tr("\"") + appDir + QObject::tr("\"\\win32\\tcc -r -I\"") + 
				appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\c -L\"") + appDir + 
				("\"\\win32\\lib -o cells_ssa.o \"") + 
				appDir + QObject::tr("\"\\c\\mtrand.c \"") + 
				appDir + QObject::tr("\"\\c\\ssa.c \"") + 
				appDir + QObject::tr("\"\\c\\langevin.c \"") +
				appDir + QObject::tr("\"\\c\\cells_ssa.c"));
	proc.waitForFinished();
	err = proc.readAllStandardError();
	if (!err.isEmpty())
		Tinkercell::ConsoleWindow::message(err);
	else
		Tinkercell::ConsoleWindow::message(QObject::tr("cells_ssa.o created in ") + homeDir);
	

#else	//if not windows, assume gcc exists

	proc.start(QObject::tr("gcc -c -o") + 
				appDir + QObject::tr("/c/cvode_src/cvode/*.c ") +
				appDir + QObject::tr("/c/cvode_src/sundials/*.c ") + 
				appDir + QObject::tr("/c/cvode_src/nvec_ser/*.c ") + 
				appDir + QObject::tr("/c/cvodesim.c"));
	proc.waitForFinished();
	
	proc.start(QObject::tr("ar -r libodesim.a *.o"));
	proc.waitForFinished();
	err = proc.readAllStandardError();
	if (!err.isEmpty())
		Tinkercell::ConsoleWindow::message(err);
	else
		Tinkercell::ConsoleWindow::message(QObject::tr("odesim.a created in ") + homeDir);
	
	proc.start(QObject::tr("gcc -c -o ") +
				appDir + QObject::tr("/c/mtrand.c \"") + 
				appDir + QObject::tr("/c/ssa.c \"") + 
				appDir + QObject::tr("/c/langevin.c \"") + 
				appDir + QObject::tr("/c/cells_ssa.c"));
	proc.waitForFinished();
	
	proc.start(QObject::tr("ar -r libcells_ssa.a mtrand.o ssa.o cells_ssa.o"));
	proc.waitForFinished();
	if (!err.isEmpty())
		Tinkercell::ConsoleWindow::message(err);
	else
		Tinkercell::ConsoleWindow::message(QObject::tr("cells_ssa.a created in ") + homeDir);
	
#endif	
	*/
	Tinkercell::DynamicLibraryMenu * libMenu = new Tinkercell::DynamicLibraryMenu;
	main->addTool(libMenu);
	
	Tinkercell::LoadCLibrariesTool * cLibraries = new Tinkercell::LoadCLibrariesTool;
	main->addTool(cLibraries);
		
	Tinkercell::PythonTool * pythonTool = new Tinkercell::PythonTool;
	main->addTool(pythonTool);
		
	Tinkercell::CodingWindow * cScriptWindow = new Tinkercell::CodingWindow;
	main->addTool(cScriptWindow);

}

