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
	/******************
	LIBRARY THREAD
	*******************/
	typedef void (*TinkercellCEntryFunction)();
	
	void CThread::createProgressBarDialog()
	{
		QDialog * dialog = new QDialog(mainWindow);
		QHBoxLayout * layout = new QHBoxLayout;
		QLabel * label = new QLabel();
		QProgressBar * progressbar = new QProgressBar;
		layout->addWidget(label);
		layout->addWidget(progressbar);
		progressbar->setRange(0,100);
		dialog->setLayout(layout);
		dialog->hide();
		
		/*signals to control progress bar dialog*/
		connect(this,SIGNAL(setProgress(int)),progressbar,SLOT(setValue(int)));
		connect(this,SIGNAL(setTitle(const QString&)),label,SLOT(setText(const QString&)));
		connect(this,SIGNAL(showProgressBar()),dialog,SLOT(show()));
		connect(this,SIGNAL(hideProgressBar()),dialog,SLOT(hide()));
	}
	
	CThread::CThread(MainWindow * main, QLibrary * libPtr, bool autoUnload)
		: QThread(main), mainWindow(main), autoUnloadLibrary(autoUnload)
	{
		f1 = 0;
		f2 = 0;
		f3 = 0;
		f4 = 0;
		callbackPtr = 0;
		callWhenExitPtr = 0;
		setLibrary(libPtr);

		connect(this,SIGNAL(terminated()),this,SLOT(cleanupAfterTerminated()));
		
		cthreads << this;
		
		createProgressBarDialog();
		
		call_tc_main();
	}

	CThread::CThread(MainWindow * main, const QString & libName, bool autoUnload)
		: QThread(main), mainWindow(main), autoUnloadLibrary(autoUnload)
	{
		f1 = 0;
		f2 = 0;
		f3 = 0;
		f4 = 0;
		callbackPtr = 0;
		callWhenExitPtr = 0;
		this->lib = 0;
		setLibrary(libName);
		connect(this,SIGNAL(terminated()),this,SLOT(cleanupAfterTerminated()));

		cthreads << this;
		
		createProgressBarDialog();
		
		call_tc_main();
	}
	
	void CThread::call_tc_main()
	{
		if (!lib) return;
		
		TinkercellCEntryFunction f = (TinkercellCEntryFunction)lib->resolve(MainWindow::C_ENTRY_FUNCTION.toAscii().data());
		if (f)
			f();	
	}

	CThread::~CThread()
	{
		cthreads.removeAll(this);
	}

	typedef void (*VoidFunction)();

	typedef void (*IntFunction)(int);

	typedef void (*DoubleFunction)(double);

	typedef void (*CharFunction)(const char*);

	typedef void (*MatrixFunction)(tc_matrix);

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

	void CThread::setFunction( void (*f)(tc_matrix) )
	{
		f4 = f;
	}

	void CThread::setMatrixFunction( const char* f )
	{
		if (!lib) return;
		f4 = (MatrixFunction)lib->resolve(f);
	}

	typedef void (*cthread_api_initialize)(
		long cthread,
		void (*callback)(long, void (*f)(void)),
		void (*callWhenExiting)(long, void (*f)(void)),
		void (*showProgress)(long , const char *, int) );
		
	void CThread::setupCFunctionPointers(QLibrary * lib)
	{
		if (lib == 0)
			lib = this->lib;
		if (lib)
		{
			cthread_api_initialize f0 = (cthread_api_initialize)lib->resolve("tc_CThread_api_initialize");
			void * p = (void*)this;
			if (f0)
			{
				f0( 
					(long)(p),
					&(_setCallback),
					&(_setCallWhenExiting),
					&(_setProgress)
				);
			}
		}
	}
	
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
		
		setupCFunctionPointers();
	}

	void CThread::setLibrary(const QString& libname)
	{
		lib = loadLibrary(libname,this);
		
		bool loaded = lib && lib->isLoaded();
		
		if (mainWindow && loaded)
		{
			setLibrary(lib);
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
		QDir::setCurrent(MainWindow::tempDir());

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
			unload();
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

	void CThread::cleanupAfterTerminated()
	{
		if (autoUnloadLibrary)
		{
			unload();
		}
	}
	
	void CThread::update()
	{
		if (callbackPtr)
			callbackPtr();
	}
	
	void CThread::unload()
	{
		if (lib && lib->isLoaded())
		{
			if (callWhenExitPtr)
				callWhenExitPtr();
			lib->unload();
			lib = 0;
		}
	}

	/*
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
			connect(newThread,SIGNAL(progress(int)),progressbar,SLOT(setValue(int)));
			connect(newThread,SIGNAL(title(const QString&)),label1,SLOT(setText(const QString&)));
		}

		layout->addWidget(killButton);
		dialog->setWindowTitle(title);

		dialog->setLayout(layout);

		connect(killButton,SIGNAL(released()),newThread,SLOT(terminate()));
		connect(newThread,SIGNAL(finished()),dialog,SLOT(hide()));
		connect(newThread,SIGNAL(terminated()),dialog,SLOT(hide()));
		connect(newThread,SIGNAL(started()),dialog,SLOT(show()));
		newThread->hasDialog = true;
		
		return dialog;
	}*/

	QList<CThread*> CThread::cthreads;

	void CThread::_setCallback(long address,  void (*f)(void) )
	{
		void * ptr = (void*)address;
		CThread * thread = static_cast<CThread*>(ptr);
		if (cthreads.contains(thread))
		{
			thread->callbackPtr = f;
			connect(thread->mainWindow,SIGNAL(historyChanged(int)),thread,SLOT(update()));
		}
	}

	void CThread::_setCallWhenExiting(long address,  void (*f)(void) )
	{
		void * ptr = (void*)address;
		CThread * thread = static_cast<CThread*>(ptr);
		if (cthreads.contains(thread))
			thread->callWhenExitPtr = f;
	}

	void CThread::_setProgress(long address, const char * title, int progress)
	{
		void * ptr = (void*)address;
		CThread * thread = static_cast<CThread*>(ptr);
		if (cthreads.contains(thread))
		{
			thread->showProgress(QString(title), progress);
		}
	}
	
	void CThread::showProgress(const QString & title, int progress)
	{
		if (progress > 99)
		{
			emit hideProgressBar();
			return;
		}
		
		if (progress < 2)
			emit showProgressBar();
		
		if (title != _prevProgressBarTitle)
		{
			_prevProgressBarTitle = title;
			emit setTitle(title);
		}
		
		if (progress != _prevProgress)
		{
			_prevProgress = progress;
			emit setProgress(progress);
		}
	}

	QLibrary * CThread::loadLibrary(const QString& libname, QObject * parent)
	{
		QString  home = MainWindow::homeDir(),
			temp = MainWindow::tempDir(),
			current = QDir::currentPath(),
			appDir = QCoreApplication::applicationDirPath();

		QString name[] = {  
			libname,
			temp + QObject::tr("/") + libname,
			home + QObject::tr("/") + libname,
			current + QObject::tr("/") + libname,
			appDir + QObject::tr("/") + libname,
			};

		QLibrary * lib = new QLibrary(parent);

		bool loaded = false;
		for (int i=0; i < 5; ++i) //try different possibilities
		{
			lib->setFileName(name[i]);
			loaded = lib->load();
			if (loaded)
				break;
		}

		if (!loaded)
		{
			if (!lib->parent())
			{
				delete lib;
				lib = 0;
			}
		}
		
		return lib;
	}

	/******************
	PROCESS THREAD
	*******************/

	QWidget * ProcessThread::dialog(MainWindow * mainWindow, ProcessThread * newThread, const QString& text, QIcon icon)
	{
		QWidget * dialog = new QDialog(mainWindow);
		
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
			QDir::setCurrent(MainWindow::tempDir());

			//setPriority(QThread::LowestPriority);
			connect(this,SIGNAL(terminated()),&process,SLOT(kill()));
			process.start(exe,QStringList() << args);
			process.waitForFinished();
			errStream = process.readAllStandardError();
			outputStream = process.readAllStandardOutput();
			//ConsoleWindow::error(errors);

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
	
	QString ProcessThread::output() const
	{
		return outputStream;
	}
	
	QString ProcessThread::errors() const
	{
		return errStream;
	}
}
