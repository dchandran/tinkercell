/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The perl interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "PerlInterpreterThread.h"

namespace Tinkercell
{
    PerlInterpreterThread::PerlInterpreterThread(const QString & dllname, MainWindow* main)
        : CThread(main,dllname,false)
    {
        f = 0;
        disconnect(this);
        CThread::cthreads << this;
    }

    void PerlInterpreterThread::setCPointers()
    {
        if (!lib ||!mainWindow) return;
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        mainWindow->setupNewThread(s,lib);
		s->acquire();
        s->release();
    }

    void PerlInterpreterThread::runCode(const QString& code)
    {
        if (!mainWindow || !lib || !lib->isLoaded()) return;
		
		if (isRunning())
		{
			commandQueue.enqueue(code);
			return;
		}
		
		perlCode = code;

        start();
    }

    void PerlInterpreterThread::finalize()
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

    void PerlInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			qDebug() << "perl Interpreter: lib not loaded" << mainWindow << " " << lib;
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

    void PerlInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || perlCode.isEmpty()) return;

        QString code;
        code = QObject::tr("use IO::Handle\nopen OUTPUT, '>', \"pm.out\";\nopen ERROR,  '>', \"pm.out\";\n\n");
        code += perlCode;
        
        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::userTemp());

            f(code.toAscii().data(),"pm.out");

            QDir::setCurrent(currentDir);
        }
		
		if (!commandQueue.isEmpty())
		{
			perlCode = commandQueue.dequeue();
			run();
		}
    }

    PerlInterpreterThread::~PerlInterpreterThread()
    {
        finalize();
    }

}
