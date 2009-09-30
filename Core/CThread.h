/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the class that is used to create new threads in the 
Tinkercell main window. The threads can be associated with a dialog that provides
users with the option to terminate the thread.


****************************************************************************/

#ifndef TINKERCELL_CTHREAD_H
#define TINKERCELL_CTHREAD_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QDialog>
#include <QCompleter>
#include <QListWidget>
#include <QThread>
#include <QToolBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QActionGroup>
#include <QLibrary>
#include <QProcess>
#include <QProgressBar>
#include <QItemDelegate>
#include "Tool.h"
#include "TCstructs.h"
#include "DataTable.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	/*! \brief This class is used to run specific functions inside a C dynamic library
	as a separate thread. The class can be used to load a library or just
	run a specific function inside an already loaded library. If the library is
	loaded by this class, the library will be unloaded upon completion on the
	function. To prevent the automatic unloading, use the setAutoUnload option.
	Only four types of functions are supported.
	\ingroup core
	*/
	class MY_EXPORT CThread : public QThread
	{
		Q_OBJECT

	signals:
		/*! \brief display progress of this thread (0-100). This signal is usually connected
		to a slot in ProgressBarSignalItem*/
		virtual void progress(int);

	public:

		static QString style;

		/*! \brief emits the progress signal*/
		virtual void emitSignal(int i) { emit progress(i); }

		/*! \brief set progress on a thread with the given name*/
		static void setProgress(const char * name, int progress);

		/*! \brief hash stores the name and progress bar pointers for updating progress on different threads*/
		static QHash<QString,CThread*> cthreads;

		/*!
		* \brief constructor
		* \param MainWindow the Tinkercell main window
		* \param QLibrary the dynamic library to load (optional)
		* \param bool whether or not to automatically unload the library
		*/
		CThread(MainWindow * main, QLibrary * lib = 0, bool autoUnload=false);

		/*!
		* \brief constructor
		* \param MainWindow the Tinkercell main window
		* \param QString the name of the dynamic library to load (optional)
		* \param bool whether or not to automatically unload the library
		*/
		CThread(MainWindow * main, const QString & lib = tr(""), bool autoUnload=false);

		/*! \brief destructor. unload and deletes the library*/
		virtual ~CThread();

		/*!
		* \brief set the function to run inside this threads
		* \param void function pointer
		*/
		virtual void setFunction( void (*f)(void) );
		/*!
		* \brief set the function to run inside this threads
		* \param void function pointer
		*/
		virtual void setFunction( void (*f)(double) );
		/*!
		* \brief set the function to run inside this threads
		* \param void function pointer
		*/
		virtual void setFunction( void (*f)(const char*) );
		/*!
		* \brief set the function to run inside this threads
		* \param void function pointer
		*/
		virtual void setFunction( void (*f)(Matrix) );
		/*!
		* \brief set the function to run inside this threads
		* \param void name of the function inside the library that has been loaded in this thread.
		*/
		virtual void setVoidFunction(const char*);
		/*!
		* \brief set the function to run inside this threads
		* \param void name of the function inside the library that has been loaded in this thread.
		*/
		virtual void setDoubleFunction(const char*);
		/*!
		* \brief set the function to run inside this threads
		* \param void name of the function inside the library that has been loaded in this thread.
		*/
		virtual void setCharFunction(const char*);
		/*!
		* \brief set the function to run inside this threads
		* \param void name of the function inside the library that has been loaded in this thread.
		*/
		virtual void setMatrixFunction(const char*);
		/*!
		* \brief set the dynamic library for this threads. 
		The library will be loaded if it has not already been loaded
		* \param QLibrary* library
		*/
		virtual void setLibrary(QLibrary*);
		/*!
		* \brief set the dynamic library for this threads.
		* \param QLibrary* library
		*/
		virtual void setLibrary(const QString&);
		/*!
		* \brief the library used inside this thread
		* \return QLibrary*
		*/
		virtual QLibrary * library();

		/*!
		* \brief set whether or not to automatically unload the library when the thread is done running
		* \param bool
		*/
		virtual void setAutoUnload(bool);
		/*!
		* \brief whether or not to automatically unload the library when the thread is done running
		* \return bool
		*/
		virtual bool autoUnload();
		/*!
		* \brief set the argument for the target function
		* \param double
		*/
		virtual void setArg(double);
		/*!
		* \brief set the argument for the target function
		* \param QString
		*/
		virtual void setArg(const QString&);
		/*!
		* \brief set the argument for the target function
		* \param DataTable
		*/
		virtual void setArg(const DataTable<qreal>&);

		/*!
		* \brief Creates a dialog with a progress bar for running a new thread. The dialog allows
		the user to terminate the thread.
		* \param CThread * target thread
		* \param QString display text for the dialog
		* \param QIcon display icon for the dialog
		* \param bool whether or not to show a progress bar
		*/
		static QWidget * dialog(CThread * , const QString& title, const QIcon& icon = QIcon(), bool progressBar = true);

		/*!
		* \brief main window
		*/
		MainWindow * mainWindow;

	protected:

		/*!
		* \brief whether or not to automatically unload the library when the thread is done running
		*/
		bool autoUnloadLibrary;
		/*!
		* \brief one of the functions that can be run inside this thread
		*/
		void (*f1)(void);
		/*!
		* \brief one of the functions that can be run inside this thread
		*/
		void (*f2)(double);
		/*!
		* \brief one of the functions that can be run inside this thread
		*/
		void (*f3)(const char*);
		/*!
		* \brief one of the functions that can be run inside this thread
		*/
		void (*f4)(Matrix);
		/*!
		* \brief the library where the functions are located that can be run inside this thread
		*/
		QLibrary * lib;
		/*!
		* \brief the argument for one of the the run function
		*/
		double argDouble;
		/*!
		* \brief the argument for one of the the run function
		*/
		QString argString;
		/*!
		* \brief the argument for one of the the run function
		*/
		DataTable<qreal> argMatrix;
		/*!
		* \brief the main function that runs one of the specified functions
		*/
		virtual void run();
	protected slots:
		/*!
		* \brief cleanup (such as unload libraries) upon termination
		*/
		virtual void cleanupAfterTerminated();
	};

	/*! \brief This class is used to run a process (command + args) as a separate thread as a separate thread
	\ingroup core
	*/
	class MY_EXPORT ProcessThread : public QThread
	{
		Q_OBJECT
	public:
		/*! \brief constructor -- used to initialize the main window, the command name and the args for the command
		* \param QString command
		* \param QString arguments
		* \param MainWindow main window
		*/
		ProcessThread(const QString&, const QString& ,MainWindow* main);
		/*! \brief destructor -- free the library that this thread loaded
		*/
		virtual ~ProcessThread();
		/*! \brief  creates a dialog that shows the name of the running thread and a button for terminating the thread
		* \param MainWindow main window
		* \param ProcessThread
		* \param QString text to display
		* \param QIcon icon to display
		*/
		static QWidget * dialog(MainWindow *, ProcessThread*, const QString& text = QString("Process"), QIcon icon = QIcon());
		protected slots:
			/*! \brief unload the library (if loaded) and delete it*/
			void stopProcess();
	protected:
		/*! \brief the name of the executable*/
		QString exe;
		/*! \brief the arguments*/
		QString args;
		/*! \brief Tinkercell's main window*/
		MainWindow * mainWindow;
		/*! \brief Tinkercell's main window*/
		QProcess process;
		/*! \brief initializes the function pointers through the main window and then runs the target function*/
		virtual void run();

	};

}

#endif
