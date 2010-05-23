/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "InterpreterThread.h"

namespace Tinkercell
{
    InterpreterThread::InterpreterThread(const QString & dllname, MainWindow* main)
        : CThread(main,dllname,false)
    {
        disconnect(this);
        CThread::cthreads << this;
    }

    void InterpreterThread::setCPointers()
    {
        if (!lib ||!mainWindow) return;
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        mainWindow->setupNewThread(s,lib);
		s->acquire();
        s->release();
    }

    void InterpreterThread::exec(const QString& str)
    {
        if (!mainWindow || !lib || !lib->isLoaded()) return;
		
		if (isRunning())
		{
			codeQueue.enqueue(code);
			return;
		}
		
		code = str;

        start();
    }

    void InterpreterThread::finalize()
    {
    }

    void InterpreterThread::initialize()
    {
    	setCPointers();
    }

    void InterpreterThread::run()
    {
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

    InterpreterThread::~InterpreterThread()
    {
        finalize();
    }

}
