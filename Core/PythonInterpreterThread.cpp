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

    PythonInterpreterThread::PythonInterpreterThread(const QString & dllname, MainWindow* main)
        : InterpreterThread(dllname,main)
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
        }
        mainWindow->statusBar()->showMessage(tr("Python initialized"));
    }

    void PythonInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;

        QString script;

		if (!addpathDone)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			QString homeDir = Tool::homeDir();

		#ifdef Q_WS_WIN
			QString pydir1 = appDir.replace("/","\\\\") + tr("\\\\") + PYTHON_FOLDER;
			QString pydir2 = homeDir.replace("/","\\\\") + tr("\\\\") + PYTHON_FOLDER;
		#else
			QString pydir1 = appDir + tr("/") + PYTHON_FOLDER;
			QString pydir2 = homeDir + tr("/") + PYTHON_FOLDER;
		#endif
			script = tr("import sys\nsys.path.append(\"") + pydir1 + tr("\")\n");
			if (QDir(homeDir + QObject::tr("/") + PYTHON_FOLDER).exists())
				script += tr("import sys\nsys.path.append(\"") + pydir2 + tr("\")\n");
			addpathDone = true;
		}
        
		script +=  QObject::tr("import sys\n_outfile = open('py.out','w')\nsys.stdout = _outfile;\n");
		script += code;
		script +=  QObject::tr("\n_outfile.close();\n");

        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());

            f(script.toAscii().data(),"py.out");
            if (mainWindow && mainWindow->console())
            {
            	QFile file(tr("py.out"));
            	if (file.open(QFile::ReadOnly | QFile::Text))
            	{
		            QString allText(file.readAll());
					mainWindow->console()->message(allText);
					file.close();
				}
            }

            QDir::setCurrent(currentDir);
        }
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

