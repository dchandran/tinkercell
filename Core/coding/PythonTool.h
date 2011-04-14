/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT
 
****************************************************************************/

#ifndef TINKERCELL_PYTHONTOOL_H
#define TINKERCELL_PYTHONTOOL_H

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

namespace Tinkercell
{
	class PythonTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void runPythonCode(QSemaphore*,const QString&);
			void runPythonFile(QSemaphore*,const QString&);
			void addPythonPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&);
		public slots:
			void runPythonCode(const char*);
			void runPythonFile(const char*);
			void addPythonPlugin(const char*,const char*,const char*,const char*,const char*);
	};

	class TINKERCELLEXPORT PythonTool : public Tool
	{
		Q_OBJECT

	public:
		PythonTool();
		bool setMainWindow(MainWindow*);
		PythonInterpreterThread * pythonInterpreter;

	public slots:
		void setupFunctionPointers( QLibrary * );
		bool loadFromDir( QDir& );
		bool loadFromDir(DynamicLibraryMenu* , QDir& );
		void toolLoaded(Tool*);
		void runPythonCode(const QString&);
		void runPythonFile(const QString&);
		void runPythonCode(QSemaphore*,const QString&);
		void runPythonFile(QSemaphore*,const QString&);
		
	protected slots:
		void buttonPressed (int);	 
		void actionTriggered(QAction *);
		void addPythonPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&);
	
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
        static void _addPythonPlugin(const char*,const char*,const char*,const char*,const char*);
	};
}

#endif

