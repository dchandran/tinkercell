/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The python interpreter that runs as a separate thread and can accept strings to parse and execute

****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
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

		        QDir::setCurrent(MainWindow::tempDir());

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
					mainWindow->console()->message("Could not initialize Python");
				else
					mainWindow->statusBar()->showMessage("Could not initialize Python");
			return;
		}

        QString appDir = QCoreApplication::applicationDirPath();

        initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
            QString currentDir = QDir::currentPath();

            QDir::setCurrent(MainWindow::tempDir());

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
			QString homeDir = MainWindow::homeDir();
			QString tempDir = MainWindow::tempDir();

		#ifdef Q_WS_WIN
			QString pydir1 = appDir.replace("/","\\\\") + tr("\\\\") + PYTHON_FOLDER;
			QString pydir2 = homeDir.replace("/","\\\\") + tr("\\\\") + PYTHON_FOLDER;
			QString pydir3 = tempDir.replace("/","\\\\");
		#else
			QString pydir1 = appDir + tr("/") + PYTHON_FOLDER;
			QString pydir2 = homeDir + tr("/") + PYTHON_FOLDER;
			QString pydir3 = tempDir;
		#endif
			script += tr("sys.path.append(\"") + pydir1 + tr("\")\n");
			if (QDir(homeDir).exists(PYTHON_FOLDER))
				script += tr("sys.path.append(\"") + pydir2 + tr("\")\n");
			script += tr("sys.path.append(\"") + pydir3 + tr("\")\n");
			addpathDone = true;
		}
        
		script +=  QObject::tr("_outfile = open('") + PYTHON_OUTPUT_FILE + QObject::tr("','w')\nsys.stdout = _outfile;\nsys.stderr = _outfile;\n");
		script += code;
		script +=  QObject::tr("\n_outfile.close();\ntc_printFile('py.out');\n");

        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());

            f(script.toUtf8().data());

            QDir::setCurrent(currentDir);
        }
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

