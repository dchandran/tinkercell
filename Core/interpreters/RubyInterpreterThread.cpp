/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The ruby interpreter that runs as a separate thread and can accept strings to parse and execute

****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "RubyInterpreterThread.h"

namespace Tinkercell
{
	QString RubyInterpreterThread::RUBY_FOLDER("ruby");
	QString RubyInterpreterThread::RUBY_OUTPUT_FILE("ruby.out");
	QString RubyInterpreterThread::RUBY_ERROR_FILE("ruby.err");

    RubyInterpreterThread::RubyInterpreterThread(const QString & dllname, MainWindow* main) :
		InterpreterThread(dllname,main)
    {
		f = 0;
	    addpathDone = false;
    }
    
    void RubyInterpreterThread::finalize()
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

    void RubyInterpreterThread::initialize()
    {
        if (!mainWindow || !lib || !lib->isLoaded())
		{
			if (lib && mainWindow)
				if (mainWindow->console())
					mainWindow->console()->message("Failed to initialize Ruby");
				else
					mainWindow->statusBar()->showMessage("Failed to initialize Ruby");
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
	            mainWindow->console()->message(tr("Ruby initialized"));
			else
				mainWindow->statusBar()->showMessage(tr("Ruby initialized"));
        }
		else
		{
			if (lib && mainWindow)
				if (mainWindow->console())
					mainWindow->console()->message("Cannot find initialize function in Ruby library");
				else
					mainWindow->statusBar()->showMessage("Cannot find initialize function in Ruby library");
		}
    }

    void RubyInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;

        QString script = QObject::tr("import sys\n");

		if (!addpathDone)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			QString homeDir = GlobalSettings::homeDir();
			QString tempDir = GlobalSettings::tempDir();
			
			QStringList subdirs;
			subdirs << allSubdirectories(appDir + tr("/") + RUBY_FOLDER)
						<< allSubdirectories(homeDir + tr("/") + RUBY_FOLDER)
						<< tempDir;
			
			for (int i=0; i < subdirs.size(); ++i)
			{
				QString dir = subdirs[i];
				#ifdef Q_WS_WIN
					dir = dir.replace("/","\\\\");
				#endif
				script += tr("$LOAD_PATH << \"") + dir + tr("\")\n");
			}
			addpathDone = true;
		}
        
		script +=  QObject::tr("old_stdout = $stdout;\nold_stderr = $stderr;\n_outfile = open('") + 
						RUBY_OUTPUT_FILE + QObject::tr("','w')\n$stdout = _outfile;\n_errfile = open('") + 
						RUBY_ERROR_FILE + QObject::tr("','w')\n$stderr = _errfile;\n");
		script += code;
		script +=  QObject::tr("\n_outfile.close();\n_errfile.close();\n$stdout = old_stdout;\n$stderr = old_stderr;\ntc_errorReport('ruby.err');\ntc_printFile('ruby.out');\n");

        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(GlobalSettings::tempDir());

            f(script.toAscii().data());

            QDir::setCurrent(currentDir);
        }
		
		if (!codeQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}

