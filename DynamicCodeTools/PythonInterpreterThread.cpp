/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

The python interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "OutputWindow.h"
#include "PythonInterpreterThread.h"

namespace Tinkercell
{

	 PythonInterpreterThread::PythonInterpreterThread(const QString& dll, const QString& func, MainWindow* main)
	 : LibraryThread(dll,func,main,emptyMatrix()), outputFile("py.out")
	 {
		disconnect(this);
		this->title = tr("runpy");
// 		progressBars[tr("runpy")] = progressBars[tr("runpy")];
// 		LibraryThread::progressBars[tr("runpy")] = ProgressBarSignalItem();
// 		connect(&LibraryThread::progressBars[tr("runpy")],SIGNAL(progress(int)),this,SIGNAL(progress(int)));
	 }

	 typedef void (*progress_api_initialize)(void (*tc_showProgress)(const char *, int));

	 void PythonInterpreterThread::setCPointers()
	 {
		if (!library ||!mainWindow) return;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		mainWindow->setupNewThread(s,library);
		progress_api_initialize f0 = (progress_api_initialize)library->resolve("tc_Progress_api_initialize");
		if (f0)
		{
// 			f0(&(LibraryThread::setProgress));
		}
		s->acquire();
		s->release();
	 }

	 typedef void (*initFunc)();
	 typedef void (*execFunc)(const char*,const char*);
	 typedef void (*finalFunc)();

	 void PythonInterpreterThread::runCode(const QString& code)
	 {
		pythonCode = code;

		if (!library || !library->isLoaded())
			initialize();

		if (isRunning()) return;
			//terminate();

		if (library->isLoaded())
			start();
	 }

	 void PythonInterpreterThread::finalize()
	 {
		if (!library || !library->isLoaded()) return;

		finalFunc f = (finalFunc)library->resolve("finalize");
		if (f)
		{
			QString currentDir = QDir::currentPath();
			QString tcdir("Tinkercell");
			QDir dir(QDir::home());
			if (!dir.exists(tcdir))
			{
				dir.mkdir(tcdir);
			}
			dir.cd(tcdir);
			QDir::setCurrent(dir.absolutePath());

			f();

			QDir::setCurrent(currentDir);
		 }
	 }

	 void PythonInterpreterThread::initialize()
	 {
		if (!mainWindow || dllFile.isEmpty()) return;

		QString appDir = QCoreApplication::applicationDirPath();
		#ifdef Q_WS_MAC
		appDir += tr("/../../..");
		#endif

		if (library) delete library;
		library = new QLibrary;

		QString name[] = {	MainWindow::userHome() + tr("/") + PROJECTNAME + tr("/") + dllFile,
					dllFile,
					QDir::currentPath() + tr("/") + dllFile,
					appDir + tr("/") + dllFile };

		bool loaded = false;
		for (int i=0; i < 4; ++i) //try different possibilities
		{
			library->setFileName(name[i]);
			loaded = library->load();
			if (loaded)
				break;
		}

		if (loaded)
		{
			initFunc f = (initFunc)library->resolve("initialize");
			if (f)
			{
				QString currentDir = QDir::currentPath();

				QString tcdir("Tinkercell");

				QDir dir(QDir::home());
				if (!dir.exists(tcdir))
				{
					dir.mkdir(tcdir);
				}

				dir.cd(tcdir);
				QDir::setCurrent(dir.absolutePath());

				setCPointers();
				f();

				QDir::setCurrent(currentDir);
			}
			mainWindow->statusBar()->showMessage(tr("Python initialized"));
		}
		else
		{
			mainWindow->statusBar()->showMessage(tr("Could not start Python"));
			OutputWindow::error(QString("Could not start Python"));
			OutputWindow::unfreeze();
		}
	 }

	 void PythonInterpreterThread::run()
	 {
		if (!library || !library->isLoaded() || pythonCode.isEmpty()) return;

		QString code;
		if (!outputFile.isEmpty())
		{
			code = QString("import sys\n_outfile = open('") + outputFile + QString("','w')\nsys.stdout = _outfile;\n");
			code += pythonCode;
			code += QString("\n_outfile.close();\n");
		}
		else
		{
			code = pythonCode;
		}

		execFunc f = (execFunc)library->resolve("exec");
		if (f)
		{
			QString currentDir = QDir::currentPath();
			QString tcdir("Tinkercell");
			QDir dir(QDir::home());
			if (!dir.exists(tcdir))
			{
				dir.mkdir(tcdir);
			}
			dir.cd(tcdir);
			QDir::setCurrent(dir.absolutePath());

			f(code.toAscii().data(),outputFile.toAscii().data());

			QDir::setCurrent(currentDir);
		}
	 }

	 PythonInterpreterThread::~PythonInterpreterThread()
	 {
		finalize();
		unloadLibrary();
	 }

}
