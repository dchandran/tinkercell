
/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The octave interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "OctaveInterpreterThread.h"

namespace Tinkercell
{
	QString OctaveInterpreterThread::OCTAVE_FOLDER("octave");
	
    OctaveInterpreterThread::OctaveInterpreterThread(const QString & octname, const QString & dllname, MainWindow* main)
        : InterpreterThread(OCTAVE_FOLDER + QObject::tr("/") + dllname,main)
    {
		addpathDone = false;
    	f = 0;
		swigLib = loadLibrary(OCTAVE_FOLDER + tr("/") + octname, mainWindow);
    }

    void OctaveInterpreterThread::setCPointers()
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
    
    void OctaveInterpreterThread::toolLoaded(Tool*)
    {
    	setCPointers();
    }
    
    void OctaveInterpreterThread::finalize()
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

    void OctaveInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			//qDebug() << "Octave interpreter: lib not loaded" << mainWindow << " " << lib;
			return;
		}

       QString appDir = QCoreApplication::applicationDirPath();

       initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
        	if (mainWindow->console())
	            mainWindow->console()->message(tr("Octave initialized"));
            QString currentDir = QDir::currentPath();

            QDir::setCurrent(MainWindow::tempDir());

            setCPointers();
            f();

            QDir::setCurrent(currentDir);
            
            mainWindow->statusBar()->showMessage(tr("Octave initialized"));
        }
    }
    
    void OctaveInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;
       
        QString script;
		
        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
        	QString tempDir = MainWindow::tempDir();
        	if (!addpathDone)
        	{
        		QString appDir = QCoreApplication::applicationDirPath();
		        QString homeDir = MainWindow::homeDir();
        	#ifdef Q_WS_WIN
        		appDir.replace("/","\\\\");
        		homeDir.replace("/","\\\\");
	        	script += QObject::tr("addpath(\"") + appDir + QObject::tr("\\\\") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	script += QObject::tr("addpath(\"") + homeDir + QObject::tr("\\\\") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	script += QObject::tr("addpath(\"") + tempDir + QObject::tr("\")\n");
	        #else
	        	script += QObject::tr("addpath(\"") + appDir + QObject::tr("/") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	script += QObject::tr("addpath(\"") + homeDir + QObject::tr("/") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	script += QObject::tr("addpath(\"") + tempDir + QObject::tr("\")\n");
        	#endif
	        	script += QObject::tr("tinkercell('global')\n");
	        	addpathDone = true;
	        	
	        	f(script.toAscii().data(),"octav.out","octav.err");
	        }
			
			script = code;

            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());
            
            QFile sourcefile(tempDir + QObject::tr("/temp.m"));
			if (sourcefile.open(QFile::WriteOnly))
			{
			    sourcefile.write(script.toAscii());
			    sourcefile.close();
			}

            f("source('temp.m')","octav.out","octav.err");
            if (mainWindow && mainWindow->console())
            {
            	QFile outfile(tr("octav.out"));
            	if (outfile.open(QFile::ReadOnly | QFile::Text))
            	{
		            QString allText(outfile.readAll());
		            if (!allText.isEmpty())
						mainWindow->console()->message(allText);
					outfile.close();
				}
				QFile errfile(tr("octav.err"));
            	if (errfile.open(QFile::ReadOnly | QFile::Text))
            	{
		            QString allText(errfile.readLine());
		            if (!allText.isEmpty() && !allText.contains(tr("octave_base_value::print")))
						mainWindow->console()->error(allText);
					errfile.close();
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
