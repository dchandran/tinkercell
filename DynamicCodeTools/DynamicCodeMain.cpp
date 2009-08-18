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
	
	QProcess proc;
    QString appDir = QCoreApplication::applicationDirPath();
	QString homeDir = Tinkercell::MainWindow::userHome();
	
	proc.setWorkingDirectory(homeDir);
	
	//build the odesim and ssa libraries for use by various C plug-ins (windows: generate .o files, not libs)
	
#ifdef Q_WS_WIN

	proc.start(QObject::tr("\"") + appDir + QObject::tr("\"\\win32\\tcc -r -I\"") + appDir + 
				("\"/win32/include -I\"") + appDir + ("\"/c -L\"") + 
				appDir + ("\"/win32/lib -o odesim.o \"") + 
				appDir + QObject::tr("\"/c/cvode_src/cvode/*.c \"") + 
				appDir + QObject::tr("\"/c/cvode_src/sundials/*.c \"") + 
				appDir + QObject::tr("\"/c/cvode_src/nvec_ser/*.c \"") + 
				appDir + QObject::tr("\"/c/cvodesim.c"));
	proc.waitForFinished();
	
	proc.start(QObject::tr("\"") + appDir + QObject::tr("\"\\win32\\tcc -r -I\"") + 
				appDir + ("\"/win32/include -I\"") + appDir + ("\"/c -L\"") + appDir + 
				("\"/win32/lib -o cells_ssa.o \"") + 
				appDir + QObject::tr("\"/c/mtrand.c \"") + 
				appDir + QObject::tr("\"/c/ssa.c \"") + 
				appDir + QObject::tr("\"/c/cells_ssa.c"));
	proc.waitForFinished();

#else	//if not windows, assume gcc exists

	proc.start(QObject::tr("gcc -c -o") + 
				appDir + QObject::tr("/c/cvode_src/cvode/*.c ") +
				appDir + QObject::tr("/c/cvode_src/sundials/*.c ") + 
				appDir + QObject::tr("/c/cvode_src/nvec_ser/*.c ") + 
				appDir + QObject::tr("/c/cvodesim.c"));
	proc.waitForFinished();
	
	proc.start(QObject::tr("ar -r libodesim.a *.o"));
	proc.waitForFinished();
	
	proc.start(QObject::tr("gcc -c -o ") +
				appDir + QObject::tr("/c/mtrand.c \"") + 
				appDir + QObject::tr("/c/ssa.c \"") + 
				appDir + QObject::tr("/c/cells_ssa.c"));
	proc.waitForFinished();
	
	proc.start(QObject::tr("ar -r libcells_ssa.a mtrand.o ssa.o cells_ssa.o"));
	proc.waitForFinished();
	
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

