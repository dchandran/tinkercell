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
        : InterpreterThread(dllname,main)
    {
    	fromTC = QRegExp("([A-Za-z0-9_]+)\\s*=\\s*fromTC\\s*\\(\\s*(\\s*[A-Za-z0-9_]+\\s*)\\)");
		addpathDone = false;
    	f = 0;
		swigLib = loadLibrary(octname, mainWindow);
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
http://www.sciencemag.org/content/314/5805/1585.abstract
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
			if (lib && mainWindow && mainWindow->console())
				mainWindow->console()->message("Could not initialize Octave");
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
        else
        {
        	if (lib && mainWindow && mainWindow->console())
    	 		mainWindow->console()->message("Cannot find initialize function in Octave library");
        }
    }
    
    void OctaveInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;
		
	#ifdef Q_WS_WIN
		if (fromTC.indexIn(code) > -1) //hack
		{
			QString m2 = fromTC.cap(1), m1 = fromTC.cap(2);
			QString s = m2 + QObject::tr(" = zeros(")+ m1 + QObject::tr(".rows,") + m1 + 
							QObject::tr(".cols); for i=1:") + m1 + QObject::tr(".rows for j=1:") + m1 + 
							QObject::tr(".cols ") + m2 + QObject::tr("(i,j) = tinkercell.tc_getMatrixValue(") + m1 + 
							QObject::tr(",i-1,j-1); endfor endfor");
			code.replace(fromTC,s);
		}
	#endif
       
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
        		appDir.replace("/","\\\\");  //MS Windows just works differently
        		script += QObject::tr("addpath(\"") + appDir + QObject::tr("\\\\") + OCTAVE_FOLDER + QObject::tr("\")\n");
				if (QDir(homeDir).exists(OCTAVE_FOLDER))
				{
					homeDir.replace("/","\\\\");
					script += QObject::tr("addpath(\"") + homeDir + QObject::tr("\\\\") + OCTAVE_FOLDER + QObject::tr("\")\n");
				}
	        	script += QObject::tr("addpath(\"") + tempDir + QObject::tr("\")\n");
	        #else
	        	script += QObject::tr("addpath(\"") + appDir + QObject::tr("/") + OCTAVE_FOLDER + QObject::tr("\")\n");
				if (QDir(homeDir).exists(OCTAVE_FOLDER))
					script += QObject::tr("addpath(\"") + homeDir + QObject::tr("/") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	script += QObject::tr("addpath(\"") + tempDir + QObject::tr("\")\n");
        	#endif
	        	script += QObject::tr("tinkercell\n");
	        	addpathDone = true;
	        	
	        	f(script.toAscii().data(),"octav.out","octav.err");
	        }
		
		#ifdef Q_WS_WIN
			script += QObject::tr("diary on\n\n");
			script += code;
			script += QObject::tr("\n\ndiary off\n");
		#else
			script = code;
		#endif

            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());
            
            QFile sourcefile(tempDir + QObject::tr("/temp.m"));
			if (sourcefile.open(QFile::WriteOnly))
			{
			    sourcefile.write(script.toAscii());
			    sourcefile.close();
			}
      	#ifdef Q_WS_WIN
			QFile outfile(tr("diary"));
			if (outfile.open(QFile::WriteOnly))
			{
			    outfile.write(QString().toAscii());
			    outfile.close();
			}
            f("source('temp.m')",0,"octav.err"); //MS Windows just works differently
		#else
			f("source('temp.m')","octav.out","octav.err");
			QFile outfile(tr("octav.out"));
		#endif
            if (mainWindow && mainWindow->console())
            {
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
