/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The python interpreter that runs as a separate thread and can accept strings to parse and execute

****************************************************************************/
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "PythonInterpreterThread.h"

namespace Tinkercell
{
	QString PythonInterpreterThread::PYTHON_FOLDER("python");
	QString PythonInterpreterThread::PYTHON_OUTPUT_FILE("py.out");

    PythonInterpreterThread::PythonInterpreterThread(const QString & dllname, MainWindow* main) :
#ifdef Q_WS_WIN
        InterpreterThread(dllname + QObject::tr(".pyd"),main)
#else
		InterpreterThread(dllname,main)
#endif
    {
		f = 0;
	    addpathDone = false;
    }
    
    void PythonInterpreterThread::finalize()
    {
        if (!lib || !lib->isLoaded()) return;

        finalFunc f = (finalFunc)lib->resolve("finalize");
        if (f)
        {
		        QString currentDir = QDir::currentPath();

		        QDir::setCurrent(GlobalSettings::tempDir());

		        f();

		        QDir::setCurrent(currentDir);
        }
    }

    void PythonInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			if (lib && mainWindow)
				if (mainWindow->console())
					mainWindow->console()->message("Failed to initialize Python");
				else
					mainWindow->statusBar()->showMessage("Failed to initialize Python");
			return;
		}

        QString appDir = QCoreApplication::applicationDirPath();

        initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
            QString currentDir = QDir::currentPath();

            QDir::setCurrent(GlobalSettings::tempDir());

            setCPointers();
            f();

            QDir::setCurrent(currentDir);

			if (mainWindow->console())
	            mainWindow->console()->message(tr("Python initialized"));
			else
				mainWindow->statusBar()->showMessage(tr("Python initialized"));
        }
		else
		{
			if (lib && mainWindow)
				if (mainWindow->console())
					mainWindow->console()->message("Cannot find initialize function in Python library");
				else
					mainWindow->statusBar()->showMessage("Cannot find initialize function in Python library");
		}
    }

    void PythonInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;

        QString script = QObject::tr("import sys\n");

		if (!addpathDone)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			QString homeDir = GlobalSettings::homeDir();
			QString tempDir = GlobalSettings::tempDir();
			
			QStringList subdirs;
			subdirs << allSubdirectories(appDir + tr("/") + PYTHON_FOLDER)
						<< allSubdirectories(homeDir + tr("/") + PYTHON_FOLDER)
						<< tempDir;
			
			for (int i=0; i < subdirs.size(); ++i)
			{
				QString dir = subdirs[i];
				#ifdef Q_WS_WIN
					dir = dir.replace("/","\\\\");
				#endif
				script += tr("sys.path.append(\"") + dir + tr("\")\n");
			}
			addpathDone = true;
		}
        
		script +=  QObject::tr("old_stdout = sys.stdout;\n_outfile = open('") + PYTHON_OUTPUT_FILE + QObject::tr("','w')\nsys.stdout = _outfile;\n");
		script += code;
		script +=  QObject::tr("\n_outfile.close();\nsys.stdout = old_stdout;\ntc_printFile('") + PYTHON_OUTPUT_FILE + QObject::tr("');\n");

        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(GlobalSettings::tempDir());

            f(script.toAscii().data());

            QDir::setCurrent(currentDir);
        }
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

