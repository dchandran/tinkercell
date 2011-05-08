/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The octave interpreter that runs as a separate thread and can accept strings to parse and execute

****************************************************************************/
#include <iostream>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "OctaveInterpreterThread.h"

namespace Tinkercell
{
	QString OctaveInterpreterThread::OCTAVE_FOLDER("octave");
	QString OctaveInterpreterThread::OUTPUT_FILE("octav.out");
	QString OctaveInterpreterThread::ERROR_FILE("octav.err");
	
    OctaveInterpreterThread::OctaveInterpreterThread(const QString & swiglibname, const QString & dllname, MainWindow* main)
        : InterpreterThread(dllname,main)
    {
    	fromTC = QRegExp("([A-Za-z][A-Za-z0-9_]*)\\s*=\\s*fromTC\\s*\\(\\s*(\\s*[A-Za-z][A-Za-z0-9_]*\\s*)\\)");
		toTC = QRegExp("([A-Za-z][A-Za-z0-9_]*)\\s*=\\s*toTC\\s*\\(\\s*(\\s*[A-Za-z][A-Za-z0-9_]*\\s*)\\)");
		addpathDone = false;
    	f = 0;
		if (swiglibname.endsWith(QObject::tr(".oct")))
			swigLib = loadLibrary(swiglibname, mainWindow);
		else
			swigLib = loadLibrary(swiglibname + QObject::tr(".oct"), mainWindow);
		
		setupCFunctionPointers(swigLib);
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
            QDir::setCurrent(GlobalSettings::tempDir());
            f();
            QDir::setCurrent(currentDir);
        }
    }

    void OctaveInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			if (lib && mainWindow && mainWindow->console())
				mainWindow->console()->message("Failed to initialize Octave");
			else
				mainWindow->statusBar()->showMessage("Failed to initialize Octave");
			return;
		}

       QString appDir = QCoreApplication::applicationDirPath();

       initFunc f = (initFunc)lib->resolve("initialize");
        if (f)
        {
        	QString currentDir = QDir::currentPath();
            QDir::setCurrent(GlobalSettings::tempDir());

			setCPointers();
            f();

            QDir::setCurrent(currentDir);
			
            if (mainWindow->console())
	            mainWindow->console()->message(tr("Octave initialized"));
			else
				mainWindow->statusBar()->showMessage(tr("Octave initialized"));
        }
        else
        {
			if (lib && mainWindow)
				if (mainWindow->console())
					mainWindow->console()->message("Cannot find initialize function in Octave library");
				else
					mainWindow->statusBar()->showMessage("Cannot find initialize function in Octave library");
        }
    }
    
    void OctaveInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;
		
		if (fromTC.indexIn(code) > -1) //hack
		{
			QString m2 = fromTC.cap(1), m1 = fromTC.cap(2);
			QString s = m2 + QObject::tr(" = zeros(")+ m1 + QObject::tr(".rows,") + m1 + 
							QObject::tr(".cols); for i=1:") + m1 + QObject::tr(".rows for j=1:") + m1 + 
							QObject::tr(".cols (") + m2 + QObject::tr(")(i,j) = tinkercell.tc_getMatrixValue(") + m1 + 
							QObject::tr(",i-1,j-1); endfor endfor");
			code.replace(fromTC,s);
		}

		if (toTC.indexIn(code) > -1) //hack
		{
			QString m2 = toTC.cap(1), m1 = toTC.cap(2);
			QString s = m2 + QObject::tr(" = tinkercell.tc_createMatrix(size((")+ m1 + QObject::tr("),1), size((") + m1 + QObject::tr("),2)); ") + 
							QObject::tr("for i=1:size((") + m1 + QObject::tr("),1) for j=1:size((") + m1 + 
							QObject::tr("),2) tinkercell.tc_setMatrixValue(") + m2 + QObject::tr(",i-1,j-1,(") + m1 + 
							QObject::tr(")(i,j)); endfor endfor");
			code.replace(toTC,s);
		}
       
        QString script;
		
        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
        	QString tempDir = GlobalSettings::tempDir();
        	if (!addpathDone)
        	{
        		QString appDir = QCoreApplication::applicationDirPath();
				QString homeDir = GlobalSettings::homeDir();
				QString tempDir = GlobalSettings::tempDir();
			
				QStringList subdirs;
				subdirs << allSubdirectories(appDir + tr("/") + OCTAVE_FOLDER)
							<< allSubdirectories(homeDir + tr("/") + OCTAVE_FOLDER)
							<< tempDir;
			
				for (int i=0; i < subdirs.size(); ++i)
				{
					QString dir = subdirs[i];
					#ifdef Q_WS_WIN
						dir = dir.replace("/","\\\\");
					#endif
					script += tr("addpath(\"") + dir + tr("\")\n");
				}

	        	f(script.toAscii().data(),0,0);
	        }
		
		#ifdef Q_WS_WIN
			script = QObject::tr("diary on\n\n");
			script += code;
			script += QObject::tr("\n\ndiary off\n");
		#else
			script = code;
		#endif

            QString currentDir = QDir::currentPath();
            QDir::setCurrent(GlobalSettings::tempDir());
            
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
            f("source('temp.m')",0,ERROR_FILE.toAscii().data());
		#else
			f("source('temp.m')",OUTPUT_FILE.toAscii().data(),ERROR_FILE.toAscii().data());
			QFile outfile(OUTPUT_FILE);
		#endif
            if (mainWindow && mainWindow->console() && addpathDone)
            {
            	if (outfile.open(QFile::ReadOnly | QFile::Text))
            	{
		            QString allText(outfile.readAll());
		            if (!allText.isEmpty())
						mainWindow->console()->message(allText);
					outfile.close();
				}
				QFile errfile(ERROR_FILE);
            	if (errfile.open(QFile::ReadOnly | QFile::Text))
            	{
		            QString allText(errfile.readLine());
		            if (!allText.isEmpty() && !allText.contains(tr("octave_base_value::print")))
						mainWindow->console()->error(allText);
					errfile.close();
				}
            }
			
			addpathDone = true;
            QDir::setCurrent(currentDir);
        }
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

