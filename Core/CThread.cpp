/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the class that is used to create new threads in the 
Tinkercell main window. The threads can be associated with a dialog that provides
users with the option to terminate the thread.


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CThread.h"
#include "ConsoleWindow.h"
#include <QVBoxLayout>
#include <QDockWidget>
#include <QDir>
#include <QSemaphore>
#include <QCoreApplication>
#include <QtDebug>

namespace Tinkercell
{
	CThread::CThread(MainWindow * main, QLibrary * libPtr, bool autoUnload)
		: QThread(main), mainWindow(main), autoUnloadLibrary(autoUnload)
	{
		f1 = 0;
		f2 = 0;
		f3 = 0;
		f4 = 0;
		setLibrary(libPtr);
	}

	CThread::CThread(MainWindow * main, const QString & libName, bool autoUnload)
		: QThread(main), mainWindow(main), autoUnloadLibrary(autoUnload)
	{
		f1 = 0;
		f2 = 0;
		f3 = 0;
		f4 = 0;
		this->lib = 0;
		setLibrary(libName);
	}

	CThread::~CThread()
	{
		if (lib)
		{
			if (lib->isLoaded())
				lib->unload();
			delete lib;
			lib = 0;
		}
	}

	typedef void (*VoidFunction)();

	typedef void (*IntFunction)(int);

	typedef void (*DoubleFunction)(double);

	typedef void (*CharFunction)(const char*);

	typedef void (*MatrixFunction)(Matrix);

	void CThread::setFunction( void (*f)(void) )
	{
		f1 = f;
	}

	void CThread::setVoidFunction( const char* f)
	{
		if (!lib) return;
		f1 = (VoidFunction)lib->resolve(f);
	}

	void CThread::setFunction( void (*f)(double) )
	{
		f2 = f;
	}

	void CThread::setDoubleFunction(const char* f)
	{
		if (!lib) return;
		f2 = (DoubleFunction)lib->resolve(f);
	}

	void CThread::setFunction( void (*f)(const char*) )
	{
		f3 = f;
	}

	void CThread::setCharFunction( const char* f )
	{
		if (!lib) return;
		f3 = (CharFunction)lib->resolve(f);
	}

	void CThread::setFunction( void (*f)(Matrix) )
	{
		f4 = f;
	}

	void CThread::setMatrixFunction( const char* f )
	{
		if (!lib) return;
		f4 = (MatrixFunction)lib->resolve(f);
	}

	typedef void (*progress_api_initialize)(void (*tc_showProgress)(const char *, int));

	void CThread::setLibrary(QLibrary * lib)
	{
		this->lib = lib;
		if (mainWindow && lib)
		{
			QSemaphore * s = new QSemaphore(1);
			s->acquire();
			mainWindow->setupNewThread(s,lib);
			s->acquire();
			s->release();
		}

		if (lib)
		{
			progress_api_initialize f0 = (progress_api_initialize)lib->resolve("tc_Progress_api_initialize");
			if (f0)
			{
				f0(&(setProgress));
			}
		}
	}

	void CThread::setLibrary(const QString& libname)
	{
		QString  home = MainWindow::userHome(),
			current = QDir::currentPath(),
			appDir = QCoreApplication::applicationDirPath();

		QString name[] = {  
			libname,
			home + tr("/") + libname,
			current + tr("/") + libname,
			appDir + tr("/") + libname,
			};

		if (lib)
		{
			if (lib->isLoaded())
				lib->unload();
			delete lib;
		}

		lib = new QLibrary(this);

		bool loaded = false;
		for (int i=0; i < 4; ++i) //try different possibilities
		{
			lib->setFileName(name[i]);
			loaded = lib->load();
			if (loaded)
				break;
		}

		if (!loaded)
		{
			delete lib;
			lib = 0;
		}

		if (mainWindow && lib)
		{
			QSemaphore * s = new QSemaphore(1);
			s->acquire();
			mainWindow->setupNewThread(s,lib);
			s->acquire();
			s->release();
		}

		if (lib)
		{
			progress_api_initialize f0 = (progress_api_initialize)lib->resolve("tc_Progress_api_initialize");
			if (f0)
			{
				f0(&(setProgress));
			}
		}
	}

	QLibrary * CThread::library()
	{
		return lib;
	}

	void CThread::setAutoUnload(bool b)
	{
		autoUnloadLibrary = b;
	}

	bool CThread::autoUnload()
	{
		return autoUnloadLibrary;
	}

	void CThread::run()
	{
		QString current = QDir::currentPath();
		QDir::setCurrent(MainWindow::userHome());

		if (f1)
			f1();
		else
			if (f2)
				f2(argDouble);
			else
				if (f3)
					f3(ConvertValue(argString));
				else
					if (f4)
						f4(ConvertValue(argMatrix));

		QDir::setCurrent(current);

		if (lib && autoUnloadLibrary)
		{
			lib->unload();
		}
	}

	void CThread::setArg(double d)
	{
		argDouble = d;
	}

	void CThread::setArg(const QString& s)
	{
		argString = s;
	}

	void CThread::setArg(const DataTable<qreal>& dat)
	{
		argMatrix = dat;
	}

	QString CThread::style = QString("background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 1.0 #585858, stop: 0.5 #0E0E0E, stop: 0.5 #9A9A9A, stop: 1.0 #E2E2E2);");

	QWidget * CThread::dialog(CThread * newThread, const QString& title, const QIcon& icon, bool progressBar)
	{
		if (!newThread || !newThread->mainWindow) return 0;

		QWidget * dialog = new QWidget(newThread->mainWindow);
		dialog->setStyleSheet(CThread::style);


		dialog->move(newThread->mainWindow->pos() + QPoint(10,10));
		dialog->setWindowIcon(icon);

		QHBoxLayout * layout = new QHBoxLayout;
		QPushButton * killButton = new QPushButton("Terminate Program",dialog);
		killButton->setShortcut(QKeySequence(Qt::Key_Escape));

		dialog->setWindowFlags(Qt::Dialog);
		dialog->setAttribute(Qt::WA_DeleteOnClose,true);

		QLabel * label1 = new QLabel(title);
		layout->addWidget(label1);

		if (progressBar)
		{
			QProgressBar * progressbar = new QProgressBar;
			layout->addWidget(progressbar);
			progressbar->setRange(0,100);
			cthreads[title] = newThread;
			connect(newThread,SIGNAL(progress(int)),progressbar,SLOT(setValue(int)));
		}

		layout->addWidget(killButton);
		dialog->setWindowTitle(title);

		dialog->setLayout(layout);

		connect(killButton,SIGNAL(released()),newThread,SLOT(terminate()));
		connect(newThread,SIGNAL(finished()),dialog,SLOT(close()));
		connect(newThread,SIGNAL(started()),dialog,SLOT(show()));
		return dialog;
	}

	/******************
	LIBRARY THREAD
	*******************/

	QHash<QString,CThread*> CThread::cthreads;

	void CThread::setProgress(const char * name, int progress)
	{
		QString s(name);
		if (cthreads.contains(s) && cthreads[s])
		{
			cthreads[s]->emitSignal(progress);
		}
	}

	/******************
	PROCESS THREAD
	*******************/

	QWidget * ProcessThread::dialog(MainWindow * mainWindow, ProcessThread * newThread, const QString& text, QIcon icon)
	{
		QWidget * dialog = new QDialog(mainWindow);

		dialog->setStyleSheet(tr("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #B7D5FF, stop: 0.5 #FFFFFF, stop: 1.0 #093A7E);"));
		dialog->hide();

		dialog->move(newThread->mainWindow->pos() + QPoint(10,10));
		dialog->setWindowIcon(icon);

		QHBoxLayout * layout = new QHBoxLayout;
		QPushButton * killButton = new QPushButton("Terminate Program");
		connect(killButton,SIGNAL(released()),dialog,SLOT(accept()));
		QLabel * label = new QLabel(text + tr(" is Running..."));

		layout->addWidget(label);
		layout->addWidget(killButton);
		dialog->setWindowTitle(tr("Program Running"));

		dialog->setLayout(layout);
		dialog->setWindowFlags(Qt::Dialog);
		dialog->setAttribute(Qt::WA_DeleteOnClose,true);

		connect(killButton,SIGNAL(released()),newThread,SLOT(terminate()));
		connect(newThread,SIGNAL(finished()),dialog,SLOT(close()));
		connect(newThread,SIGNAL(started()),dialog,SLOT(show()));

		return dialog;
	}

	ProcessThread::ProcessThread(const QString& exe, const QString& args,MainWindow* main)
		: QThread(main)
	{
		this->args = args;
		this->exe = exe;
		mainWindow = main;

		connect(this,SIGNAL(terminated()),this,SLOT(stopProcess()));
		connect(this,SIGNAL(finished()),this,SLOT(stopProcess()));
	}

	void ProcessThread::run()
	{
		if (mainWindow && !exe.isEmpty())
		{
			QString current = QDir::currentPath();
			QDir::setCurrent(MainWindow::userHome());

			//setPriority(QThread::LowestPriority);
			connect(this,SIGNAL(terminated()),&process,SLOT(kill()));
			process.start(exe,QStringList() << args);
			process.waitForFinished();
			QString errors(process.readAllStandardError());
			QString output(process.readAllStandardOutput());
			ConsoleWindow::error(errors);

			QDir::setCurrent(current);
		}
	}

	void ProcessThread::stopProcess()
	{
		if (process.state() != QProcess::NotRunning)
			process.close();
	}

	ProcessThread::~ProcessThread()
	{
		stopProcess();
	}

}
