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
    PythonInterpreterThread::PythonInterpreterThread(const QString & dllname, MainWindow* main)
        : CThread(main,dllname,false), outputFile("py.out")
    {
        f = 0;
        disconnect(this);
        CThread::cthreads.insert( ++CThread::lastProgressMeterIndex,this);
    }

    void PythonInterpreterThread::setCPointers()
    {
        if (!lib ||!mainWindow) return;
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        mainWindow->setupNewThread(s,lib);
		s->acquire();
        s->release();
    }

    void PythonInterpreterThread::runCode(const QString& code)
    {
        if (!mainWindow || !lib || !lib->isLoaded()) return;
		
		if (isRunning())
		{
			commandQueue.enqueue(code);
			return;
		}
		
		pythonCode = code;

        start();
    }

    void PythonInterpreterThread::finalize()
    {
        if (!lib || !lib->isLoaded()) return;

        finalFunc f = (finalFunc)lib->resolve("finalize");
        if (f)
        {
            QString currentDir = QDir::currentPath();

            QDir::setCurrent(MainWindow::userTemp());

            f();

            QDir::setCurrent(currentDir);
        }
    }

    void PythonInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			qDebug() << "pyInterpreter: lib not loaded" << mainWindow << " " << lib;
			return;
		}

        QString appDir = QCoreApplication::applicationDirPath();

        initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
            QString currentDir = QDir::currentPath();

            QDir::setCurrent(MainWindow::userTemp());

            setCPointers();
            f();

            QDir::setCurrent(currentDir);
        }
        mainWindow->statusBar()->showMessage(tr("Python initialized"));
    }

    void PythonInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || pythonCode.isEmpty()) return;

        QString code;
        if (!outputFile.isEmpty())
        {
            code = QString("import sys\n_outfile = open('") + outputFile + QString("','w')\nsys.stdout = _outfile;\n");
            code += pythonCode;
            code += QString("\n_outfile.close();\n");
        }
        else
        {
            code = pythonCode;
        }

        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::userTemp());

            f(code.toAscii().data(),outputFile.toAscii().data());

            QDir::setCurrent(currentDir);
        }
		
		if (!commandQueue.isEmpty())
		{
			pythonCode = commandQueue.dequeue();
			run();
		}
    }

    PythonInterpreterThread::~PythonInterpreterThread()
    {
        finalize();
    }

}
