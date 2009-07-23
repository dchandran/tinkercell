/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 Provides a toolbar with buttons that call C functions (run of separate threads)
 
****************************************************************************/

#ifndef TINKERCELL_CFUNCTIONSMENULIST_H
#define TINKERCELL_CFUNCTIONSMENULIST_H

#include <QMainWindow>
#include <QHash>
#include <QToolButton>
#include <QSemaphore>
#include <QThread>
#include <QMenu>
#include <QFile>
#include <QActionGroup>
#include <QButtonGroup>
#include "Tool.h"
#include "DynamicLibraryMenu.h"
#include "PythonInterpreterThread.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class PythonTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void runPythonCode(QSemaphore*,const QString&);
			void runPythonFile(QSemaphore*,const QString&);
		public slots:
			void runPythonCode(const char*);
			void runPythonFile(const char*);
	};

	class MY_EXPORT PythonTool : public Tool
	{
		Q_OBJECT

	public:
		PythonTool();
		bool setMainWindow(MainWindow*);
		PythonInterpreterThread * pythonInterpreter;

	public slots:
		void setupFunctionPointers( QLibrary * );
		void loadFromFile(DynamicLibraryMenu* , QFile& file);
		void toolLoaded(Tool*);
		void runPythonCode(const QString&);
		void runPythonFile(const QString&);
		void runPythonCode(QSemaphore*,const QString&);
		void runPythonFile(QSemaphore*,const QString&);
		void stopPython();
		
	protected slots:
		void buttonPressed (int);	 
		void actionTriggered(QAction *);
	
	signals:
	
		void pythonStarted();
		void pythonFinished();

	protected:
		void connectTCFunctions();
		QActionGroup actionsGroup;
		QButtonGroup buttonsGroup;
		QStringList pyFileNames;
		QHash<QAction*,QString> hashPyFile;
		
	private:
		static PythonTool_FToS fToS;
		static void _runPythonCode(const char*);
                static void _runPythonFile(const char*);
	};
}

#endif
