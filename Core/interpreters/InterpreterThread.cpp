/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/

#include "Tool.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "InterpreterThread.h"

namespace Tinkercell
{
    InterpreterThread::InterpreterThread(const QString & dllname, MainWindow* main)
        : CThread(main,dllname,false)
    {
		disconnect(this,SIGNAL(terminated()));
		CThread::cthreads << this;
		connect(main,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
    }
    
    void InterpreterThread::toolLoaded(Tool*)
    {
    	setCPointers();
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
    
    QStringList InterpreterThread::allSubdirectories(const QString& dir)
    {
    	QStringList subdirs;
    	QList<QDir> dirs;
    	dirs << QDir(dir);
    	for (int i=0; i < dirs.size(); ++i)
    		if (dirs[i].exists() && 
    			dirs[i].absolutePath().contains(dir) &&
    			!subdirs.contains(dirs[i].absolutePath()))
	    	{
	    		subdirs << dirs[i].absolutePath();
    			QFileInfoList list = dirs[i].entryInfoList();
    			for (int j=0; j < list.size(); ++j)
    				dirs << QDir(list[j].absoluteFilePath());
    		}
    	return subdirs;
	}
}
