/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "JavaInterpreterThread.h"

namespace Tinkercell
{
	QString JavaInterpreterThread::JAVA_FOLDER("java");
	
    JavaInterpreterThread::JavaInterpreterThread(const QString & swiglibname, const QString & dllname, MainWindow* main)
        : InterpreterThread(dllname,main), regexp(QString("(\\S+)\\.(\\S+)\\(\\S+\\)"))
    {
    	f = 0;
		swigLib = loadLibrary(swiglibname, mainWindow);
		if (!swigLib)
		{
			if (main && main->console())
				main->console()->error("Could not load Java TinkerCell module");
		}
		else
		{
			if (main && main->console())
				main->console()->message("Java TinkerCell module loaded");
		}
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
            QDir::setCurrent(GlobalSettings::tempDir());
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
       
       std::cout << "finding init...\n\n";

       initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
        	std::cout << "found init...\n\n";
        	QString currentDir = QDir::currentPath();
            QDir::setCurrent(GlobalSettings::tempDir());
            setCPointers();
            
            QStringList paths;
    		QString appDir = QCoreApplication::applicationDirPath();
			QString homeDir = GlobalSettings::homeDir();
			QString tempDir = GlobalSettings::tempDir();
		
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
				paths << dir;
			}
			
			QString classpath(";");
			classpath += paths.join(";");
            
            int out = f(classpath.toAscii().data());

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
        
         if (!this->f)
            this->f = (execFunc)lib->resolve("exec");

        if (this->f)
        {
        	this->f("HelloWorld", "TestCall", "echo ba > out.txt");
        }
        else
        {
        	std::cout << "\nf not found\n\n";
        }
        
        std::cout << "done with init\n\n";
    }
    
    void JavaInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded())
        {
        	mainWindow->console()->error("JVM not loaded properly");
        	return;
        }
       
       //if (code.isEmpty()) return;
       
        QString script;
		
        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
			script = code.trimmed();
		
			/*if (regexp.indexIn(script) > -1)
			{
				QString classname = regexp.cap(1), 
							  methodname = regexp.cap(2), 
							  argval = regexp.cap(3);*/

				int k = f("HelloWorld", "TestCall", "hello"); //classname.toAscii().data(), methodname.toAscii().data(), argval.toAscii().data());
			/*}
			else
			{
				mainWindow->console()->error("Code can only contain one line: class.method(string)");
			}*/
		}
		else
		{
			mainWindow->console()->error("Cannot find some functions in Java loader library");
		}
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

