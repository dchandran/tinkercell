
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
		connect(this,SIGNAL(setupSwigLibrary( QLibrary * )),mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )));
		QLibrary * swig = CThread::loadLibrary(OCTAVE_FOLDER + tr("/") + octname, mainWindow);
		if (swig->isLoaded())
			emit setupSwigLibrary(swig);
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
        	if (!addpathDone)
        	{
        		QString appDir = QCoreApplication::applicationDirPath();
		        QString homeDir = MainWindow::homeDir();
        	#ifdef Q_WS_WIN
        		appDir.replace("/","\\\\");
        		homeDir.replace("/","\\\\");
	        	script = QObject::tr("addpath(\"") + appDir + QObject::tr("\\\\") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	if (QDir(homeDir + QObject::tr("/") + OCTAVE_FOLDER).exists())
		        	script += QObject::tr("addpath(\"") + homeDir + QObject::tr("\\\\") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        #else
	        
	        	script = QObject::tr("addpath(\"") + appDir + QObject::tr("/") + OCTAVE_FOLDER + QObject::tr("\")\n");
	        	if (QDir(homeDir + QObject::tr("/") + OCTAVE_FOLDER).exists())
		        	script += QObject::tr("addpath(\"") + homeDir + QObject::tr("/") + OCTAVE_FOLDER + QObject::tr("\")\n");
        	#endif
	        	script += QObject::tr("tinkercell('global')\n");
	        	addpathDone = true;
	        }
			
			mainWindow->console()->message(script);
			script += code;

            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());

            f(script.toAscii().data(),"octav.out","octav.err");
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
