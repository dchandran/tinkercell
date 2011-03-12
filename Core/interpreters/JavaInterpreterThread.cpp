/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The octave interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "JavaInterpreterThread.h"

namespace Tinkercell
{
	QString JavaInterpreterThread::JAVA_FOLDER("java");
	
    JavaInterpreterThread::JavaInterpreterThread(const QString & swiglibname, const QString & dllname, MainWindow* main)
        : InterpreterThread(dllname,main), regexp(QString("(\\S+)\\.(\\S+)\\(\"(\\S+)\"\\)"))
    {
    	addpathDone = false;
    	f = 0;
		swigLib = loadLibrary(swiglibname, mainWindow);
    }

    void JavaInterpreterThread::setCPointers()
    {
        if (!lib || !swigLib || !lib->isLoaded() || !swigLib->isLoaded() || !mainWindow) return;
       QSemaphore * s = new QSemaphore(1);
       s->acquire();
       mainWindow->setupNewThread(s,lib);
   	   s->acquire();
       s->release();       
       mainWindow->setupNewThread(s,swigLib);
   	   s->acquire();
       s->release();
    }
    
    void JavaInterpreterThread::toolLoaded(Tool*)
    {
    	setCPointers();
    }
    
    void JavaInterpreterThread::finalize()
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

    void JavaInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			if (lib && mainWindow && mainWindow->console())
				mainWindow->console()->message("Failed to open JVM loading library");
			else
				mainWindow->statusBar()->showMessage("Failed to open JVM loading library");
			return;
		}

       QString appDir = QCoreApplication::applicationDirPath();

       initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
        	QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());
            setCPointers();
            int out = f();

            QDir::setCurrent(currentDir);			
            if (out)
            {
	            if (mainWindow->console())
		            mainWindow->console()->message(tr("JVM initialized"));
				else
					mainWindow->statusBar()->showMessage(tr("JVM initialized"));
			}
			else
			{
				if (lib && mainWindow)
					if (mainWindow->console())
						mainWindow->console()->message("Failed to load JVM");
					else
						mainWindow->statusBar()->showMessage("Failed to load JVM");
			}
        }
        else
        {
			if (lib && mainWindow)
				if (mainWindow->console())
					mainWindow->console()->message("Cannot find initialize function in Java loading library");
				else
					mainWindow->statusBar()->showMessage("Cannot find initialize function in JVM loading library");
        }
    }
    
    void JavaInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;
       
        QString script;
		
        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
        	if (!addpathDone)
        	{
        		QStringList classpath;
        		QString appDir = QCoreApplication::applicationDirPath();
				QString homeDir = MainWindow::homeDir();
				QString tempDir = MainWindow::tempDir();
			
				QStringList subdirs;
				subdirs << allSubdirectories(appDir + tr("/") + JAVA_FOLDER)
							<< allSubdirectories(homeDir + tr("/") + JAVA_FOLDER)
							<< tempDir;
			
				for (int i=0; i < subdirs.size(); ++i)
				{
					QString dir = subdirs[i];
					#ifdef Q_WS_WIN
						dir = dir.replace("/","\\\\");
					#endif
					classpath << dir;
				}
				addpathDone = true;
				
				#ifdef Q_WS_WIN
					QString cmd("set CLASSPATH=$CLASSPATH;");
					cmd += classpath.join(";");
				#else
					QString cmd("export CLASSPATH=$CLASSPATH:");
					cmd += classpath.join(":");
				#endif
				system(cmd.toAscii().data());
	        }
	        
			script = code.trimmed();
		
			if (regexp.indexIn(script) > -1)
			{
				QString classname = regexp.cap(1), 
							  methodname = regexp.cap(2), 
							  argval = regexp.cap(3);

				f(classname.toAscii().data(), methodname.toAscii().data(), argval.toAscii().data());
			}
		}
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

